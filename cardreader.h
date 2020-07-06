#ifndef CARDREADER_H
#define CARDREADER_H

#include <QObject>
#include <QProcess>

class CardReader : public QObject
{
    Q_OBJECT
public:
    explicit CardReader();
    void run();

signals:
    void newCardAvailableOnDb();

public slots:
    void pyProcessFinished(int exitCode);

private:
    QProcess pyProcess;
    QString pyScript;
};

#endif // CARDREADER_H
