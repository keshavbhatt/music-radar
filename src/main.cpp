#include "mainwindow.h"
#include <QApplication>

#include "rungaurd.h"
#include <QDebug>

int main(int argc, char *argv[]) {

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
  if (!qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO) &&
      !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR") &&
      !qEnvironmentVariableIsSet("QT_SCALE_FACTOR") &&
      !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  }

  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/icons/app/icon-256.png"));

  QApplication::setApplicationName("Music Radar");
  QApplication::setOrganizationName("org.keshavnrj.ubuntu");
  QApplication::setApplicationVersion(VERSIONSTR);

// allow multiple instances in debug builds
#ifndef QT_DEBUG
  QString appname = QApplication::applicationName();
  RunGuard guard("org.keshavnrj.ubuntu." + appname);
  if (!guard.tryToRun()) {
    QMessageBox::critical(0, appname,
                          "An instance of " + appname + " is already running.");
    return 0;
  }
#endif

  MainWindow w;
  w.show();

  return a.exec();
}
