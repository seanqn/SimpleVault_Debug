// TODO: Finish declarations

#pragma once
// include necessary crypto headers
#include <QObject>
#include <pwdbased.h>
#include <sha.h>

class CryptoServicer : public QObject {
    Q_OBJECT
public:
    CryptoServicer();
    ~CryptoServicer();

public slots:

signals:

private:

};
