QT += widgets
QMAKE_CXXFLAGS+=-std=c++11
HEADERS       = mainwindow.h \
                mdichild.h \
    highlighter.h \
    common.h \
    lexer.h \
    tokenmap.h \
    parserLL1.h \
    parsetree.h \
    treeitem.h \
    treemodel.h \
    ParserRecur.h \
    semantics.h \
    sem.h \
    compile.h \
    snl2cpp.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                mdichild.cpp \
    highlighter.cpp \
    common.cpp \
    lexer.cpp \
    tokenmap.cpp \
    parsetree.cpp \
    parserLL1.cpp \
    treeitem.cpp \
    treemodel.cpp \
    ParserRecur.cpp \
    semantics.cpp \
    compile.cpp \
    snl2cpp.cpp
RESOURCES     = mdi.qrc

RC_FILE = compiler.rc
TRANSLATIONS=SNLCompiler_zh_cn.ts

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/mdi
#INSTALLS += target

