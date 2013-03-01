TEMPLATE = app
QT = gui \
    core \
    network
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/dlgIPChecker.ui \
    ui/dlgLogger.ui \
    ui/dlgSettings.ui
HEADERS = src/IPChecker.h \
    src/Logger.h \
    src/Settings.h
SOURCES = src/IPChecker.cpp \
    src/main.cpp \
    src/Logger.cpp \
    src/Settings.cpp
RESOURCES += res/IPChecker.qrc
RC_FILE = src/main.rc
