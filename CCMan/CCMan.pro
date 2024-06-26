QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

PROJECT_VERSION = "0.9.1"
DEFINES += CCMAN_VERSION=$$PROJECT_VERSION

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# DEFINES += _AUTO_UPDATE

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about_dlg.cpp \
    account_info.cpp \
    admin_dlg.cpp \
    auto_update_service.cpp \
    cc_client.cpp \
    cert_info_dlg.cpp \
    common.cpp \
    config_dlg.cpp \
    crl_info_dlg.cpp \
    i18n_helper.cpp \
    issue_cert_dlg.cpp \
    issue_crl_dlg.cpp \
    lcn_info_dlg.cpp \
    login_dlg.cpp \
    main.cpp \
    mainwindow.cpp \
    make_cert_profile_dlg.cpp \
    make_crl_profile_dlg.cpp \
    make_lcn_dlg.cpp \
    man_applet.cpp \
    man_right_widget.cpp \
    man_tree_item.cpp \
    man_tree_model.cpp \
    man_tree_view.cpp \
    revoke_cert_dlg.cpp \
    search_menu.cpp \
    search_form.cpp \
    settings_dlg.cpp \
    settings_mgr.cpp \
    signer_dlg.cpp \
    stat_form.cpp \
    tst_info_dlg.cpp \
    user_dlg.cpp

HEADERS += \
    about_dlg.h \
    account_info.h \
    admin_dlg.h \
    auto_update_service.h \
    cc_client.h \
    cert_info_dlg.h \
    common.h \
    config_dlg.h \
    crl_info_dlg.h \
    i18n_helper.h \
    issue_cert_dlg.h \
    issue_crl_dlg.h \
    lcn_info_dlg.h \
    login_dlg.h \
    mainwindow.h \
    make_cert_profile_dlg.h \
    make_crl_profile_dlg.h \
    make_lcn_dlg.h \
    man_applet.h \
    man_right_widget.h \
    man_tree_item.h \
    man_tree_model.h \
    man_tree_view.h \
    revoke_cert_dlg.h \
    search_menu.h \
    search_form.h \
    settings_dlg.h \
    settings_mgr.h \
    signer_dlg.h \
    singleton.h \
    stat_form.h \
    tst_info_dlg.h \
    user_dlg.h

FORMS += \
    about_dlg.ui \
    admin_dlg.ui \
    cert_info_dlg.ui \
    config_dlg.ui \
    crl_info_dlg.ui \
    issue_cert_dlg.ui \
    issue_crl_dlg.ui \
    lcn_info_dlg.ui \
    login_dlg.ui \
    mainwindow.ui \
    make_cert_profile_dlg.ui \
    make_crl_profile_dlg.ui \
    make_lcn_dlg.ui \
    revoke_cert_dlg.ui \
    settings_dlg.ui \
    signer_dlg.ui \
    stat_form.ui \
    tst_info_dlg.ui \
    search_form.ui \
    user_dlg.ui

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
    INCLUDEPATH += "/usr/local/include"

    CONFIG( debug, debug | release ) {
        message( "CertMan Debug" );
        LIBS += -L"../../build-PKILib-Desktop_Qt_5_15_2_clang_64bit-Debug" -lPKILib
        LIBS += -L"../../lib/mac/debug/openssl3/lib" -lcrypto -lssl
        INCLUDEPATH += "../../lib/mac/debug/openssl3/include"
    } else {
        message( "CertMan Release" );
        LIBS += -L"../../build-PKILib-Desktop_Qt_5_15_2_clang_64bit-Release" -lPKILib
        LIBS += -L"../../lib/mac/openssl3/lib" -lcrypto -lssl
        INCLUDEPATH += "../../lib/mac/openssl3/include"
    }

    LIBS += -L"/usr/local/lib" -lltdl

    LIBS += -lldap -llber
}

win32 {
    DEFINES += _AUTO_UPDATE
    RC_ICONS = ccman.ico

    contains(QT_ARCH, i386) {
        message( "cc_man 32bit" )
        INCLUDEPATH += "../../lib/win32/winsparkle/include"
        INCLUDEPATH += "C:\msys64\mingw32\include"

        Debug {
            LIBS += -L"../../build-PKILib-Desktop_Qt_5_13_2_MinGW_32_bit-Debug" -lPKILib
            LIBS += -L"../../lib/win32/debug/openssl3/lib" -lcrypto -lssl
        } else {
            LIBS += -L"../../build-PKILib-Desktop_Qt_5_13_2_MinGW_32_bit-Release" -lPKILib
            LIBS += -L"../../lib/win32/openssl3/lib" -lcrypto -lssl
        }

        LIBS += -L"../../lib/win32" -lltdl -lldap -llber
        LIBS += -L"../../lib/win32/winsparkle/lib" -lWinSparkle -lws2_32
    } else {
        message( "cc_man 64bit" )
        INCLUDEPATH += "../../lib/win64/winsparkle/include"
        INCLUDEPATH += "C:\msys64\mingw64\include"

        Debug {
            LIBS += -L"../../build-PKILib-Desktop_Qt_5_13_2_MinGW_64_bit-Debug" -lPKILib
            LIBS += -L"../../lib/win64/debug/openssl3/lib64" -lcrypto -lssl
        } else {
            LIBS += -L"../../build-PKILib-Desktop_Qt_5_13_2_MinGW_64_bit-Release" -lPKILib
            LIBS += -L"../../lib/win64/openssl3/lib64" -lcrypto -lssl
        }

        LIBS += -L"../../lib/win64" -lltdl -lldap -llber
        LIBS += -L"../../lib/win64/winsparkle/lib" -lWinSparkle -lws2_32
    }
}

RESOURCES += \
    ccman.qrc

TRANSLATIONS += i18n/ccman_ko_KR.ts
