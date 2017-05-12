#ifndef POINTMAPPER_HPP
#define POINTMAPPER_HPP

// qt includes
#include <QObject>
#include <QRect>
#include <QPoint>

class PointMapper : public QObject
{
    Q_OBJECT

private:

    explicit PointMapper( QObject *parent = NULL );
    static PointMapper *instance;

    QRect containerRect;
    QRect originRect;
    QRect targetRect;

public:

    static PointMapper *getInstance();
    ~PointMapper();

    QRect getContainerRect() const;
    void setContainerRect( const QRect &value );

    QRect getOriginRect() const;
    void setOriginRect( const QRect &value );

    QRect getTargetRect() const;
    void setTargetRect( const QRect &value );

    QPoint map( QPoint point );

signals:

    void positionDetected( int x, int y );
};

#endif // POINTMAPPER_HPP
