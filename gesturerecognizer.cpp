#include "gesturerecognizer.hpp"

GestureRecognizer *GestureRecognizer::instance = NULL;

GestureRecognizer::GestureRecognizer( QObject *parent ) :
    QObject( parent )
{
    this->setPositionVectors( new QVector< QPointF >() );
    this->setVelocityVectors( new QVector< QPointF >() );
}

QVector< QPointF > *GestureRecognizer::getPositionVectors() const
{
    return positionVectors;
}

void GestureRecognizer::setPositionVectors( QVector< QPointF > *value )
{
    positionVectors = value;
}

QVector< QPointF > *GestureRecognizer::getVelocityVectors() const
{
    return velocityVectors;
}

void GestureRecognizer::setVelocityVectors( QVector< QPointF > *value )
{
    velocityVectors = value;
}

void GestureRecognizer::addPosition( QPointF position )
{
    this->getPositionVectors()->append( position );

    if( this->getPositionVectors()->size() > MAX_POSITIONS )
    {
        this->getPositionVectors()->removeFirst();
    }
}

void GestureRecognizer::addVelocity( QPointF velocity )
{
    this->getVelocityVectors()->append( velocity );

    if( this->getVelocityVectors()->size() > MAX_VELOCITIES )
    {
        this->getVelocityVectors()->removeFirst();
    }
}

void GestureRecognizer::recognize()
{
    /// recognition
}

GestureRecognizer *GestureRecognizer::getInstance()
{
    if( instance == NULL )
    {
        instance = new GestureRecognizer( NULL );
    }
    return instance;
}

GestureRecognizer::~GestureRecognizer()
{
    delete this->getInstance();
}
