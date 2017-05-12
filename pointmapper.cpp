#include "pointmapper.hpp"

PointMapper *PointMapper::instance = NULL;

PointMapper::PointMapper( QObject *parent ) :
    QObject( parent )
{

}

PointMapper *PointMapper::getInstance()
{
    if( instance == NULL )
    {
        instance = new PointMapper( NULL );
    }
    return instance;
}

PointMapper::~PointMapper()
{
    delete this->getInstance();
}

QRect PointMapper::getContainerRect() const
{
    return containerRect;
}

void PointMapper::setContainerRect( const QRect &value )
{
    containerRect = value;
}

QRect PointMapper::getOriginRect() const
{
    return originRect;
}

void PointMapper::setOriginRect( const QRect &value )
{
    originRect = value;
}

QRect PointMapper::getTargetRect() const
{
    return targetRect;
}

void PointMapper::setTargetRect( const QRect &value )
{
    targetRect = value;
}

QPoint PointMapper::map( QPoint point )
{
    point.setX( point.x() - this->getOriginRect().x() );
    point.setY( point.y() - this->getOriginRect().y() );

    int x = point.x() * this->getTargetRect().width() / ( double )this->getOriginRect().width();
    int y = point.y() * this->getTargetRect().height() / ( double )this->getOriginRect().height();

    return QPoint( this->getTargetRect().width() - x, y );
}

