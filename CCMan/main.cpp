#include <QFile>

#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "man_applet.h"
#include "i18n_helper.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName( "JS" );
    QCoreApplication::setOrganizationDomain( "jssoft.com");
    QCoreApplication::setApplicationName( "CCMan" );

    QFile qss(":/ccman.qss");
    qss.open( QFile::ReadOnly );
    app.setStyleSheet(qss.readAll());

    I18NHelper::getInstance()->init();

    ManApplet mApplet;
    manApplet = &mApplet;
    manApplet->start();

    QFont font;
#ifdef Q_OS_MAC
    QString strFont = "Monaco";
#else
    QString strFont = "Consolas";
#endif

    font.setFamily( strFont );
    app.setFont( font );

    return app.exec();
}
