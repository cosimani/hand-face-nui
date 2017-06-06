#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QObject>
#include <QTimer>
#include <QPoint>

#include <vector>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/background_segm.hpp>

#include "common.hpp"

#include "interface/camerawidget.h"
#include "handdetection.h"

#define FRAMES_FACELESS 70
#define FRAMES_FACELESS_BACK 50

using namespace cv;
using namespace std;

class Camera : public QObject
{
    Q_OBJECT

public:

    enum TipoDeteccion { HaarCascades = 1,
                         Features = 2,
                         OnlySmile = 4,
                         BackgroundCapture = 8,
//                         Features_and_hand = 32,
                         Hand = 16,
                         Features_and_hand = Hand | Features,
                         Hand_and_Smile = Hand | OnlySmile
                       };

    explicit Camera( QObject *parent = NULL );
    virtual ~Camera();

    VideoCapture *getVideoCapture() const;
    void setVideoCapture( VideoCapture *value );

    QTimer *getSceneTimer() const;
    void setSceneTimer( QTimer *value );

    Mat *getCameraTexture() const;
    void setCameraTexture( Mat *value );

    CascadeClassifier *getFrontalFaceClassifier() const;
    void setFrontalFaceClassifier( CascadeClassifier *value);

    CascadeClassifier *getSmileClassifier() const;
    void setSmileClassifier( CascadeClassifier *value);

    int getSize() const;
    void setSize( int value );

    bool getSmiling() const;
    void setSmiling( bool value );

    void setTipoDeteccion( TipoDeteccion tipo );

    TipoDeteccion getTipoDeteccionActual() const;

    bool getIsNeedCalibrated() const;
    void setIsNeedCalibrated(bool value);



    void setMuestraProcesada(bool value);

    bool getMuestraProcesada() const;

    void capturarFondo();

    void setSbUpValue(int value);

    void setSbDownValue(int value);

    int getSbUpValue() const;

    int getSbDownValue() const;

    bool getProcesarUnaSolaImagen() const;
    void setProcesarUnaSolaImagen(bool value);

    void setYaSeTomoOriginal(bool value);

    HandDetection *getHandDetector() const;

private:

    VideoCapture *videoCapture;
    QTimer *sceneTimer;
    Mat *cameraTexture;

    CascadeClassifier *frontalFaceClassifier;
    CascadeClassifier *smileClassifier;

    Rect currentFace;
    Rect calibration;

    /**
     * @brief size Es la cantidad de botones que tiene horizontalmente, en este caso, la interfaz. Los lee desde los xml.
     */
    int size;

    bool smiling;

    /**
     * @brief isCalibrated Se utiliza para saber si se calibro o no. Es decir, si este bool es false, entonces no se tiene
     * control de la interfaz, requiere que se pulse el boton Calibrar o bien, sonreir. Es bool se pondra en false nuevamente
     * cuando no se detecte la cara con Haar.
     */
    bool isCalibrated;

    /**
     * @brief countFrameFaceless Para acumular la cantidad de frames que no se detectan rostros, y asi reiniciar la frase
     */
    int countFrameFaceless;

    TipoDeteccion tipoDeteccionActual;

    bool isNeedCalibrated;

    Ptr<cv::BackgroundSubtractorMOG2> mog2;
    int backgroundFrame;
    bool muestraProcesada;
    Mat back;
    Mat fore;
    int sbUpValue;
    int sbDownValue;
    bool isBackgroundCapturado;

    Point baricenter;
    int lastFingers;
    vector< Point > relevants;


    void processFingers(Mat &frame);

    double distance(Point a, Point b);

    void processMenuPrincipal( Mat &frame );
    void processSmile( Mat &frame );
    void borrarRostro(Mat &frame , bool negro = false);
    void sustraerFondo( Mat &frame , bool ponerEnBlanco = true );

    Mat frameParaProcesar;
    Mat originalframeParaProcesar;
    bool procesarUnaSolaImagen;
    bool yaSeTomoOriginal;

    HandDetection * handDetector;
    // referencia de los colores de la piel
    SkinFilter *refSkin;



private slots:

    void process();


public slots:

    void calibrate();

signals:

    void positionDetected( int index );
    void selectionDetected( int index );
    void signal_newCameraFrame( cv::Mat * cameraTexture );

    /**
     * @brief signal_nonFaceDetected Se dispara cuando se no se detecta una cara luego de una cantidad de FRAMES_FACELESS
     * frames.
     */
    void signal_nonFaceDetected();

    /**
     * @brief signal_nonFaceDetectedForBack Se dispara cuando se no se detecta una cara luego de una cantidad de
     * FRAMES_FACELESS_BACK frames. Es para volver atras en la frase que se esta armando.
     */
    void signal_nonFaceDetectedForBack();

    /**
     * @brief signal_sonrisa Se emite para indica que la sonrisa fue detectada. Requiere OnlySmile activo.
     */
    void signal_sonrisa();


    /**
     * @brief signal_cursorTracking Se emite cuando se detecta con Features. Envia la posicion donde
     * deberia quedar el mouse, en caso de desear controlarlo.
     */
    void signal_cursorTracking( QPoint );


    void manoAbiertaCerrada( bool );
    void message( QString text );

    void signal_deteccionDedosJuntos();
};

#endif // CAMERA_HPP
