HEADERS = treeitem.h \
    treemodel.h \
    mainwindow.h \
    ui_mainwindow.h \
    netmodel.h \
    operationdelegate.h \
    eventwidget.h \
    positioning.h \
    graphwidget.h \
    arrowwidget.h \
    ui_dialog.h \
    dialog.h \
    fullpathesmodel.h \
    calceventmodel.h \
    calcoperationmodel.h \
    calcmodel.h
RESOURCES = 
SOURCES = treeitem.cpp \
    treemodel.cpp \
    main.cpp \
    mainwindow.cpp \
    netmodel.cpp \
    operationdelegate.cpp \
    positioning.cpp \
    graphwidget.cpp \
    arrowwidget.cpp \
    dialog.cpp \
    fullpathesmodel.cpp \
    eventwidget.cpp \
    calceventmodel.cpp \
    calcoperationmodel.cpp \
    calcmodel.cpp
CONFIG += qt

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simpletreemodel
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    *.pro \
    *.txt
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simpletreemodel
INSTALLS += target \
    sources
FORMS += mainwindow.ui \
    dialog.ui
