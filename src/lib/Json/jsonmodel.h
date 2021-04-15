#pragma once
#include <QAbstractItemModel>
#include <QJsonDocument>

class JsonItem;


class JsonModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Column
    {
        NameColumn,
        TypeColumn,
        ValueColumn,
        ColumnCount
    };
    Q_ENUMS(Column)

    explicit JsonModel(QObject *parent = Q_NULLPTR);
    virtual ~JsonModel();

    // Header:
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Basic functionality:
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Editable:
    virtual bool setData(const QModelIndex &index, const QVariant &value,
                         int role = Qt::EditRole) Q_DECL_OVERRIDE;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    const QJsonDocument &document() const
    {
        return doc_;
    }

public slots:
    void setJson(const QByteArray &json);

private:
    QJsonDocument doc_;
    JsonItem *root_;
};
