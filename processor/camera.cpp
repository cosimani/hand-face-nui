#include "camera.hpp"
#include "pointmapper.hpp"
#include "featuresprocessor.hpp"
#include <QApplication>
#include <QDesktopWidget>

#include <QDebug>

Camera::Camera( QObject *parent ) : QObject( parent ),
                                    isCalibrated( false ),
                                    countFrameFaceless( 0 ),
                                    isNeedCalibrated( false ),
                                    muestraProcesada( false )
{
    this->setVideoCapture( new VideoCapture( 0 ) );
//    this->setVideoCapture( new VideoCapture( "../videos/video10.mkv" ) );

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

    //create Background Subtractor objects
    mog2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    mog2.dynamicCast<cv::BackgroundSubtractorMOG2>()->setNMixtures(3);
    backgroundFrame=50;
    sbUpValue = 134;
    sbDownValue = 122;
    isBackgroundCapturado = false;

    this->setTipoDeteccion( HaarCascades );


    procesarUnaSolaImagen = false;
    yaSeTomoOriginal = false;

    refSkin = new SkinFilter( this );
    handDetector = new HandDetection(this);

}

Camera::~Camera()
{
    videoCapture->release();
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
    switch( tipo )  {
    case HaarCascades: qInfo() << "Tipo Deteccion = HaarCascades";
        break;
    case Features: qInfo() << "Tipo Deteccion = Features";
        break;
    case OnlySmile: qInfo() << "Tipo Deteccion = OnlySmile";
        break;
    case BackgroundCapture: qInfo() << "Tipo Deteccion = BackgroundCapture";
        break;
    case Features_and_hand: qInfo() << "Tipo Deteccion = Features_and_hand";
        break;
    case Hand: qInfo() << "Tipo Deteccion = Hand";
        break;
    case Hand_and_Smile: qInfo() << "Tipo Deteccion = Hand_and_Smile = Hand | OnlySmile";
        break;
    default: qInfo() << "Tipo Deteccion = default";
    }


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

void Camera::setMuestraProcesada(bool value)
{
    muestraProcesada = value;
}

bool Camera::getMuestraProcesada() const
{
    return muestraProcesada;
}

void Camera::capturarFondo()
{
    this->setTipoDeteccion( BackgroundCapture );
}

void Camera::setSbUpValue(int value)
{
    sbUpValue = value;
}

void Camera::setSbDownValue(int value)
{
    sbDownValue = value;
}

int Camera::getSbUpValue() const
{
    return sbUpValue;
}

int Camera::getSbDownValue() const
{
    return sbDownValue;
}

void Camera::process()
{
    if ( ! videoCapture->isOpened() )
        return;

    if ( ! procesarUnaSolaImagen )  {
        this->getVideoCapture()->operator >>( *this->getCameraTexture() );
    }
    else  {

        if ( ! yaSeTomoOriginal )
            originalframeParaProcesar = (*this->getCameraTexture()).clone();

        yaSeTomoOriginal = true;
        (*this->getCameraTexture()) = originalframeParaProcesar.clone();
    }


    if ( ! muestraProcesada )
        // Esta signal envia el nuevo Mat a la interfaz para que la pueda visualizar
        emit signal_newCameraFrame( this->getCameraTexture() );

    if ( tipoDeteccionActual & HaarCascades )  {
//        qWarning() << "HaarCascades";

        this->processMenuPrincipal( *(this->getCameraTexture()) );

    }

    if ( tipoDeteccionActual & BackgroundCapture )  {

//        qWarning() << "BackgroundCapture";

        Mat frame = (*this->getCameraTexture()).clone();

        //Update the current background model and get the foreground
        if ( backgroundFrame>0 )  {
            mog2.dynamicCast<cv::BackgroundSubtractorMOG2>()->apply(frame, fore);
            backgroundFrame--;

            int resto = backgroundFrame % 10;

            std::string str;

            if (resto==0)       str = "Capturing background";
            else if (resto==1)  str = "Capturing background.";
            else if (resto==2)  str = "Capturing background...";
            else if (resto==3)  str = "Capturing background....";
            else if (resto==4)  str = "Capturing background.....";
            else if (resto==5)  str = "Capturing background......";
            else if (resto==6)  str = "Capturing background.....";
            else if (resto==7)  str = "Capturing background....";
            else if (resto==8)  str = "Capturing background...";
            else if (resto==9)  str = "Capturing background..";
            else                str = "Capturing background.";

            putText((*this->getCameraTexture()), str, cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(20,200,250), 1, CV_AA);

            this->isBackgroundCapturado = true;

        }
        else  {  // backgroundFrame es igual a 0

            // Lo ponemos como HaarCascades porque el boton para captura background esta solo ahi
            this->setTipoDeteccion( HaarCascades );
        }

    }

    if ( tipoDeteccionActual & OnlySmile )  {
//        qWarning() << "OnlySmile";

        this->processSmile( *(this->getCameraTexture()) );

    }

    if ( tipoDeteccionActual & Features )  {

//        qWarning() << "Features";

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
            roi.width = roi.width / ( double )3;
            roi.height = roi.height / ( double )3;
        }
        else  {
            qDebug() << "no detectar cara";
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

    if ( tipoDeteccionActual & Hand )  {

//        qWarning() << "Hand";

        // Lo siguiente es para la deteccion de la manos, sin no esta isBackgroundCapturado no lo hace
        if ( isBackgroundCapturado )  {

            this->borrarRostro( *(this->getCameraTexture()), true );

            this->sustraerFondo( *(this->getCameraTexture()), true );

            bool existePuno = handDetector->fistDetection( *this->getCameraTexture(), sbDownValue, sbUpValue);

//            qDebug() << existePuno;

//            this->processFingers( *this->getCameraTexture() );

            if ( muestraProcesada )  emit signal_newCameraFrame( this->getCameraTexture() );


        }
    }



//    if ( tipoDeteccionActual & Features_and_hand )  {

////        qWarning() << "Features_and_hand";

//        // Lo siguiente es para la deteccion de la manos, sin no esta isBackgroundCapturado no lo hace
//        if ( isBackgroundCapturado )  {

//            this->borrarRostro( *(this->getCameraTexture()) );

//            this->sustraerFondo( *(this->getCameraTexture()), true );

//            this->processFingers( *this->getCameraTexture() );


//        }


//        // Lo siguiente es para la deteccion de la sonrisa, sin no esta isBackgroundCapturado solo hace esto

//        vector< Rect > detectedFaces;
//        detectedFaces.clear();

//        frontalFaceClassifier->detectMultiScale( *this->getCameraTexture(), detectedFaces,
//                                         1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

//        if( detectedFaces.size() > 0 )
//        {
//            vector< Rect > detectedSmiles;
//            detectedSmiles.clear();

//            Mat face( *this->getCameraTexture(), detectedFaces.at( 0 ) );

//            smileClassifier->detectMultiScale( face, detectedSmiles,
//                                              1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 50, 50 ) );

//            // Este if y else es para que no detecte una sonrisa en frames consecutivos.
//            // Algo asi como un antirebote.
//            if( detectedSmiles.size() > 0 )
//            {
//                if( ! this->getSmiling() )
//                {
//                    this->setSmiling( true );
//                    emit signal_sonrisa();
//                }
//            }
//            else
//            {
//                if( smiling )
//                {
//                    this->setSmiling( false );
//                }
//            }

//        }
//    }



//    if ( tipoDeteccionActual & Features_and_hand )  {

////        qWarning() << tipoDeteccionActual << "Features_and_hand";


////        handDetector->fistDetection( *this->getCameraTexture(), refSkin->minimumHue, refSkin->maximumHue);

//        vector< Rect > faces;
//        faces.clear();

//        frontalFaceClassifier->detectMultiScale( *this->getCameraTexture(), faces,
//                                                 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

//        Rect roi( 0, 0, 0, 0 );

//        if( faces.size() > 0 )
//        {
//            roi = faces.at( 0 );

//            // actually noise tip region

//            roi.x = roi.x + roi.width / ( double )3;
//            roi.y = roi.y + roi.height / ( double )2;
//            roi.width = roi.width * 1 / ( double )3;
//            roi.height = roi.height * 1 / ( double )3;
//        }

//        // Esta es la region de la nariz
//        QRect rectRoi( roi.x, roi.y, roi.width, roi.height );


//        if( this->isNeedCalibrated )
//        {
//            // Este containerRect es 0,0 640x480
//            PointMapper::getInstance()->setContainerRect( QRect( 0, 0,
//                      this->getVideoCapture()->get( CV_CAP_PROP_FRAME_WIDTH ),
//                      this->getVideoCapture()->get( CV_CAP_PROP_FRAME_HEIGHT ) ) );

//            // Este es el rectangulo de la nariz
//            PointMapper::getInstance()->setOriginRect( rectRoi );

//            // Este es el rectangulo de la pantalla completa. Por ejemplo QRect(0,0 1366x768)
//            PointMapper::getInstance()->setTargetRect( QApplication::desktop()->screenGeometry() );
////                PointMapper::getInstance()->setTargetRect( QRect(0,0, 100, 1) );

//            FeaturesProcessor::getInstance()->setNeedToInitFT( true );
//            FeaturesProcessor::getInstance()->setInitialFrames( true );
//            FeaturesProcessor::getInstance()->setInitialFramesCounter( 0 );
//            FeaturesProcessor::getInstance()->setFirstCentroid( true );
//            FeaturesProcessor::getInstance()->setDrawProcessing( false );  // Para que no imprima mensajes OSD

//            this->isNeedCalibrated = false;
//        }

//        QPoint target = FeaturesProcessor::getInstance()->process( this->getCameraTexture(), rectRoi );

//        bool deseaControlarMouse = false;

//        if( deseaControlarMouse )
//        {
//            QCursor::setPos( PointMapper::getInstance()->map( target ) );
//        }

//        emit signal_cursorTracking( target );
//    }
}



void Camera::calibrate()
{
    calibration.x = currentFace.x + currentFace.width / 4;
    calibration.y = currentFace.y + currentFace.height / 4;
    calibration.width = currentFace.width / 2;
    calibration.height = currentFace.height / 2;

    this->isCalibrated = true;
}


double Camera::distance( Point a, Point b )
{
    return ( a.x - b.x ) * ( a.x - b.x ) + ( a.y - b.y ) * ( a.y - b.y );
}

void Camera::processMenuPrincipal(Mat &frame)
{
    vector< Rect > detectedFaces;
    detectedFaces.clear();

    frontalFaceClassifier->detectMultiScale( frame, detectedFaces,
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

            Mat face( frame, detectedFaces.at( 0 ) );

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

        Mat face( frame, detectedFaces.at( 0 ) );

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

void Camera::processSmile(Mat &frame)
{
    // Lo siguiente es para la deteccion de la sonrisa, sin no esta isBackgroundCapturado solo hace esto

    vector< Rect > detectedFaces;
    detectedFaces.clear();

    frontalFaceClassifier->detectMultiScale( frame, detectedFaces,
                                     1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

    if( detectedFaces.size() > 0 )
    {
        vector< Rect > detectedSmiles;
        detectedSmiles.clear();

        Mat face( frame, detectedFaces.at( 0 ) );

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

/**
 * @brief Camera::borrarRostro
 * @param frame
 * @param negro Elimina la franga vertical del rostro y lo pone con negro, si no, blanco
 */
void Camera::borrarRostro(Mat &frame, bool negro)
{
    std::vector< Rect > vCarasDetectadas;
    vCarasDetectadas.clear();
    frontalFaceClassifier->detectMultiScale( frame, vCarasDetectadas,
                                            1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 100, 100 ) );

    if( vCarasDetectadas.size() > 0 )  {

        Scalar color;

        if ( negro )  {
            color = Scalar( 0, 0, 0 );
        }
        else  {
            color = Scalar( 255, 255, 255 );
        }

        // Son los pixeles que se dejan a cada costado de la cara, para agrandarlo un poco
        int margen = 35;

        cv::Mat paraEliminarRostro( frame.rows, vCarasDetectadas.at( 0 ).width + 2 * margen,
                                    frame.type(), color );

//        cv::Mat paraEliminarRostro( frame.rows, vCarasDetectadas.at( 0 ).width,
//                                    frame.type(), color );

        // Tener en cuenta que la imagen esta al reves

        int x = vCarasDetectadas.at( 0 ).x - margen;
        x = qMax( x, 0 );

        int w = paraEliminarRostro.cols;
        w = qMin( w, frame.cols - x);

        Size size(w,paraEliminarRostro.rows);//the dst image size,e.g.100x100
        resize(paraEliminarRostro, paraEliminarRostro,size);

        paraEliminarRostro.copyTo( frame ( Rect( x, 0,
                                                 paraEliminarRostro.cols,
                                                 paraEliminarRostro.rows ) ) );

//        line( frame, Point(vCarasDetectadas.at( 0 ).x, 0),
//              Point(vCarasDetectadas.at( 0 ).x, 480), Scalar( 128, 128, 128 ), 1 );

//        paraEliminarRostro.copyTo( frame ( Rect( vCarasDetectadas.at( 0 ).x, 0,
//                                                 paraEliminarRostro.cols,
//                                                 paraEliminarRostro.rows ) ) );

    }
}

void Camera::sustraerFondo( Mat &frame, bool ponerEnBlanco )
{

//    Mat frame = (*this->getCameraTexture()).clone();


    // Computes a foreground mask.
    mog2.dynamicCast<cv::BackgroundSubtractorMOG2>()->apply( frame, fore, 0 );

    cv::Mat kernel3x3 = (cv::Mat_<uchar>(5,5) <<   0,1,0,
                                                   1,1,1,
                                                   0,1,0);

    cv::Mat kernel5x5 = (cv::Mat_<uchar>(5,5) <<   0,0,1,0,0,
                                                   0,1,1,1,0,
                                                   1,1,1,1,1,
                                                   0,1,1,1,0,
                                                   0,0,1,0,0);

    //Enhance edges in the foreground by applying erosion and dilation
    erode(fore, fore, kernel3x3);
    dilate(fore, fore, kernel3x3);

    mog2.dynamicCast<cv::BackgroundSubtractorMOG2>()->getBackgroundImage( back );

    // PONE EN BLANCO EL BACKGROUND.
    if ( ponerEnBlanco )  {
        cv::Mat frameConMascara( frame.rows, frame.cols, frame.type(), cv::Scalar( 255, 255, 255 ) );
        frame.copyTo( frameConMascara, fore);
        frame = frameConMascara.clone();
    }

//        // MUESTRA EL BACK ALMACENADO EN UN CUADRITO ARRIBA A LA IZQUIERDA
//            bool detectMostrarBackOption = true;

//        if ( detectMostrarBackOption )  {
//            cv::Mat backResized(48*2,64*2, back.type());
//            cv::resize(back, backResized, backResized.size(), 0, 0, INTER_CUBIC);

//            backResized.copyTo( (*this->getCameraTexture())(Rect(0, 0,
//                                                                      backResized.cols,
//                                                                      backResized.rows)));
//        }
//        // MUESTRA EL BACK ALMACENADO EN UN CUADRITO ARRIBA A LA IZQUIERDA //////////////// FIN


}

HandDetection *Camera::getHandDetector() const
{
    return handDetector;
}

void Camera::setYaSeTomoOriginal(bool value)
{
    yaSeTomoOriginal = value;
}

bool Camera::getProcesarUnaSolaImagen() const
{
    return procesarUnaSolaImagen;
}

void Camera::setProcesarUnaSolaImagen(bool value)
{
    procesarUnaSolaImagen = value;
}


void Camera::processFingers( Mat &frame )
{
    // Filtramos por color

    Mat hsvFrame;
    //    cvtColor( frame, hsvFrame, CV_BGR2HSV );  // Del Emi
    cvtColor( frame, hsvFrame, CV_BGR2Lab );  // Del Cesar

    std::vector< uint8_t > mask;

    for( int j = 0; j < hsvFrame.rows; j++ )
    {
        for( int i = 0; i < hsvFrame.cols; i++ )
        {
            Vec3b color = hsvFrame.at< Vec3b >( Point( i, j ) );

// Esto es del Emi que elige hue, sat y val haciendo clic en la pantalla
//            if( color[0] >= refSkin->minimumHue && color[0] <= refSkin->maximumHue &&
//                color[1] >= refSkin->minimumSat && color[1] <= refSkin->maximumSat &&
//                color[2] >= refSkin->minimumVal && color[2] <= refSkin->maximumVal )
//            {
//                mask.push_back( 255 );
//            }
//            else
//            {
//                mask.push_back( 0 );
//            }

            // Yo solo controlo el a del Lab sacado de los QSlider
            if( color[1] >= this->getSbDownValue() && color[1] <= this->getSbUpValue() )
            {
                mask.push_back( 0 );
            }
            else
            {
                mask.push_back( 255 );
            }

        }
    }

    Mat binary( Size( hsvFrame.cols, hsvFrame.rows ), CV_8UC1, ( void* )mask.data() );

    // Erosion y dilatacion de la imagen binaria

//    Mat matrix = ( Mat_< uchar >( 7, 7 ) << 0,0,1,1,1,0,0,
//                                            0,1,1,1,1,1,0,
//                                            1,1,1,1,1,1,1,
//                                            1,1,1,1,1,1,1,
//                                            1,1,1,1,1,1,1,
//                                            0,1,1,1,1,1,0,
//                                            0,0,1,1,1,0,0);

    int erosion_size = 9;
    Mat matrix = getStructuringElement( MORPH_CROSS,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ) );


    erode( binary, binary, matrix );
    dilate( binary, binary, matrix );

    Mat binaryCopy = binary.clone();

    // Cierre convexo

    vector< Point > points;

    for( int i = 0; i < binary.rows; i++ )
    {
        for( int j = 0; j < binary.cols; j++ )
        {
            if( ( ( int )binary.at< uchar >( i, j ) ) == 255 )
            {
                points.push_back( Point( j, i ) );
            }
        }
    }

    vector< Point > hull;
    hull.clear();

    convexHull( Mat( points ), hull, false );

    // Dibujamos extremos

    int minimumDistance = 3000;

    vector< Point > baricenters;

    if( hull.size() > 1 )
    {
        if( distance( hull.at( 0 ), hull.at( ( hull.size() - 1 ) ) ) > minimumDistance )
        {
            baricenters.push_back( hull.at( 0 ) );
        }
    }

    for( unsigned int i = 1; i < hull.size(); i++ )
    {
        if( distance( hull.at( i ), hull.at( i - 1 ) ) > minimumDistance )
        {
            baricenters.push_back( hull.at( i ) );
        }
    }

    // Centro de masa segun cierre convexo

    if( baricenters.size() > 0 )
    {
        baricenter.x = 0;
        baricenter.y = 0;

        for( unsigned int i = 0; i < baricenters.size(); i++ )
        {
            baricenter.x += baricenters.at( i ).x;
            baricenter.y += baricenters.at( i ).y;
        }

        baricenter.x /= baricenters.size();
        baricenter.y /= baricenters.size();

        // Dibuja el centro de la palma
        circle( frame, baricenter, 10 , Scalar( 255, 0, 255 ), 2 );

        // Dibuja lineas grises desde el centro a los bordes
        for( unsigned int i = 0; i < baricenters.size(); i++ )
        {
            line( frame, baricenters.at( i ), baricenter, Scalar( 128, 128, 128 ), 1 );
        }
    }

    // Dibujamos bordes, Entre las puntas de los dedos
    if( hull.size() > 1 )
        line( frame, hull.at( 0 ), hull.at( ( hull.size() - 1 ) ), Scalar( 128, 128, 128 ), 1 );

    double distanciaEuclidian = 0;
    double sumatoriaDistancia = 0;
    double sumatoriaVarianzaDistancia = 0;
    float promediaDistancia = 0;
    float varianzaDistancia = 0;

    // Dibujamos bordes, Entre las puntas de los dedos
    for( unsigned int i = 1; i < hull.size(); i++ )
    {
        line( frame, hull.at( i ), hull.at( i - 1 ), Scalar( 128, 128, 128 ), 1 );

        distanciaEuclidian = cv::norm(hull.at( i ) - hull.at( i - 1 ));//Euclidian distance
        sumatoriaDistancia += distanciaEuclidian;
//        qDebug() << "distanciaEuclidian=" << distanciaEuclidian;

    }

    promediaDistancia = sumatoriaDistancia/(hull.size()-1);
//    qDebug() << "promediaDistancia=" << promediaDistancia;

    for( unsigned int i = 1; i < hull.size(); i++ )
    {

        distanciaEuclidian = cv::norm(hull.at( i ) - hull.at( i - 1 ));//Euclidian distance
        sumatoriaVarianzaDistancia = sumatoriaVarianzaDistancia +
                (distanciaEuclidian-promediaDistancia) * (distanciaEuclidian-promediaDistancia);
//        qDebug() << "sumatoriaVarianzaDistancia=" << sumatoriaVarianzaDistancia;

    }

    varianzaDistancia = sqrt(sumatoriaVarianzaDistancia/(hull.size()-1));
//    qDebug() << "varianzaDistancia = " << varianzaDistancia;


    // Buscamos los contornos en la imagen binaria
    vector< vector< Point > > contours;
    findContours( binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );

    relevants.clear();
    int fingers = 1;

    for( unsigned int i = 0 ; i < contours.size(); i++ )
    {
        // Ignoramos las areas insignificantes
        if( contourArea( contours[i] ) >= 3000 )
        {
            // Detectamos cierre convexo en el contorno actual
            vector<vector< Point > > hulls( 1 );
            vector<vector< int > > hullsI( 1 );
            convexHull( Mat( contours[i] ), hulls[0], false );
            convexHull( Mat( contours[i] ), hullsI[0], false );

            // Buscamos defectos de convexidad
            vector< Vec4i > defects;
            if ( hullsI[0].size() > 0 )
            {
                convexityDefects( contours[i], hullsI[0], defects );
                if( defects.size() >= 3 )
                {

                    float umbral = 0;  // umbral para la profundidad de la concavidad
                    float sumatoria = 0;
                    float promedio = 0;
                    float varianza = 0;

                    // Para el promedio
                    for( unsigned int j = 0; j < defects.size(); j++ )
                    {
                        float depth = defects[j][3] / 256;

                        sumatoria += depth;
                    }
                    promedio = sumatoria/defects.size();
//                    qDebug() << "Promedio = " << promedio;

                    sumatoria = 0;  // Ponemos en cero para usarla para la varianza

                    // Para la varianza
                    for( unsigned int j = 0; j < defects.size(); j++ )
                    {
                        float depth = defects[j][3] / 256;
                        sumatoria = sumatoria + (depth-promedio) * (depth-promedio);
                    }

                    varianza = sqrt(sumatoria/defects.size());
//                    qDebug() << "Varianza = " << varianza;


                    for( unsigned int j = 0; j < defects.size(); j++ )
                    {
                        // the farthest from the convex hull point within the defect
                        float depth = defects[j][3] / 256;

//                        qDebug() << depth;

                        // Filtramos por profundidad
//                        if( depth > 100 )  // Umbral para que no detecte valle en la muneca
//                        if( depth > 70 )
                        if( depth > promedio )
                        {
                            // Entra a este if cada vez que supere esta depth, deberia detectar 4 depth que superen
                            // este umbral. Es decir, detectar los 4 valles. Si el antebrazo se muestra mucho,
                            // entonces quizas se detecte un valle mas, que parezca un sexto dedo.

                            // Cuando la mano se aleja, los depth no llegan a superar el umbral. Deberia fijarse
                            // como umbral un porcentaje y no una distancia fija.

                            // Valles = convexity defects = defectos de convexidad
                            // Envoltura convexa = convex hull

                            // creo que ptStart representa donde comieza la punta del dedo (circulo azul)
                            int startidx = defects[j][0];
                            Point ptStart( contours[i][startidx] );

                            // ptFar es donde finaliza el dedo (circulo Rojo)
                            int endidx = defects[j][1];
                            Point ptEnd( contours[i][endidx] );

                            // donde se encuentra cada valle
                            int faridx = defects[j][2];
                            Point ptFar( contours[i][faridx] );

//                            qDebug() << "cv::norm(ptStart - ptEnd)"  << cv::norm(ptStart - ptEnd);


//                            if ( cv::norm(ptStart - ptEnd) < varianzaDistancia )  {

                                circle( frame, ptStart, 8, Scalar( 255, 0, 0 ), 2 );
                                circle( frame, ptFar, 8, Scalar( 0, 255, 0 ), 2 );
                                circle( frame, ptEnd, 8, Scalar( 0, 0, 255 ), 2 );
//                            }

                            relevants.push_back( ptStart );
                            relevants.push_back( ptFar );
                            relevants.push_back( ptEnd );

                            fingers++;
                        }
                    }
                }
            }
        }
    }

    if( fingers > 1 )
    {
        QString text( "Dedos: " + QString::number( fingers ) );
        putText( frame, text.toStdString(), Point( 10, 30 ), 1, 2, Scalar( 255, 0, 0 ) );
    }

    // Aca se detecta la interaccion para cambiar de modelo a dibujar
    if( fingers == 5  && lastFingers == 4 )
    {
        emit signal_deteccionDedosJuntos();
//        textureIndex++;
//        modelIndex++;

//        if( textureIndex >= textures->size() ) textureIndex = 0;
//        if( modelIndex >= models->size() ) modelIndex = 0;
    }

    if ( fingers == 1 || fingers == 2 || fingers == 3 )
        emit manoAbiertaCerrada( false );

    if ( fingers >= 4 )
        emit manoAbiertaCerrada( true );

    if ( fingers == 0 )
        emit manoAbiertaCerrada( false );

    lastFingers = fingers;

    // Mostramos miniatura
    Mat preview( binaryCopy.rows, binaryCopy.cols, CV_8UC3 );
    cvtColor( binaryCopy, preview, CV_GRAY2BGR );
    Mat previewResized( 96, 128, CV_8UC3 );
    cv::resize( preview, previewResized, previewResized.size(), 0, 0, INTER_CUBIC );
    previewResized.copyTo( frame( Rect( frame.cols - 135, frame.rows - 103, 128, 96 ) ) );


}
