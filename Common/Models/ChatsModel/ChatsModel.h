#ifndef CHATSMODEL_H
#define CHATSMODEL_H

#include <map>
#include <array>

#include <QUuid>
#include <QAbstractTableModel>

#include "Classes/ChatInfo/ChatInfo.h"

class TChatsModel : public QAbstractTableModel, public std::map<QUuid, Core::ChatInfo_Ptr>
{
    Q_OBJECT
public:
    enum eColumns { cChatUuid = 0, cChatName = 1, cChatPrivateStatus = 2 };

    TChatsModel(QObject* inParent = nullptr);
    TChatsModel(const TChatsModel &inOther);
    ~TChatsModel();

    TChatsModel& operator = (const TChatsModel &inOther);

    //-- Методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    //--
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    //-- Методы контейнера
    std::pair<std::map<QUuid, Core::ChatInfo_Ptr>::iterator, bool> insert(const std::pair<QUuid, Core::ChatInfo_Ptr> &inValue);
    //--
    std::map<QUuid, Core::ChatInfo_Ptr>::iterator erase(std::map<QUuid, Core::ChatInfo_Ptr>::iterator inIt);
    std::map<QUuid, Core::ChatInfo_Ptr>::size_type erase(const QUuid &inUuid);
    //--
    void clear();

private:
    static const quint8 fColumnCount = 3;
    std::array<QString, fColumnCount> fColumns;

    void initColumns();

signals:
    void sig_chatDeleting(const QUuid inDeletedChatUuid); // Сигнал об удалении беседы
};

#endif // CHATSMODEL_H
