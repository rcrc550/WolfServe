TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lhiredis
SOURCES += \
    include/wolfplayroom.cpp \
    src/Mysql.cpp \
    src/TCPKernel.cpp \
    src/Thread_pool.cpp \
    src/block_epoll_net.cpp \
    src/err_str.cpp \
    src/libevent_net.cpp \
    src/main.cpp \
    src/redistool.cpp

HEADERS += \
    include/Mysql.h \
    include/TCPKernel.h \
    include/Thread_pool.h \
    include/block_epoll_net.h \
    include/err_str.h \
    include/libevent_net.h \
    include/packdef.h \
    include/redistool.h \
    include/wolfplayroom.h
INCLUDEPATH += ./include \
        /usr/local/libevent/include
LIBS += -lmysqlclient -lpthread -L/usr/local/libevent/lib -levent
