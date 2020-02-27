#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "man_applet.h"
#include "i18n_helper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName( "JS" );
    QCoreApplication::setOrganizationDomain( "jssoft.com");
    QCoreApplication::setApplicationName( "CCMan" );

    I18NHelper::getInstance()->init();

    ManApplet mApplet;
    manApplet = &mApplet;
    manApplet->start();

    return a.exec();
}
