#ifndef XMLREADER_HPP
#define XMLREADER_HPP

#define XML_INITIAL_ELEMENT         "nodes"
#define XML_INSTANCE_ELEMENT        "node"

#define XML_ID_ELEMENT_NAME         "id"
#define XML_TITLE_ELEMENT_NAME      "title"
#define XML_ICON_ELEMENT_NAME       "icon"
#define XML_CHILDREN_ELEMENT_NAME   "children"
#define XML_TEXT_ELEMENT_NAME       "text"

#include <QObject>
#include <QFile>
#include <QXmlStreamReader>
#include <QVector>

#include "node.hpp"

class XMLReader : public QObject
{
    Q_OBJECT

private:

public:

    explicit XMLReader( QObject *parent = NULL );
    ~XMLReader();

    QVector< Node* > *parseXML( QFile *file );
};

#endif // XMLREADER_HPP

