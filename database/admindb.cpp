#include "admindb.h"

AdminDB::AdminDB(QObject *parent) : QObject( parent )
{
    db = QSqlDatabase::addDatabase("QSQLITE");
}

AdminDB::~AdminDB()
{
    if (db.isOpen())
        db.close();
}

bool AdminDB::conectar(QString archivo)
{
    db.setDatabaseName(archivo);
    return db.open();      // NO confundir con el metodo isopen()
}

void AdminDB::mostrarTabla(QString tabla)  {
    if (this->isConnected())  {
        QSqlQuery query = db.exec("SELECT * FROM " + tabla);

        // creo que siempre retorna -1 porque tal vez la base de datos no soporta esto (segun qt)
        //  if (query.size() == 0 || query.size() == -1)
        //    qDebug() << "La consulta no trajo registros";

        while(query.next())  {
            QSqlRecord registro = query.record();  // Devuelve un objeto que maneja un registro (linea, row)
            int campos = registro.count();  // Devuleve la cantidad de campos de este registro

            QString informacion;  // En este QString se va armando la cadena para mostrar cada registro
            for (int i=0 ; i<campos ; i++)  {
                informacion += registro.fieldName(i) + ":";  // Devuelve el nombre del campo
                informacion += registro.value(i).toString() + " - ";
            }
            qDebug() << informacion;
        }
    }
    else
        qDebug() << "No se encuentra conectado a la base";
}


bool AdminDB::insertarEvento(QString evento)
{
    QSqlQuery query(db);

    return query.exec("INSERT INTO logs (evento) VALUES('"+ evento +"')");
}


bool AdminDB::insertarFecha(QString fecha)
{
    QSqlQuery query(db);


    return query.exec("INSERT INTO logs (fecha) VALUES('"+ fecha +"')");
}
























