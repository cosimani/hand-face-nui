#include "xmlreader.hpp"

XMLReader::XMLReader( QObject *parent ) :
    QObject( parent )
{

}

XMLReader::~XMLReader()
{

}

QVector< Node* > *XMLReader::parseXML( QFile *file )
{
    if( !file->open( QFile::ReadOnly | QFile::Text ) )
    {
        // No se pudo abrir el archivo

        return NULL;
    }

    QVector< Node* > *nodes = new QVector< Node* >();

    QXmlStreamReader xmlReader;
    xmlReader.setDevice( file );

    if( xmlReader.readNextStartElement() &&
        xmlReader.name() == XML_INITIAL_ELEMENT )
    {
        while( xmlReader.readNextStartElement() )
        {
            if( xmlReader.name() == XML_INSTANCE_ELEMENT )
            {
                QString id( "" );
                QString title( "" );
                QString icon( "" );
                QString children( "" );
                QString text( "" );

                while( xmlReader.readNextStartElement() )
                {
                    if( xmlReader.name() == XML_ID_ELEMENT_NAME )
                        id = xmlReader.readElementText();          else

                    if( xmlReader.name() == XML_TITLE_ELEMENT_NAME )
                        title = xmlReader.readElementText();       else

                    if( xmlReader.name() == XML_ICON_ELEMENT_NAME )
                        icon = xmlReader.readElementText();        else

                    if( xmlReader.name() == XML_CHILDREN_ELEMENT_NAME )
                        children = xmlReader.readElementText();    else

                    if( xmlReader.name() == XML_TEXT_ELEMENT_NAME )
                        text = xmlReader.readElementText();        else

                    xmlReader.skipCurrentElement();
                }

                if( id       != "" &&
                    title    != "" &&
                    icon     != "" &&
                    children != "" )
                {
                    nodes->append( new Node( id, title, icon, children.remove( " " ).split( "," ), text ) );
                }
            }
        }
    }
    else
    {
        // No se encontro el elemento inicial del xml

        delete nodes;
        nodes = NULL;
    }

    file->close();

    return nodes;
}
