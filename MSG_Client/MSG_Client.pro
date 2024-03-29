#-------------------------------------------------
#
# Project created by QtCreator 2019-03-02T15:19:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( ../common.pri )
include( ../app.pri )

TARGET = MSG_Client
TEMPLATE = app

CONFIG += c++14
win32:DEFINES += __cpp_constexpr=201304 __cpp_variable_templates=201304

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
        ClientMainWindow.cpp \
    Classes/DataModule/DataModule.cpp \
    Classes/MSGClient/MSGClient.cpp \
    Classes/Settings/Settings.cpp \
    ../Common/Classes/UserInfo/UserInfo.cpp \
    Frames/MainFrame.cpp \
    Classes/FrameController/FrameController.cpp \
    Dialogs/UserViewDialog/UserViewDialog.cpp \
    Dialogs/ConnectDialog/ConnectDialog.cpp \
    ../Common/Classes/UserAccount/UserAccount.cpp \
    ../Common/Models/UsersModel/UsersModel.cpp \
    Classes/MSGClient/MSGClientExecutor.cpp \
    Delegates/UserItemDelegate/UserItemDelegate.cpp \
    ../Common/Models/UsersModel/UsersProxyModel.cpp \
    Classes/UserHeaderWidget/UserHeaderWidget.cpp \
    ../Common/Models/UsersTypeModel/UsersTypeModel.cpp \
    ../Common/othertypes.cpp \
    Classes/Models/Models.cpp \
    ../Common/Classes/ChatInfo/ChatInfo.cpp \
    ../Common/Models/ChatsModel/ChatsModel.cpp \
    Frames/ChatWidget/ChatWidget.cpp \
    Models/ClientLogModel/ClientLogModel.cpp \
    Dialogs/UserListDialog/UserListDialog.cpp \
    Delegates/ChatItemDelegate/ChatItemDelegate.cpp \
    ../Common/Models/ChatsModel/ChatsProxyModel.cpp \
    ../Common/Classes/ChatMessage/ChatMessage.cpp \
    ../Common/Classes/MessageHeadline/MessageHeadline.cpp \
    ../Common/Classes/Connection/Connection.cpp \
    ../Common/Classes/DataPacker/DataPacker.cpp \
    ../Common/Classes/CommandSystem/CommandSystem.cpp \
    ../Common/Classes/AbstractCommand/AbstractCommand.cpp \
    ../Common/Classes/AbstractExecutedCommand/AbstractExecutedCommand.cpp \
    ../Common/Classes/AbstractSendedCommand/AbstractSendedCommand.cpp

HEADERS += \
        ClientMainWindow.h \
    Classes/DataModule/DataModule.h \
    Classes/MSGClient/MSGClient.h \
    Classes/Settings/Settings.h \
    ../Common/comandes.h \
    ../Common/resultcodes.h \
    ../Common/Classes/UserInfo/UserInfo.h \
    Frames/MainFrame.h \
    Classes/FrameController/FrameController.h \
    Dialogs/UserViewDialog/UserViewDialog.h \
    Dialogs/ConnectDialog/ConnectDialog.h \
    ../Common/Classes/UserAccount/UserAccount.h \
    ../Common/Models/UsersModel/UsersModel.h \
    Delegates/UserItemDelegate/UserItemDelegate.h \
    ../Common/Models/UsersModel/UsersProxyModel.h \
    Classes/UserHeaderWidget/UserHeaderWidget.h \
    ../Common/Models/UsersTypeModel/UsersTypeModel.h \
    Classes/Models/Models.h \
    ../Common/Classes/ChatInfo/ChatInfo.h \
    ../Common/Models/ChatsModel/ChatsModel.h \
    Frames/ChatWidget/ChatWidget.h \
    Models/ClientLogModel/ClientLogModel.h \
    Dialogs/UserListDialog/UserListDialog.h \
    Delegates/ChatItemDelegate/ChatItemDelegate.h \
    ../Common/Models/ChatsModel/ChatsProxyModel.h \
    ../Common/Classes/ChatMessage/ChatMessage.h \
    ../Common/Classes/MessageHeadline/MessageHeadline.h \
    ../Common/Classes/Connection/Connection.h \
    ../Common/Classes/DataPacker/DataPacker.h \
    ../Common/Classes/CommandSystem/CommandSystem.h \
    ../Common/Classes/AbstractCommand/AbstractCommand.h \
    ../Common/Classes/AbstractExecutedCommand/AbstractExecutedCommand.h \
    ../Common/Classes/AbstractSendedCommand/AbstractSendedCommand.h

FORMS += \
        ClientMainWindow.ui \
    Frames/MainFrame.ui \
    Dialogs/UserViewDialog/UserViewDialog.ui \
    Dialogs/ConnectDialog/ConnectDialog.ui \
    Classes/UserHeaderWidget/UserHeaderWidget.ui \
    Frames/ChatWidget/ChatWidget.ui \
    Dialogs/UserListDialog/UserListDialog.ui

RESOURCES += \
    clientres.qrc
