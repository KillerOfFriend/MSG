#-------------------------------------------------
#
# Project created by QtCreator 2019-03-02T10:02:43
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( ../common.pri ) # Общие настройки
include( ../app.pri ) # Настройки сборки

TARGET = MSG_Server
TEMPLATE = app

CONFIG += c++14
#win32: DEFINES += __cpp_constexpr=201304 __cpp_variable_templates=201304

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../Common

SOURCES += \
        main.cpp \
        ServerMainWindow.cpp \
    Classes/DataModule/DataModule.cpp \
    Classes/MSGServer/MSGServer.cpp \
    ../Common/Classes/UserInfo/UserInfo.cpp \
    Models/ConnectedUsers/ConnectedUsers.cpp \
    Models/ServerLogModel/ServerLogModel.cpp \
    Classes/DB/DB.cpp \
    Classes/Settings/Settings.cpp \
    ../Common/Classes/UserAccount/UserAccount.cpp \
    ../Common/Models/UsersModel/UsersModel.cpp \
    Classes/MSGServer/MSGServerExecutor.cpp \
    ../Common/Models/UsersTypeModel/UsersTypeModel.cpp \
    Models/UsersTypeModelDB/UsersTypeModelDB.cpp \
    ../Common/othertypes.cpp \
    ../Common/Classes/ChatInfo/ChatInfo.cpp \
    ../Common/Models/ChatsModel/ChatsModel.cpp \
    Classes/ServerCache/ServerCacheDB.cpp \
    Classes/ServerCache/ServerCache.cpp \
    ../Common/Classes/ChatMessage/ChatMessage.cpp \
    ../Common/Classes/MessageHeadline/MessageHeadline.cpp \
    ../Common/Classes/Connection/Connection.cpp \
    ../Common/Classes/DataPacker/DataPacker.cpp \
    ../Common/Classes/AbstractCommand/AbstractCommand.cpp \
    ../Common/Classes/AbstractSendedCommand/AbstractSendedCommand.cpp \
    ../Common/Classes/CommandSystem/CommandSystem.cpp

HEADERS += \
        ServerMainWindow.h \
    Classes/DataModule/DataModule.h \
    Classes/MSGServer/MSGServer.h \
    ../Common/comandes.h \
    ../Common/resultcodes.h \
    ../Common/Classes/UserInfo/UserInfo.h \
    Models/ConnectedUsers/ConnectedUsers.h \
    Models/ServerLogModel/ServerLogModel.h \
    ../Common/othertypes.h \
    Classes/DB/DB.h \
    Classes/Settings/Settings.h \
    ../Common/Classes/UserAccount/UserAccount.h \
    ../Common/Models/UsersModel/UsersModel.h \
    ../Common/Models/UsersTypeModel/UsersTypeModel.h \
    Models/UsersTypeModelDB/UsersTypeModelDB.h \
    ../Common/Classes/ChatInfo/ChatInfo.h \
    ../Common/Models/ChatsModel/ChatsModel.h \
    Classes/ServerCache/ServerCache.h \
    ../Common/Classes/ChatMessage/ChatMessage.h \
    ../Common/Classes/MessageHeadline/MessageHeadline.h \
    ../Common/Classes/Connection/Connection.h \
    ../Common/Classes/DataPacker/DataPacker.h \
    ../Common/Classes/AbstractCommand/AbstractCommand.h \
    ../Common/Classes/AbstractSendedCommand/AbstractSendedCommand.h \
    ../Common/Classes/CommandSystem/CommandSystem.h

FORMS += \
        ServerMainWindow.ui
