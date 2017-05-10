#include <QApplication>
#include <QFile>

#include "interface/interface.hpp"
#include "common.hpp"
#include "processor/camera.hpp"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QString xml( DATA_PATH );
    xml.append( XML_PATH );
    xml.append( "ottaa.xml" );
    QFile file( xml );

    Interface *interface = new Interface();
    interface->showMaximized();
//    interface->show();
    interface->initInterface( &file );

    return app.exec();
}
