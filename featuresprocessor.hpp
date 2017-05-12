#ifndef FEATURESPROCESSOR_HPP
#define FEATURESPROCESSOR_HPP

// qt includes
#include <QObject>
#include <QTimer>
#include <QPoint>
#include <QPointF>
#include <QRect>

// opencv includes
#include <opencv2/opencv.hpp>

// project includes
#include "gesturerecognizer.hpp"

using namespace cv;
using namespace std;

class FeaturesProcessor : public QObject
{
    Q_OBJECT

private:

    explicit FeaturesProcessor( QObject *parent = NULL );
    static FeaturesProcessor *instance;

    bool drawProcessing;
    Mat *grayImage;
    Mat *previousGrayImage;

    bool needToInitFT;
    vector< Point2f > points[2];
    int maxCount;
    Size subWinPixSize;
    Size winSize;
    TermCriteria termCriteria;

    bool initialFrames;
    int initialFramesCounter;
    bool firstCentroid;
    Point offset;

public:

    static FeaturesProcessor *getInstance();
    ~FeaturesProcessor();

    bool getDrawProcessing() const;
    void setDrawProcessing( bool value );

    Mat *getGrayImage() const;
    void setGrayImage( Mat *value );

    Mat *getPreviousGrayImage() const;
    void setPreviousGrayImage( Mat *value );

    bool getNeedToInitFT() const;
    void setNeedToInitFT( bool value );

    int getMaxCount() const;
    void setMaxCount( int value );

    Size getSubWinPixSize() const;
    void setSubWinPixSize( const Size &value );

    Size getWinSize() const;
    void setWinSize( const Size &value );

    TermCriteria getTermCriteria() const;
    void setTermCriteria( const TermCriteria &value );

    bool getInitialFrames() const;
    void setInitialFrames( bool value );

    int getInitialFramesCounter() const;
    void setInitialFramesCounter( int value );

    bool getFirstCentroid() const;
    void setFirstCentroid( bool value );

    Point getOffset() const;
    void setOffset( const Point &value );

    QPoint process(Mat *mat, QRect roi );
};

#endif // FEATURESPROCESSOR_HPP
