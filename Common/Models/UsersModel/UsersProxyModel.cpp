#include "UsersProxyModel.h"

//-----------------------------------------------------------------------------
TUsersProxyModel::TUsersProxyModel(QObject* inParent) : QSortFilterProxyModel(inParent)
{

}
//-----------------------------------------------------------------------------
TUsersProxyModel::~TUsersProxyModel()
{
    fFilter.clear();
}
//-----------------------------------------------------------------------------
void TUsersProxyModel::setSourceModel(QAbstractItemModel* inSourceModel) // Метод задаст модель - источник
{
    if (inSourceModel)
    {
        QSortFilterProxyModel::setSourceModel(inSourceModel);
        connect(inSourceModel, &QAbstractItemModel::dataChanged, this, &TUsersProxyModel::invalidate);
    }
}
//-----------------------------------------------------------------------------
void TUsersProxyModel::setFilter(const TUsersModel::eColumns inColumn, const QString inFilter) // Задаём фильтр
{
    fColumn = inColumn; // Фильтруемы столбец
    fFilter = inFilter.toLower(); // Сам фильтр
    invalidate(); // Запускаем фильтр
}
//-----------------------------------------------------------------------------
bool TUsersProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const // Метод фильтрации
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
