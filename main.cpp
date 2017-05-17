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
    interfaz->showMaximized();
//    interfaz->show();
//    interfaz->resize(QApplication::desktop()->screenGeometry().width(),
//                      QApplication::desktop()->screenGeometry().height()/2);
//    interfaz->move(0,0);

    interfaz->initInterface( &file );

    return app.exec();
}
