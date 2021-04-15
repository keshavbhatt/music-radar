#include "stable.h"
#include "jsonitem.h"


class JsonItemData : public QSharedData
{
public:
    QString name_;
    QJsonValue value_;
    JsonItem *parent_;
    QList<JsonItem *> children_;
};


JsonItem::JsonItem()
    : data(new JsonItemData)
{
    data->parent_ = Q_NULLPTR;
}


JsonItem::JsonItem(const QString &name, const QJsonValue &value, JsonItem *parent)
    : data(new JsonItemData)
{
    data->name_ = name;
    data->value_ = value;
    data->parent_ = parent;

    if (value.isArray())
    {
        auto array = value.toArray();

        for (int i = 0; i < array.size(); i++)
        {
            data->children_.append(new JsonItem(QS("[%1]").arg(i), array.at(i), this));
        }
    }
    else if (value.isObject())
    {
        auto object = value.toObject();

        for (const auto &key : object.keys())
        {
            data->children_.append(new JsonItem(key, object.value(key), this));
        }
    }
}


JsonItem::JsonItem(const JsonItem &rhs)
    : data(rhs.data)
{
}


JsonItem &JsonItem::operator=(const JsonItem &rhs)
{
    if (this != &rhs)
    {
        data.operator=(rhs.data);
    }

    return *this;
}


JsonItem::~JsonItem()
{
    qDeleteAll(data->children_);
}


QString JsonItem::name() const
{
    return data->name_;
}


QJsonValue::Type JsonItem::type() const
{
    return data->value_.type();
}


QVariant JsonItem::value() const
{
    QVariant var;

    if (data->value_.isArray())
    {
        var = tr("<%1 items>").arg(data->children_.size());
    }
    else if (!data->value_.isObject())
    {
        var = data->value_.toVariant();
    }

    return var;
}


JsonItem *JsonItem::parent() const
{
    return data->parent_;
}


int JsonItem::row() const
{
    if (data->parent_ != Q_NULLPTR)
    {
        return data->parent_->data->children_.indexOf(const_cast<JsonItem *>(this));
    }

    return 0;
}


int JsonItem::childCount() const
{
    return data->children_.size();
}


JsonItem *JsonItem::childAt(int index) const
{
    return data->children_.at(index);
}
