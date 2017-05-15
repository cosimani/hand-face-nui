#ifndef SKINFILTER_H
#define SKINFILTER_H

#include <QObject>

// clase para detectar el color de la piel
class SkinFilter : public QObject
{
    Q_OBJECT

public:

    int minimumHue;
    int maximumHue;
    int minimumSat;
    int maximumSat;
    int minimumVal;
    int maximumVal;

    SkinFilter( QObject *parent = 0 ) : QObject( parent ),
                                  minimumHue( -1 ),
                                  maximumHue( -1 ),
                                  minimumSat( -1 ),
                                  maximumSat( -1 ),
                                  minimumVal( -1 ),
                                  maximumVal( -1 )
    {
    }

    void addMinHue( int hue )  {
        minimumHue = hue;
    }
    void addMaxHue( int hue )  {
        maximumHue = hue;
    }

    void addGoodValue( int hue, int sat, int val )
    {
        if( minimumHue < 0 || maximumHue < 0 )
        {
            minimumHue = hue; maximumHue = hue;
        }

        if( minimumSat < 0 || maximumSat < 0 )
        {
            minimumSat = sat; maximumSat = sat;
        }

        if( minimumVal < 0 || maximumVal < 0 )
        {
            minimumVal = val; maximumVal = val;
        }

        if( hue < minimumHue ) minimumHue = hue;
        if( hue > maximumHue ) maximumHue = hue;

        if( sat < minimumSat ) minimumSat = sat;
        if( sat > maximumSat ) maximumSat = sat;

        if( val < minimumVal ) minimumVal = val;
        if( val > maximumVal ) maximumVal = val;
    }
};

#endif // SKINFILTER_H
