// TODO: Finish declarations

#ifndef CRYPTOSERVICER_H
#define CRYPTOSERVICER_H
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

#endif // CRYPTOSERVICER_H
