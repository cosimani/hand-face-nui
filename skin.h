#ifndef SKIN_H
#define SKIN_H

#include <QObject>

class Skin : public QObject
{
    Q_OBJECT

public:

    int minimumHue;
    int maximumHue;
    int minimumSat;
    int maximumSat;
    int minimumVal;
    int maximumVal;

    Skin( QObject *parent = 0 ) : QObject( parent ),
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


#endif // SKIN_H
