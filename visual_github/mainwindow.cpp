#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <git2.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    git_repository *repo = nullptr;

    // Test with local repo
    const char *repo_path = "/home/antoinecarlin/Bureau/Obsidian/Hacking/tryhackme/tryhackme/Challenges TryHackMe";

    int error = git_repository_open(&repo, repo_path);

    if (error < 0) {
        const git_error *e = git_error_last();
        qDebug() << "Failed to open Git repository:" << e->message;
    } else {
        qDebug() << "Success";
        git_repository_free(repo); // Toujours libérer la mémoire des objets libgit2
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
