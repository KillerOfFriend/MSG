#ifndef CLIENTLOGMODEL_H
#define CLIENTLOGMODEL_H

#include <QAbstractListModel>

class TClientLogModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TClientLogModel(QObject *inParent = nullptr);
    ~TClientLogModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    std::list<QString> fMessages; // Список сообщений

public slots:
    void slot_NewMessage(QString inMessage); // Слод добавит сообщение в модель
};

#endif // CLIENTLOGMODEL_H
