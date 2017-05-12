#include "featuresprocessor.hpp"
#include <QDebug>

FeaturesProcessor *FeaturesProcessor::instance = NULL;

FeaturesProcessor::FeaturesProcessor( QObject *parent ) :
    QObject( parent )
{
    this->setDrawProcessing( true );
    this->setGrayImage( new Mat() );
    this->setPreviousGrayImage( new Mat() );
    this->setTermCriteria( TermCriteria( TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03 ) );
    this->setSubWinPixSize( Size( 10, 10 ) );
    this->setWinSize( Size( 31, 31 ) );
    this->setMaxCount( 5000 );
    this->setNeedToInitFT( true );
    this->setInitialFrames( true );
    this->setInitialFramesCounter( 0 );
    this->setOffset( Point( 0, 0 ) );
    this->setFirstCentroid( true );
}

FeaturesProcessor *FeaturesProcessor::getInstance()
{
    if( instance == NULL )
    {
        instance = new FeaturesProcessor( NULL );
    }
    return instance;
}

FeaturesProcessor::~FeaturesProcessor()
{
    delete this->getPreviousGrayImage();
    delete this->getGrayImage();
    delete this->getInstance();
}

bool FeaturesProcessor::getDrawProcessing() const
{
    return drawProcessing;
}

void FeaturesProcessor::setDrawProcessing( bool value )
{
    drawProcessing = value;
}

Mat *FeaturesProcessor::getGrayImage() const
{
    return grayImage;
}

void FeaturesProcessor::setGrayImage(Mat *value)
{
    grayImage = value;
}

Mat *FeaturesProcessor::getPreviousGrayImage() const
{
    return previousGrayImage;
}

void FeaturesProcessor::setPreviousGrayImage( Mat *value )
{
    previousGrayImage = value;
}

bool FeaturesProcessor::getNeedToInitFT() const
{
    return needToInitFT;
}

void FeaturesProcessor::setNeedToInitFT(bool value)
{
    needToInitFT = value;
}

int FeaturesProcessor::getMaxCount() const
{
    return maxCount;
}

void FeaturesProcessor::setMaxCount(int value)
{
    maxCount = value;
}

Size FeaturesProcessor::getSubWinPixSize() const
{
    return subWinPixSize;
}

void FeaturesProcessor::setSubWinPixSize( const Size &value )
{
    subWinPixSize = value;
}

Size FeaturesProcessor::getWinSize() const
{
    return winSize;
}

void FeaturesProcessor::setWinSize(const Size &value)
{
    winSize = value;
}

TermCriteria FeaturesProcessor::getTermCriteria() const
{
    return termCriteria;
}

void FeaturesProcessor::setTermCriteria( const TermCriteria &value )
{
    termCriteria = value;
}

bool FeaturesProcessor::getInitialFrames() const
{
    return initialFrames;
}

void FeaturesProcessor::setInitialFrames(bool value)
{
    initialFrames = value;
}

int FeaturesProcessor::getInitialFramesCounter() const
{
    return initialFramesCounter;
}

void FeaturesProcessor::setInitialFramesCounter(int value)
{
    initialFramesCounter = value;
}

bool FeaturesProcessor::getFirstCentroid() const
{
    return firstCentroid;
}

void FeaturesProcessor::setFirstCentroid(bool value)
{
    firstCentroid = value;
}

Point FeaturesProcessor::getOffset() const
{
    return offset;
}

void FeaturesProcessor::setOffset(const Point &value)
{
    offset = value;
}

QPoint FeaturesProcessor::process( Mat *mat, QRect roi )
{
    /// messages

    Point messagePos( mat->cols * 7 / ( double )8, 0 );
    int messageFont = FONT_HERSHEY_SIMPLEX;
    int messageLineThickness = 1;
    double messageFontScale = 0.5;

    /// processing

    QPoint target( 0, 0 );

    if( mat->empty() )
    {
        return target;
    }

    /// features processing

    cvtColor( *mat, *this->getGrayImage(), COLOR_BGR2GRAY );

    /// modificar mat para mensajes SOLO despues de esta linea, para no afectar el procesamiento

    if( this->getDrawProcessing() )
    {
        int messagesRectOffset = 30;
        Rect messagesRect( messagePos.x - messagesRectOffset, 0, mat->cols * 1 / ( double )8 + messagesRectOffset, mat->rows );

        Mat messagesRoi = (*mat)( messagesRect );
        Mat color( messagesRoi.size(), CV_8UC3, cv::Scalar( 0, 0, 0 ) );
        double alpha = 0.3;
        addWeighted( color, alpha, messagesRoi, 1.0 - alpha , 0.0, messagesRoi );
    }

    if( this->getNeedToInitFT() )
    {
        goodFeaturesToTrack( *this->getGrayImage(),
                             points[1],
                             this->getMaxCount(),
                             0.01, 10, Mat(), 3, 0, 0.04 );

        cornerSubPix( *this->getGrayImage(),
                      points[1],
                      this->getSubWinPixSize(),
                      Size( -1, -1 ),
                      this->getTermCriteria() );
    }
    else
    {
        if( !points[0].empty() )
        {
            vector< uchar > status;
            vector< float > error;

            if( this->getPreviousGrayImage()->empty() )
            {
                this->getGrayImage()->copyTo( *this->getPreviousGrayImage() );
            }

            calcOpticalFlowPyrLK( *this->getPreviousGrayImage(),
                                  *this->getGrayImage(),
                                  points[0],
                                  points[1],
                                  status, error,
                                  this->getWinSize(),
                                  3, this->getTermCriteria(), 0, 0.001 );
            size_t i, k;
            for( i = k = 0; i < points[1].size(); i++ )
            {
                if( !status[i] )
                {
                    continue;
                }

                points[1][k++] = points[1][i];

                if( this->getDrawProcessing() )
                {
                    circle( *mat, points[1][i], 1, Scalar( 255, 255, 255 ), 1 );
                }
            }
            points[1].resize( k );

            if( this->getDrawProcessing() )
            {
                messagePos.y += 25;

                putText( *mat, "features", messagePos, messageFont, messageFontScale, Scalar( 255, 255, 255 ), messageLineThickness );
                circle( *mat, Point( messagePos.x - 13,  messagePos.y - 8 ), 1, Scalar( 255, 255, 255 ), 1 );
                circle( *mat, Point( messagePos.x - 18, messagePos.y ), 1, Scalar( 255, 255, 255 ), 1 );
                circle( *mat, Point( messagePos.x - 8,  messagePos.y ), 1, Scalar( 255, 255, 255 ), 1 );
            }
        }
    }

    this->setNeedToInitFT( false );

    std::swap( points[1], points[0] );

    // Parece que este if se ejecuta los 20 primeros frames.
    if( this->getInitialFrames() )
    {
        /// points filter according roi

        this->setInitialFramesCounter( this->getInitialFramesCounter() + 1 );

        for( unsigned int i = 0; i < points[0].size(); i++ )
        {
            if( points[0][i].x < roi.x() ||
                points[0][i].x > roi.x() + roi.width() ||
                points[0][i].y < roi.y() ||
                points[0][i].y > roi.y() + roi.height() )
            {
                points[0].erase( points[0].begin() + i );
            }
        }
        if( this->getInitialFramesCounter() == 20 )
        {
            this->setInitialFrames( false );

            GestureRecognizer::getInstance()->getPositionVectors()->clear();
            GestureRecognizer::getInstance()->getVelocityVectors()->clear();
        }
    }
    else
    {
        /// essential points processing

        Point centroid( 0, 0 );

        int pointsCounter = 0;
        for( unsigned int i = 0; i < points[0].size(); i++ )
        {
            centroid.x += points[0][i].x;
            centroid.y += points[0][i].y;
            pointsCounter++;
        }
        if( pointsCounter != 0 )
        {
            centroid.x /= ( double )pointsCounter;
            centroid.y /= ( double )pointsCounter;
        }

        if( this->getDrawProcessing() )
        {
            messagePos.y += 25;

            putText( *mat, "centroid", messagePos, messageFont, messageFontScale, Scalar( 255, 0, 0 ), messageLineThickness );
            circle( *mat, centroid, 2, Scalar( 255, 0, 0 ), 2 );
            circle( *mat, Point( messagePos.x - 8, messagePos.y - 3 ), 2, Scalar( 255, 0, 0 ), 2 );
        }

        if( this->getFirstCentroid() )
        {
            this->setOffset( Point( roi.center().x() - centroid.x,
                                    roi.center().y() - centroid.y ) );
        }
        this->setFirstCentroid( false );

        Point final( centroid.x + this->getOffset().x,
                     centroid.y + this->getOffset().y );

        if( this->getDrawProcessing() )
        {
            messagePos.y += 25;

            putText( *mat, "target", messagePos, messageFont,  messageFontScale, Scalar( 255, 0, 255 ), messageLineThickness );
            circle( *mat, final, 2, Scalar( 255, 0, 255 ), 2 );
            circle( *mat, Point( messagePos.x - 8, messagePos.y - 3 ), 2, Scalar( 255, 0, 255 ), 2 );
        }

        if( final.x != 0 && final.y != 0 )
        {
            target.setX( final.x );
            target.setY( final.y );
        }
    }

    if( target.x() != 0 && target.y() != 0 )
    {
        GestureRecognizer::getInstance()->addPosition( QPointF( target.x() - 320, target.y() - 240 ) );
    }

    if( this->getDrawProcessing() )
    {
        Point roiCenter( roi.center().x(),
                         roi.center().y() );

        messagePos.y += 25;

        putText( *mat, "roiCenter", messagePos, messageFont, messageFontScale, Scalar( 0, 0, 255 ), messageLineThickness );
        circle( *mat, roiCenter, 2, Scalar( 0, 0, 255 ), 2 );
        circle( *mat, Point( messagePos.x - 8, messagePos.y - 3 ), 2, Scalar( 0, 0, 255 ), 2 );
    }

    cv::swap( *this->getPreviousGrayImage(), *this->getGrayImage() );

    if( this->getDrawProcessing() )
    {
        Rect roiRect( roi.x(), roi.y(), roi.width(), roi.height() );
        rectangle( *mat, roiRect, Scalar( 0, 0, 255 ) );
    }

    /// velocity processing for gestures

    QPointF velocityVector( 0, 0 );
    int velocityPointsCounter = 0;

    for( unsigned int i = 0; i < points[1].size(); i++ )
    {
        if( i < points[0].size() )
        {
            velocityVector.setX( velocityVector.x() + ( points[0][i].x - points[1][i].x ) );
            velocityVector.setY( velocityVector.y() + ( points[0][i].y - points[1][i].y ) );
            velocityPointsCounter++;
        }
    }
    if( velocityPointsCounter!= 0 )
    {
        velocityVector.setX( velocityVector.x() / ( double )velocityPointsCounter );
        velocityVector.setY( velocityVector.y() / ( double )velocityPointsCounter );
    }

    GestureRecognizer::getInstance()->addVelocity( velocityVector );
    GestureRecognizer::getInstance()->recognize();

    if( this->getDrawProcessing() )
    {
        //line( *mat, Point( target.x() - velocityVector.x() * 10, target.y() - velocityVector.y() * 10 ),
        //            Point( target.x(), target.y() ), Scalar( 255, 0, 0 ), 2 );
    }

    return target;
}
