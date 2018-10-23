#ifndef COMMONCORE_H
#define COMMONCORE_H

#include <QObject>

class CommonCore : public QObject
{
    Q_OBJECT
public:
    explicit CommonCore(QObject *parent = 0);
    static DWORD currentDwProcessId;
signals:

public slots:
};

#endif // COMMONCORE_H
