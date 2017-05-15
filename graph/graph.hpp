#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QObject>
#include <QVector>
#include <QFile>

#include "nodo.hpp"
#include "xmlreader.hpp"

class Graph : public QObject
{
    Q_OBJECT

private:

    QVector< Nodo* > *nodes;

public:

    explicit Graph( QObject *parent = NULL );
    ~Graph();

    QVector< Nodo* > *getNodes() const;
    void setNodes( QVector< Nodo* > *value);

    bool initGraph( QFile *file );
    Nodo *get( QString id );
};

#endif // GRAPH_HPP

