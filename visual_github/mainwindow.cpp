#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <git2.h>
#include <QDebug>
#include <QDateTime>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->saveCredentialsButton, &QPushButton::clicked, this, &MainWindow::saveCredentials);

    loadCredentials(); // Pour pré-remplir les champs au démarrage
    loadLogHistory("/home/antoinecarlin/Bureau/Obsidian/Hacking/tryhackme/tryhackme/Challenges TryHackMe");

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

void MainWindow::saveCredentials()
{
    QSettings settings("MonStudio", "VisualGit");

    settings.setValue("github/username", ui->usernameEdit->text());
    settings.setValue("github/token", ui->tokenEdit->text());

    // Petite astuce visuelle : afficher un message temporaire dans la barre d'état
    ui->statusbar->showMessage("Identifiants sauvegardés avec succès !", 3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}
