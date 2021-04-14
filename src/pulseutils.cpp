#include "pulseutils.h"
#include <QDebug>

PulseUtils::PulseUtils(QObject *parent) : QObject(parent)
{

}

QStringList PulseUtils::getSources()
{
    return QStringList();
}

QString PulseUtils::getDefaultSource()
{
    QProcess pactl;
    QProcess grep;
    QProcess sed;
    pactl.setStandardOutputProcess(&grep);
    grep.setStandardOutputProcess(&sed);

    pactl.start("pactl",QStringList()<<"info");
    grep.start("grep",QStringList()<<"-i"<<"Default Source:");
    sed.start("sed",QStringList()<<"s/^Default Source:[ ]*//i");

    pactl.waitForFinished();

    grep.waitForFinished();

    sed.waitForFinished();

    return sed.readAll().trimmed();
}

QStringList PulseUtils::getSinks()
{

    return QStringList();
}

QString PulseUtils::getDefaultSink()
{
    QProcess pactl;
    QProcess grep;
    QProcess sed;
    pactl.setStandardOutputProcess(&grep);
    grep.setStandardOutputProcess(&sed);

    pactl.start("pactl",QStringList()<<"info");
    grep.start("grep",QStringList()<<"-i"<<"Default Sink:");
    sed.start("sed",QStringList()<<"s/^Default sink:[ ]*//i");

    pactl.waitForFinished();

    grep.waitForFinished();

    sed.waitForFinished();

    return sed.readAll().trimmed()+".monitor";
}

PulseUtils::~PulseUtils()
{

}
