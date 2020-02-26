QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about_dlg.cpp \
    auto_update_service.cpp \
    i18n_helper.cpp \
    mac_sparkle_support.mm \
    main.cpp \
    mainwindow.cpp \
    man_applet.cpp \
    man_tree_item.cpp \
    man_tree_model.cpp \
    man_tree_view.cpp \
    settings_dlg.cpp \
    settings_mgr.cpp

HEADERS += \
    about_dlg.h \
    auto_update_service.h \
    i18n_helper.h \
    mac_sparkle_support.h \
    mainwindow.h \
    man_applet.h \
    man_tree_item.h \
    man_tree_model.h \
    man_tree_view.h \
    settings_dlg.h \
    settings_mgr.h \
    singleton.h

FORMS += \
    about_dlg.ui \
    mainwindow.ui \
    settings_dlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
