#include "mainwindow.h"

#include <QApplication>
#include <git2.h>

int main(int argc, char *argv[])
{
    git_libgit2_init(); //init
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    auto f = a.exec();
    git_libgit2_shutdown();

    return f;
}
