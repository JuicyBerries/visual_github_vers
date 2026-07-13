#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <git2.h>
#include <QDebug>
#include <QDateTime>
#include <QSettings>
#include <QFileDialog>

// Petite structure pour faire transiter les identifiants jusqu'au callback libgit2
struct GitHubCredentials {
    QByteArray username;
    QByteArray token;
};

static int credentialsCallback(git_credential **out,
                               const char * /*url*/,
                               const char * /*username_from_url*/,
                               unsigned int allowed_types,
                               void *payload)
{
    auto *creds = static_cast<GitHubCredentials *>(payload);
    if (allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) {
        return git_credential_userpass_plaintext_new(out, creds->username.constData(), creds->token.constData());
    }

    return GIT_EUSER;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->saveCredentialsButton, &QPushButton::clicked, this, &MainWindow::saveCredentials);
    connect(ui->testConnectionButton, &QPushButton::clicked, this, &MainWindow::testCredentials);
    connect(ui->browseRepoButton, &QPushButton::clicked, this, &MainWindow::browseRepo);

    loadCredentials(); // Pour pré-remplir les champs au démarrage

    // On récupère le dernier dépôt utilisé, s'il y en a un
    QSettings settings("MonStudio", "VisualGit");
    QString savedRepoPath = settings.value("repo/path").toString();

    if (!savedRepoPath.isEmpty()) {
        ui->repoPathEdit->setText(savedRepoPath);
        loadLogHistory(savedRepoPath.toUtf8().constData());
    }
}

void MainWindow::browseRepo()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "Sélectionner le dossier du dépôt Git",
        ui->repoPathEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (dir.isEmpty()) {
        return; // L'utilisateur a annulé
    }

    ui->repoPathEdit->setText(dir);

    // On sauvegarde le choix pour le prochain démarrage
    QSettings settings("MonStudio", "VisualGit");
    settings.setValue("repo/path", dir);

    loadLogHistory(dir.toUtf8().constData());
}

void MainWindow::loadLogHistory(const char *repo_path)
{
    ui->commitLogList->clear(); // delete mocked data

    git_repository *repo = nullptr;

    // 1. Open repo
    if (git_repository_open(&repo, repo_path) == 0) {
        git_revwalk *walker = nullptr;
        git_revwalk_new(&walker, repo);

        git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);

        // HEAD
        git_revwalk_push_head(walker);

        git_oid oid; // Commit hash

        // 2. Get all commits
        while (git_revwalk_next(&oid, walker) == 0) {
            git_commit *commit = nullptr;

            // On récupère l'objet commit à partir de son identifiant
            if (git_commit_lookup(&commit, repo, &oid) == 0) {

                // A. Extraire le Hash court (7 caractères)
                char short_hash[8] = {0};
                git_oid_tostr(short_hash, sizeof(short_hash), &oid);

                // B. Extraire l'auteur et le message
                const git_signature *author = git_commit_author(commit);
                const char *message = git_commit_summary(commit);
                QDateTime dateTime = QDateTime::fromSecsSinceEpoch(git_commit_time(commit));

                // C. Formater la ligne pour l'interface Qt
                QString logText = QString("%1 | %2 | %3 | %4").arg(short_hash).arg(message).arg(author->name).arg(dateTime.toString("dd/MM/yyyy 'at' HH:mm"));

                // D. Ajouter la ligne dans ton QListWidget
                ui->commitLogList->addItem(logText);

                // Toujours libérer le commit une fois qu'on a fini avec
                git_commit_free(commit);
            }
        }

        // 3. Nettoyage
        git_revwalk_free(walker);
        git_repository_free(repo);

    } else {
        const git_error *e = git_error_last();
        qDebug() << "Erreur d'ouverture du dépôt Git :" << (e ? e->message : "Inconnue");
    }
}

void MainWindow::loadCredentials()
{
    // "MonStudio" et "VisualGit" définissent le nom du dossier et du fichier de config
    QSettings settings("MonStudio", "VisualGit");

    ui->usernameEdit->setText(settings.value("github/username").toString());
    ui->tokenEdit->setText(settings.value("github/token").toString());
}

void MainWindow::testCredentials()
{
    const QString username = ui->usernameEdit->text().trimmed();
    const QString token = ui->tokenEdit->text().trimmed();
    const QString repoPath = ui->repoPathEdit->text().trimmed();

    if (username.isEmpty() || token.isEmpty()) {
        ui->statusbar->showMessage("Fill username/password before testing.", 4000);
        return;
    }

    if (repoPath.isEmpty()) {
        ui->statusbar->showMessage("Select a repository.", 4000);
        return;
    }

    git_repository *repo = nullptr;
    if (git_repository_open(&repo, repoPath.toUtf8().constData()) != 0) {
        const git_error *e = git_error_last();
        ui->statusbar->showMessage(
            QString("Impossible d'ouvrir le dépôt : %1").arg(e ? e->message : "Unknown error"), 5000);
        return;
    }

    git_remote *remote = nullptr;
    if (git_remote_lookup(&remote, repo, "main") != 0) {
        ui->statusbar->showMessage("No remote \"main\" configurated on this repo.", 5000);
        git_repository_free(repo);
        return;
    }

    GitHubCredentials creds{username.toUtf8(), token.toUtf8()};

    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
    callbacks.credentials = credentialsCallback;
    callbacks.payload = &creds;

    ui->testConnectionButton->setEnabled(false);
    ui->statusbar->showMessage("Verification...");

    const int connectResult = git_remote_connect(remote, GIT_DIRECTION_PUSH, &callbacks, nullptr, nullptr);

    if (connectResult == 0) {
        ui->statusbar->showMessage(
            QString("Successful connexion at %1 !").arg(QString::fromUtf8(git_remote_url(remote))), 5000);
        git_remote_disconnect(remote);
    } else {
        const git_error *e = git_error_last();
        ui->statusbar->showMessage(
            QString("Échec de l'authentification : %1").arg(e ? e->message : "Unknown error"), 6000);
    }

    ui->testConnectionButton->setEnabled(true);

    git_remote_free(remote);
    git_repository_free(repo);
}

void MainWindow::saveCredentials()
{
    QSettings settings("MonStudio", "VisualGit");

    settings.setValue("github/username", ui->usernameEdit->text());
    settings.setValue("github/token", ui->tokenEdit->text());

    // Petite astuce visuelle : afficher un message temporaire dans la barre d'état
    ui->statusbar->showMessage("Credentials successfully saved !", 3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}
