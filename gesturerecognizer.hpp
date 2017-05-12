#ifndef GESTURERECOGNIZER_HPP
#define GESTURERECOGNIZER_HPP

#define MAX_POSITIONS  500
#define MAX_VELOCITIES 500

// qt includes
#include <QObject>
#include <QVector>
#include <QPointF>

class GestureRecognizer : public QObject
{
    Q_OBJECT

private:

    explicit GestureRecognizer( QObject *parent = NULL );
    static GestureRecognizer *instance;

    QVector< QPointF > *positionVectors;
    QVector< QPointF > *velocityVectors;

public:

    static GestureRecognizer *getInstance();
    ~GestureRecognizer();

    QVector< QPointF > *getPositionVectors() const;
    void setPositionVectors( QVector< QPointF > *value );

    QVector< QPointF > *getVelocityVectors() const;
    void setVelocityVectors( QVector< QPointF > *value );

    void addPosition( QPointF position );
    void addVelocity( QPointF velocity );
    void recognize();
};

#endif // GESTURERECOGNIZER_HPP
