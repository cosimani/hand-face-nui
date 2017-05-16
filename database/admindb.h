#ifndef ADMINDB_H
#define ADMINDB_H
#include <QSqlDatabase>
#include <QString>
#include <QObject>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

class AdminDB : public QObject
{
    Q_OBJECT

    QSqlDatabase db;

public:
    AdminDB();
    ~AdminDB();

    bool conectar(QString archivo);
    void mostrarTabla(QString tabla);
    // nos devuelve el objeto db de esta clase, para poder realizar consultas desde otro lado
    QSqlDatabase getDB() { return db;}
    bool isConnected(){ return db.isOpen();}

    bool insertarEvento(QString evento);
    bool insertarFecha(QString f);

};

#endif // ADMINDB_H
