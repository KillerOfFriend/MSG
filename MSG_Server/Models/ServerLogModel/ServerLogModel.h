#ifndef SERVERLOGMODEL_H
#define SERVERLOGMODEL_H

#include <list>

#include <QAbstractTableModel>
#include <QHostAddress>
#include <QDateTime>
#include <QString>

class TServerLogModel : public QAbstractTableModel
{
public:
    enum eColumns { cTime = 0, cAddres = 1, cMessage = 2 };

    static const eColumns firstColumnIndex = cTime;
    static const eColumns lastColumnIndex = cMessage;

    TServerLogModel(QObject* inParent = nullptr);
    ~TServerLogModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    struct TRecord
    {
        QDateTime Time;
        QHostAddress ClientAddres;
        QString Message;
    };

    const quint16 fMaxLines = 300;
    static const quint8 fColumnCount = 3;

    std::list<TRecord> fRecords;
    std::array<QString, fColumnCount> fColumns;

    void initColumns();

public slots:
    void slot_NewMessage(QHostAddress inAddres, QString inMessage);
};

#endif // SERVERLOGMODEL_H
