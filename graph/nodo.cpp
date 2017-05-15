#include "nodo.hpp"

void Nodo::init( QString id,
                 QString title,
                 QString icon,
                 QStringList children,
                 QString text )
{
    this->setId( id );
    this->setTitle( title );
    this->setIcon( icon );
    this->setChildren( children );
    this->setText( text );
}

Nodo::Nodo( QString id,
            QString title,
            QString icon,
            QStringList children,
            QString text,
            QObject *parent ) :
    QObject( parent )
{
    this->init( id, title, icon, children, text );
}

Nodo::~Nodo()
{

}

QString Nodo::getId() const
{
    return id;
}

void Nodo::setId( const QString &value )
{
    id = value;
}

QString Nodo::getTitle() const
{
    return title;
}

void Nodo::setTitle( const QString &value )
{
    title = value;
}

QString Nodo::getIcon() const
{
    return icon;
}

void Nodo::setIcon(const QString &value )
{
    icon = value;
}

QStringList Nodo::getChildren() const
{
    return children;
}

void Nodo::setChildren( const QStringList &value )
{
    children = value;
}

QString Nodo::getText() const
{
    return text;
}

void Nodo::setText( const QString &value )
{
    text = value;
}
