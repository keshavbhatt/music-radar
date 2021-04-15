#ifndef ITEMINFOWIDGET_H
#define ITEMINFOWIDGET_H

#include <QWidget>

class JsonModel;

namespace Ui {
class ItemInfoWidget;
}

class ItemInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemInfoWidget(QWidget *parent = nullptr);
    ~ItemInfoWidget();

public slots:
    void load(const QString jsonPath);
protected slots:
    void closeEvent(QCloseEvent *e);
private slots:
    void saveSettings() const;
    void loadSettings();
private:
    Ui::ItemInfoWidget *ui;
    JsonModel *model_;
};

#endif // ITEMINFOWIDGET_H
