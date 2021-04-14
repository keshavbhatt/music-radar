#ifndef PLUSEUTILS_H
#define PLUSEUTILS_H

#include <QObject>
#include <QProcess>

class PulseUtils : public QObject
{
    Q_OBJECT
public slots:
    QString getDefaultSink();
    QString getDefaultSource();
    QStringList getSources();
    QStringList getSinks();
public:
    PulseUtils(QObject* parent=0);
    virtual ~PulseUtils();
private slots:

};

#endif // PLUSEUTILS_H
