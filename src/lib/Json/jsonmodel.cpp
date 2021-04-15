#include "stable.h"
#include "jsonmodel.h"
#include "jsonitem.h"


JsonModel::JsonModel(QObject *parent)
    : QAbstractItemModel(parent)
    , root_(new JsonItem)
{
}


JsonModel::~JsonModel()
{
    delete root_;
}


QVariant JsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var;

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case NameColumn:
            var = tr("Name");
            break;
        case TypeColumn:
            var = tr("Type");
            break;
        case ValueColumn:
            var = tr("Value");
            break;
        default:
            break;
        }
    }

    return var;
}


QModelIndex JsonModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex mi;

    if (hasIndex(row, column, parent))
    {
        const JsonItem *parentItem = root_;

        if (parent.isValid())
        {
            parentItem = static_cast<const JsonItem *>(parent.internalPointer());
        }

        JsonItem *childNode = parentItem->childAt(row);

        if (childNode != Q_NULLPTR)
        {
            mi = createIndex(row, column, childNode);
        }
    }

    return mi;
}


QModelIndex JsonModel::parent(const QModelIndex &index) const
{
    QModelIndex mi;

    if (index.isValid())
    {
        const JsonItem *childNode = static_cast<const JsonItem *>(index.internalPointer());
        JsonItem *parentNode = childNode->parent();

        if (parentNode != root_)
        {
            mi = createIndex(parentNode->row(), 0, parentNode);
        }
    }

    return mi;
}


int JsonModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    const JsonItem *item = root_;

    if (parent.isValid())
    {
        item = static_cast<const JsonItem *>(parent.internalPointer());
    }

    return item->childCount();
}


int JsonModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}


QVariant JsonModel::data(const QModelIndex &index, int role) const
{
    QVariant var;

    if (index.isValid() && (role == Qt::DisplayRole))
    {
        const JsonItem *item = static_cast<const JsonItem *>(index.internalPointer());

        switch (index.column())
        {
        case NameColumn:
            var = item->name();
            break;
        case TypeColumn:
            switch (item->type())
            {
            case QJsonValue::Null:
                var = tr("Null");
                break;
            case QJsonValue::Bool:
                var = tr("Bool");
                break;
            case QJsonValue::Double:
                var = tr("Double");
                break;
            case QJsonValue::String:
                var = tr("String");
                break;
            case QJsonValue::Array:
                var = tr("Array");
                break;
            case QJsonValue::Object:
                var = tr("Object");
                break;
            case QJsonValue::Undefined:
                var = tr("Undefined");
                break;
            default:
                break;
            }

            break;

        case ValueColumn:
            var = item->value();
            break;
        }
    }

    return var;
}


bool JsonModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value)
    {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }

    return false;
}


Qt::ItemFlags JsonModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    //return Qt::ItemIsEditable; // FIXME: Implement me!
    return QAbstractItemModel::flags(index);
}


void JsonModel::setJson(const QByteArray &json)
{
    beginResetModel();

    doc_ = QJsonDocument::fromJson(json);
    delete root_;

    if (doc_.isArray())
    {
        root_ = new JsonItem(QString(), doc_.array());
    }
    else if (doc_.isObject())
    {
        root_ = new JsonItem(QString(), doc_.object());
    }
    else
    {
        root_ = new JsonItem;
    }

    endResetModel();
}
