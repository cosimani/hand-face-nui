#include "graph.hpp"

Graph::Graph( QObject *parent ) :
    QObject( parent ),
    nodes( new QVector< Nodo* >() )
{

}

Graph::~Graph()
{
    if( this->getNodes() )
    {
        for( int i = 0; i < this->getNodes()->size(); i++ )
        {
            delete this->getNodes()->at( i );
        }
    }
    delete this->getNodes();
}

QVector< Nodo* > *Graph::getNodes() const
{
    return nodes;
}

void Graph::setNodes( QVector< Nodo* > *value )
{
    nodes = value;
}

bool Graph::initGraph( QFile *file )
{
    XMLReader xmlReader;

    this->setNodes( xmlReader.parseXML( file ) );

    if( !this->getNodes() || this->getNodes()->size() == 0 )
    {
        // No se pudo parsear el XML o no hay nodos para leer

        return false;
    }

    return true;
}

Nodo *Graph::get( QString id )
{
    for( int i = 0; i < this->getNodes()->size(); i++ )
    {
        if( this->getNodes()->at( i )->getId() == id )
        {
            return this->getNodes()->at( i );
        }
    }

    return NULL;
}
