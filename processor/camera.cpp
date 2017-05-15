#include "camera.hpp"
#include "pointmapper.hpp"
#include "featuresprocessor.hpp"
#include <QApplication>
#include <QDesktopWidget>

#include <QDebug>

Camera::Camera( QObject *parent ) : QObject( parent ),
                                    isCalibrated( false ),
                                    countFrameFaceless( 0 ),
                                    tipoDeteccionActual( HaarCascades ),
                                    isNeedCalibrated( false ),
                                    handDetector( new HandDetection )
{
    this->setVideoCapture( new VideoCapture( 0 ) );
    this->setSceneTimer( new QTimer( this ) );
    this->setCameraTexture( new Mat() );

    QString faceClassifier( APPLICATION_PATH );
    faceClassifier.append( CLASSIFIERS_PATH );
    faceClassifier.append( "face.xml" );
    string face = faceClassifier.toStdString();

    QString smileClassifier( APPLICATION_PATH );
    smileClassifier.append( CLASSIFIERS_PATH );
    smileClassifier.append( "smile.xml" );
    string smile = smileClassifier.toStdString();

    this->setFrontalFaceClassifier( new CascadeClassifier( face ) );
    this->setSmileClassifier( new CascadeClassifier( smile ) );
    this->setSize( 0 );

    connect( this->getSceneTimer(), SIGNAL( timeout() ), SLOT( process() ) );
    this->getSceneTimer()->start( 10 );
}

VideoCapture *Camera::getVideoCapture() const
{
    return videoCapture;
}

void Camera::setVideoCapture( VideoCapture *value )
{
    videoCapture = value;
}

QTimer *Camera::getSceneTimer() const
{
    return sceneTimer;
}

void Camera::setSceneTimer( QTimer *value )
{
    sceneTimer = value;
}

Mat *Camera::getCameraTexture() const
{
    return cameraTexture;
}

void Camera::setCameraTexture( Mat *value )
{
    cameraTexture = value;
}

CascadeClassifier *Camera::getFrontalFaceClassifier() const
{
    return frontalFaceClassifier;
}

void Camera::setFrontalFaceClassifier( CascadeClassifier *value )
{
    frontalFaceClassifier = value;
}

CascadeClassifier *Camera::getSmileClassifier() const
{
    return smileClassifier;
}

void Camera::setSmileClassifier( CascadeClassifier *value )
{
    smileClassifier = value;
}

int Camera::getSize() const
{
    return size;
}

void Camera::setSize( int value )
{
    size = value;
}

bool Camera::getSmiling() const
{
    return smiling;
}

void Camera::setSmiling( bool value )
{
    smiling = value;
}

/**
 * @brief Camera::setTipoDeteccion Este metodo activa alguno metodo particular de deteccion y deshabilita los otros.
 * @param tipo
 */
void Camera::setTipoDeteccion(Camera::TipoDeteccion tipo)
{
    this->tipoDeteccionActual = tipo;
}

Camera::TipoDeteccion Camera::getTipoDeteccionActual() const
{
    return tipoDeteccionActual;
}

bool Camera::getIsNeedCalibrated() const
{
    return isNeedCalibrated;
}

void Camera::setIsNeedCalibrated(bool value)
{
    isNeedCalibrated = value;
}

void Camera::process()
{
    this->getVideoCapture()->operator >>( *this->getCameraTexture() );

    // Esta signal envia el nuevo Mat a la interfaz para que la pueda visualizar
    emit signal_newCameraFrame( this->getCameraTexture() );

    if ( tipoDeteccionActual == HaarCascades )  {

        vector< Rect > detectedFaces;
        detectedFaces.clear();

        frontalFaceClassifier->detectMultiScale( *this->getCameraTexture(), detectedFaces,
                                         1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

        if( detectedFaces.size() > 0 )
        {
            currentFace = detectedFaces.at( 0 );
            countFrameFaceless = 0;
        }
        else  {
            this->isCalibrated = false;  // Si no detecta cara ya no se tiene control de la interfaz
            emit positionDetected( -1 );  // Aca desiluminamos el ultimo boton iluminado

            countFrameFaceless++;

            if ( countFrameFaceless == FRAMES_FACELESS_BACK )  {
                emit signal_nonFaceDetectedForBack();
            }
            else if ( countFrameFaceless > FRAMES_FACELESS )  {
                emit signal_nonFaceDetected();
                countFrameFaceless = 0;
            }
        }

        if( detectedFaces.size() > 0 && size != 0 )
        {
            int xFaceCenter = detectedFaces.at( 0 ).x + detectedFaces.at( 0 ).width / 2;
            int yFaceCenter = detectedFaces.at( 0 ).y + detectedFaces.at( 0 ).height / 2;

            // Entra a este if cuando aun no esta calibrado. Directamente retorna. Pero en esta version le agregamos la
            // posibilidad de calibrar con la sonrisa. Entonces, dentro de aqui detectamos sonrisas.
            if( ( xFaceCenter < calibration.x ||
                  xFaceCenter > calibration.x + calibration.width ||
                  yFaceCenter < calibration.y ||
                  yFaceCenter > calibration.y + calibration.height )
                  || this->isCalibrated == false )
            {
                vector< Rect > detectedCalibrationSmiles;
                detectedCalibrationSmiles.clear();

                detectedFaces.operator []( 0 ).width -= detectedFaces.operator []( 0 ).width % 3;

                Mat face( *this->getCameraTexture(), detectedFaces.at( 0 ) );

                smileClassifier->detectMultiScale( face, detectedCalibrationSmiles,
                                                  1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 50, 50 ) );

                // Si detecta sonrisa, entonces calibramos
                if( detectedCalibrationSmiles.size() > 0 )
                {
                    // El getSmiling setSmiling y smiling es para evitar que la sonrisa sea detectada en frames consecutivos
                    if( ! this->getSmiling() )
                    {
                        this->calibrate();
                        this->setSmiling( true );
                    }
                }
                else
                {
                    if( smiling )
                    {
                        this->setSmiling( false );
                    }
                }
            }

            // Si no esta calibrado, entonces no se puede controlar el menu.
            if ( ! this->isCalibrated )  {
                return;
            }

            int index = this->getSize() - ( ( xFaceCenter - calibration.x ) / ( calibration.width / size ) ) - 1;

            emit positionDetected( index );

            vector< Rect > detectedSmiles;
            detectedSmiles.clear();

            detectedFaces.operator []( 0 ).width -= detectedFaces.operator []( 0 ).width % 3;

            Mat face( *this->getCameraTexture(), detectedFaces.at( 0 ) );

            smileClassifier->detectMultiScale( face, detectedSmiles,
                                              1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 50, 50 ) );

            if( detectedSmiles.size() > 0 )
            {
                if( ! this->getSmiling() )
                {
                    emit selectionDetected( index );
                    this->setSmiling( true );
                }
            }
            else
            {
                if( smiling )
                {
                    this->setSmiling( false );
                }
            }
        }
    }

    if ( tipoDeteccionActual == OnlySmile )  {

        vector< Rect > detectedFaces;
        detectedFaces.clear();

        frontalFaceClassifier->detectMultiScale( *this->getCameraTexture(), detectedFaces,
                                         1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

        if( detectedFaces.size() > 0 )
        {
            vector< Rect > detectedSmiles;
            detectedSmiles.clear();

            Mat face( *this->getCameraTexture(), detectedFaces.at( 0 ) );

            smileClassifier->detectMultiScale( face, detectedSmiles,
                                              1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 50, 50 ) );

            // Este if y else es para que no detecte una sonrisa en frames consecutivos.
            // Algo asi como un antirebote.
            if( detectedSmiles.size() > 0 )
            {
                if( ! this->getSmiling() )
                {
                    this->setSmiling( true );
                    emit signal_sonrisa();
                }
            }
            else
            {
                if( smiling )
                {
                    this->setSmiling( false );
                }
            }

        }
    }

    if ( tipoDeteccionActual == Features )  {

        vector< Rect > faces;
        faces.clear();

        frontalFaceClassifier->detectMultiScale( *this->getCameraTexture(), faces,
                                                 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

        Rect roi( 0, 0, 0, 0 );

        if( faces.size() > 0 )
        {
            roi = faces.at( 0 );

            // actually noise tip region

            roi.x = roi.x + roi.width / ( double )3;
            roi.y = roi.y + roi.height / ( double )2;
            roi.width = roi.width * 1 / ( double )3;
            roi.height = roi.height * 1 / ( double )3;
        }

        // Esta es la region de la nariz
        QRect rectRoi( roi.x, roi.y, roi.width, roi.height );


        if( this->isNeedCalibrated )
        {
            // Este containerRect es 0,0 640x480
            PointMapper::getInstance()->setContainerRect( QRect( 0, 0,
                      this->getVideoCapture()->get( CV_CAP_PROP_FRAME_WIDTH ),
                      this->getVideoCapture()->get( CV_CAP_PROP_FRAME_HEIGHT ) ) );

            // Este es el rectangulo de la nariz
            PointMapper::getInstance()->setOriginRect( rectRoi );

            // Este es el rectangulo de la pantalla completa. Por ejemplo QRect(0,0 1366x768)
            PointMapper::getInstance()->setTargetRect( QApplication::desktop()->screenGeometry() );
//                PointMapper::getInstance()->setTargetRect( QRect(0,0, 100, 1) );

            FeaturesProcessor::getInstance()->setNeedToInitFT( true );
            FeaturesProcessor::getInstance()->setInitialFrames( true );
            FeaturesProcessor::getInstance()->setInitialFramesCounter( 0 );
            FeaturesProcessor::getInstance()->setFirstCentroid( true );
            FeaturesProcessor::getInstance()->setDrawProcessing( false );  // Para que no imprima mensajes OSD

            this->isNeedCalibrated = false;
        }

        QPoint target = FeaturesProcessor::getInstance()->process( this->getCameraTexture(), rectRoi );

        bool deseaControlarMouse = false;

        if( deseaControlarMouse )
        {
            QCursor::setPos( PointMapper::getInstance()->map( target ) );
        }

        emit signal_cursorTracking( target );
    }

    if ( tipoDeteccionActual == Features_and_hand )  {

//        handDetector->fistDetection( *this->getCameraTexture(), refSkin->minimumHue, refSkin->maximumHue);



        vector< Rect > faces;
        faces.clear();

        frontalFaceClassifier->detectMultiScale( *this->getCameraTexture(), faces,
                                                 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

        Rect roi( 0, 0, 0, 0 );

        if( faces.size() > 0 )
        {
            roi = faces.at( 0 );

            // actually noise tip region

            roi.x = roi.x + roi.width / ( double )3;
            roi.y = roi.y + roi.height / ( double )2;
            roi.width = roi.width * 1 / ( double )3;
            roi.height = roi.height * 1 / ( double )3;
        }

        // Esta es la region de la nariz
        QRect rectRoi( roi.x, roi.y, roi.width, roi.height );


        if( this->isNeedCalibrated )
        {
            // Este containerRect es 0,0 640x480
            PointMapper::getInstance()->setContainerRect( QRect( 0, 0,
                      this->getVideoCapture()->get( CV_CAP_PROP_FRAME_WIDTH ),
                      this->getVideoCapture()->get( CV_CAP_PROP_FRAME_HEIGHT ) ) );

            // Este es el rectangulo de la nariz
            PointMapper::getInstance()->setOriginRect( rectRoi );

            // Este es el rectangulo de la pantalla completa. Por ejemplo QRect(0,0 1366x768)
            PointMapper::getInstance()->setTargetRect( QApplication::desktop()->screenGeometry() );
//                PointMapper::getInstance()->setTargetRect( QRect(0,0, 100, 1) );

            FeaturesProcessor::getInstance()->setNeedToInitFT( true );
            FeaturesProcessor::getInstance()->setInitialFrames( true );
            FeaturesProcessor::getInstance()->setInitialFramesCounter( 0 );
            FeaturesProcessor::getInstance()->setFirstCentroid( true );
            FeaturesProcessor::getInstance()->setDrawProcessing( false );  // Para que no imprima mensajes OSD

            this->isNeedCalibrated = false;
        }

        QPoint target = FeaturesProcessor::getInstance()->process( this->getCameraTexture(), rectRoi );

        bool deseaControlarMouse = false;

        if( deseaControlarMouse )
        {
            QCursor::setPos( PointMapper::getInstance()->map( target ) );
        }

        emit signal_cursorTracking( target );
    }
}

void Camera::calibrate()
{
    calibration.x = currentFace.x + currentFace.width / 4;
    calibration.y = currentFace.y + currentFace.height / 4;
    calibration.width = currentFace.width / 2;
    calibration.height = currentFace.height / 2;

    this->isCalibrated = true;
}
