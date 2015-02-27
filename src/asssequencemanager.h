#ifndef ASSSEQUENCEMANAGER_H
#define ASSSEQUENCEMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>

class AssSequenceManager : public QObject
{
    Q_OBJECT
public:
    explicit AssSequenceManager(QObject *parent = 0);
    ~AssSequenceManager();

    QMap<QString, QStringList> assSequence;
    int seqLength;

signals:

public slots:
};

#endif // ASSSEQUENCEMANAGER_H
