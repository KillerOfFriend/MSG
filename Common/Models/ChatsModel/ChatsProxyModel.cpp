#include "ChatsProxyModel.h"

//-----------------------------------------------------------------------------
TChatsProxyModel::TChatsProxyModel(QObject* inParent) : QSortFilterProxyModel(inParent)
{

}
//-----------------------------------------------------------------------------
TChatsProxyModel::~TChatsProxyModel()
{
    fFilter.clear();
}
//-----------------------------------------------------------------------------
void TChatsProxyModel::setSourceModel(QAbstractItemModel* inSourceModel) // Метод задаст модель - источник
{
    if (inSourceModel)
    {
        QSortFilterProxyModel::setSourceModel(inSourceModel);
        connect(inSourceModel, &QAbstractItemModel::dataChanged, this, &TChatsProxyModel::invalidate);
    }
}
//-----------------------------------------------------------------------------
void TChatsProxyModel::setFilter(const TChatsModel::eColumns inColumn, const QString inFilter) // Задаём фильтр
{
    fColumn = inColumn; // Фильтруемы столбец
    fFilter = inFilter.toLower(); // Сам фильтр
    invalidate(); // Запускаем фильтр
}
//-----------------------------------------------------------------------------
bool TChatsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const // Метод фильтрации
{
    Q_UNUSED(source_parent);

    bool Result = false;

    if (fFilter.isEmpty())
        Result = true;
    else
        if (sourceModel()) // Если инициализированна модель - источник
        {
            QVariant Data = sourceModel()->data(createIndex(source_row, fColumn));
            Result = Data.toString().toLower().contains(fFilter);
        }
        else Result = true;

    return Result;
}
//-----------------------------------------------------------------------------
