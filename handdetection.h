#ifndef HANDDETECTION_H
#define HANDDETECTION_H

#include "skinfilter.h"

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class HandDetection
{

public:
    HandDetection();

    SkinFilter *refSkin;

    // processFrames se encarga de hacer el procesamiento basico
    // a cada frame para poder detectar la mano
    bool processFrames(Mat &frame, int minHue, int maxHue);
    bool fistDetection(Mat &frame, int minHue, int maxHue, bool alterFrame = true);

private:

    //calculo promedio cantidad pixeles color piel
    int count;
    double sumatory;
    double avgPx;


    // punto donde esta el centro de masa.
    // es un punto tal, que cualquier recta que pasa por él,
    // divide a dicho segmento en dos partes de igual momento respecto a dicha recta
    Point baricenter;

    // vector para almacenar los puntos de los dedos (?)
    vector< Point > relevants;

    // cantidad de dedos
    int fingers;


    ///------------------- metodos de processFrame--------------------------

    //calculos
    void filtrarPorColor(Mat &hsvFrame, std::vector<uint8_t> &mask);
    void erosionYdilatacion(Mat &binary, int erosion_size, int type = cv::MORPH_CROSS);
    void PointsForConvexHull(vector<Point> &points, Mat &binary);
    Point getBaricenter(vector< Point > baricenters, Mat &frame);
    float getPromediaDistancia(vector< Point > hull, double sumatoriaDistancia);
    double getVarianzaDist(vector< Point > hull, double distanciaEuclidian, float promediaDistancia);
    float getDesvioEstandar(double sumatoriaVarianzaDistancia, int size);
    double distance( Point a, Point b );
    double distanceSqrt( Point a, Point b);
    bool isHandInFrame(Mat &hsvFrame, int minHue, int maxHue);
    void calcBaricentersPoints(vector<Point> &hull, vector<Point> &baricenters, int minumunDistance);
    // metodo que hace el trabajo pesado.
    void findConvexityDefects(Mat &frame, vector< vector< Point > > contours, int &fingers);

    //dibujo
    void drawLinesFromPoint(vector<Point> baricenters, Point baricenter, Mat &frame);
    void dibujarLineasEntreDedos(Mat &frame, vector<Point> hull, double &distanciaEuclidian, double &sumatoriaDistancia);
    void drawLinesBetweenFingers(Mat &frame, vector<Point> hull, double &distanciaEuclidian, double &sumatoriaDistancia);
    void drawLitleBinaryImage(Mat &frame, Mat &binaryCopy);
};

#endif // HANDDETECTION_H
