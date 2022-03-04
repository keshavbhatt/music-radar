#include "iteminfowidget.h"
#include "ui_iteminfowidget.h"

#include "lib/Json/jsonhighlighter.h"
#include "lib/Json/jsonmodel.h"
#include "lib/Json/stable.h"

#include <QMessageBox>
#include <QSettings>

ItemInfoWidget::ItemInfoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ItemInfoWidget), model_(nullptr) {
  ui->setupUi(this);

  model_ = new JsonModel(this);
  ui->treeView->setModel(model_);

  new JsonHighlighter(ui->editView->document());

  loadSettings();
}

void ItemInfoWidget::load(const QString jsonPath) {
  QFile file(jsonPath);

  if (file.open(QFile::ReadOnly | QFile::Text)) {
    model_->setJson(file.readAll());
    ui->editView->setPlainText(model_->document().toJson());

    ui->treeView->expandToDepth(2);
  } else {
    QMessageBox::critical(this, tr("Failed to load data."), file.errorString());
  }
}

void ItemInfoWidget::closeEvent(QCloseEvent *e) {
  saveSettings();
  QWidget::closeEvent(e);
}

void ItemInfoWidget::loadSettings() {
  QSettings settings;
  restoreGeometry(settings.value(QS("geo")).toByteArray());
  ui->treeView->header()->restoreState(
      settings.value(QS("headerState")).toByteArray());
}

void ItemInfoWidget::saveSettings() const {
  QSettings settings;
  settings.setValue(QS("geo"), saveGeometry());
  settings.setValue(QS("headerState"), ui->treeView->header()->saveState());
}

ItemInfoWidget::~ItemInfoWidget() { delete ui; }
