#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QObject>
#include <QTimer>

#include <vector>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "common.hpp"

#include "interface/camerawidget.h"

#define FRAMES_FACELESS 70
#define FRAMES_FACELESS_BACK 50

using namespace cv;
using namespace std;

class Camera : public QObject
{
    Q_OBJECT

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

public:

    explicit Camera( QObject *parent = NULL );

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
};

#endif // CAMERA_HPP
