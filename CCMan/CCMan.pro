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
    account_info.cpp \
    auto_update_service.cpp \
    i18n_helper.cpp \
    login_dlg.cpp \
    main.cpp \
    mainwindow.cpp \
    man_applet.cpp \
    man_tree_item.cpp \
    man_tree_model.cpp \
    man_tree_view.cpp \
    reg_user_dlg.cpp \
    search_menu.cpp \
    settings_dlg.cpp \
    settings_mgr.cpp

HEADERS += \
    about_dlg.h \
    account_info.h \
    auto_update_service.h \
    i18n_helper.h \
    login_dlg.h \
    mainwindow.h \
    man_applet.h \
    man_tree_item.h \
    man_tree_model.h \
    man_tree_view.h \
    reg_user_dlg.h \
    search_menu.h \
    settings_dlg.h \
    settings_mgr.h \
    singleton.h

FORMS += \
    about_dlg.ui \
    login_dlg.ui \
    mainwindow.ui \
    reg_user_dlg.ui \
    settings_dlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += "../../PKILib"

DEFINES += _AUTO_UPDATE

mac {
    QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../Frameworks
    HEADERS += mac_sparkle_support.h
    OBJECTIVE_SOURCES += mac_sparkle_support.mm
    LIBS += -framework AppKit
    LIBS += -framework Carbon
    LIBS += -framework Foundation
    LIBS += -framework ApplicationServices
    LIBS += -framework Sparkle
    INCLUDEPATH += "/usr/local/Sparkle.framework/Headers"

    INCLUDEPATH += "../../PKILib/lib/mac/debug/cmpossl/include"
    INCLUDEPATH += "/usr/local/include"
    LIBS += -L"/usr/local/lib" -lltdl
    LIBS += -L"../../build-PKILib-Desktop_Qt_5_11_3_clang_64bit-Debug" -lPKILib
    LIBS += -L"../../PKILib/lib/mac/debug/cmpossl/lib" -lcrypto -lssl
    LIBS += -lldap -llber
}

win32 {
    INCLUDEPATH += "../../PKILib/lib/win32/cmpossl-mingw32/include"
    LIBS += -L"../../PKILib/lib/win32/ltdl/lib" -lltdl
    LIBS += -L"../../build-PKILib-Desktop_Qt_5_12_2_MinGW_32_bit-Debug/debug" -lPKILib
    LIBS += -L"../../PKILib/lib/win32/cmpossl-mingw32/lib" -lcrypto -lssl
}

RESOURCES += \
    ccman.qrc

TRANSLATIONS += i18n/ccman_ko_KR.ts
