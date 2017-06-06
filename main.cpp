#include <QApplication>
#include <QFile>
#include <QDesktopWidget>

#include "interface/interface.hpp"
#include "common.hpp"
#include "processor/camera.hpp"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QString xml( DATA_PATH );
    xml.append( XML_PATH );
    xml.append( "nui.xml" );
    QFile file( xml );

    Interface *interfaz = new Interface();
//    interfaz->showMaximized();
    interfaz->show();
//    interfaz->resize(QApplication::desktop()->screenGeometry().width(),
//                      QApplication::desktop()->screenGeometry().height()/2);
//    interfaz->move(0,0);

    interfaz->initInterface( &file );


    QRect rec = QApplication::desktop()->screenGeometry();
    int h = rec.height();
    int w = rec.width();
    interfaz->resize(w,h/2);
    interfaz->move(0,0);

//    int a = 1, b = 4, c = a | b, d = 2;

//    if ( c & a )
//        qDebug() << "if ( c & a ) ";

//    if ( c & b )
//        qDebug() << "if ( c & b ) ";

//    if ( c & d )
//        qDebug() << "if ( c & d ) ";



//    qDebug() << "a & b" << (a & b);
//    qDebug() << "a | b" << (a | b);
//    qDebug() << "a && b" << (a && b);
//    qDebug() << "a || b" << (a || b);



    return app.exec();
    return 0;
}
