#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QObject>
#include <QVector>
#include <QFile>

#include "node.hpp"
#include "xmlreader.hpp"

class Graph : public QObject
{
    Q_OBJECT

private:

    QVector< Node* > *nodes;

public:

    explicit Graph( QObject *parent = NULL );
    ~Graph();

    QVector< Node* > *getNodes() const;
    void setNodes( QVector< Node* > *value);

    bool initGraph( QFile *file );
    Node *get( QString id );
};

#endif // GRAPH_HPP

