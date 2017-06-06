#include "handdetection.h"

#include <QDebug>

HandDetection::HandDetection(QObject *parent) : QObject(parent), refSkin( new SkinFilter() ),
    fingers(0), count(0), sumatory(0), avgPx(0)
{

    estado_hayMano_o_no = false;  // Por defecto no hay mano
    estado_manoAbierta_o_cerrada = true;  // Por defecto abierta


}

/**
  retorna true si encuentra puño cerrado, en cualquier otro caso retorna false
  modifica frame, agregando texto. le pasamos false al ultimo parametro si no queremos que
  modifique el frame.
*/
bool HandDetection::fistDetection(Mat &frame, int minHue, int maxHue, bool alterFrame)
{
    if( ! (processFrames(frame, minHue, maxHue)) ){
        QString text( "No hay mano en la escena:");
        if(alterFrame)
            putText( frame, text.toStdString(), Point( 10, 30 ), 1, 2, Scalar( 255, 0, 0 ) );
        return false;
    }

    if(fingers <= 2){
        QString text( "Mano cerrada" );
        if(alterFrame)
            putText( frame, text.toStdString(), Point( 10, 30 ), 1, 2, Scalar( 255, 0, 0 ) );

        if ( estado_manoAbierta_o_cerrada == true )  {
            estado_manoAbierta_o_cerrada = false;
            emit signal_estado_manoAbierta_o_cerrada( false );
        }

        return true;
    }else if (fingers >= 3){
        QString text( "Mano abierta" );
        if(alterFrame)
            putText( frame, text.toStdString(), Point( 10, 30 ), 1, 2, Scalar( 255, 0, 0 ) );

        if ( estado_manoAbierta_o_cerrada == false )  {
            estado_manoAbierta_o_cerrada = true;
            emit signal_estado_manoAbierta_o_cerrada( true );
        }

        return false;
    }

    return false;
}


/**
  jr: hace lo mismo que process, pero lo separe en metodos para que sea mas legible.
*/
bool HandDetection::processFrames(Mat &frame, int minHue, int maxHue)
{
    refSkin->addMinHue(minHue);
    refSkin->addMaxHue(maxHue);
    Mat hsvFrame;
    std::vector< uint8_t > mask;
    cvtColor( frame, hsvFrame, CV_BGR2Lab );

    /// pregunto si existe una mano, sino dejo de analizar
    if ( ! isHandInFrame(hsvFrame, minHue, maxHue))  {
        return false;
    }

    filtrarPorColor(hsvFrame, mask);

    // creo matriz binaria
    Mat binary( Size( hsvFrame.cols, hsvFrame.rows ), CV_8UC1, ( void* )mask.data() );
    // Erosion y dilatacion de la imagen binaria
    erosionYdilatacion(binary,9);
    Mat binaryCopy = binary.clone();

    // Cierre convexo
    vector< Point > points;
    PointsForConvexHull(points, binary);

    vector< Point > hull;
    hull.clear();
    // calculamos el cierre convexo y lo guardamos en hull
    convexHull( Mat( points ), hull, false );

    // Dibujamos extremos
    int minimumDistance = 3000;
    vector< Point > baricenters;
    calcBaricentersPoints(hull,baricenters,minimumDistance);

    // Centro de masa segun cierre convexo
    baricenter = getBaricenter(baricenters, frame);


    // Dibujamos bordes, Entre las puntas de los dedos
    double EuclidianDistance = 0;
    double sumatoriaDistancia = 0;
    // dibuja lineas que unen las puntas de los dedos
    // nos dibuja una figura convexa
    drawLinesBetweenFingers(frame, hull, EuclidianDistance, sumatoriaDistancia);

    // calculo Promedio de la distancia entre los puntos del cierre convexo
    float promediaDistancia = getPromediaDistancia(hull, sumatoriaDistancia);

    // sumatoria de la varianza de la distancia
    double sumatoriaVarianzaDistancia = getVarianzaDist(hull, EuclidianDistance, promediaDistancia);

    // DesvioEstandar. // no lo usa
    float DesvioEstandarDistancia = getDesvioEstandar(sumatoriaVarianzaDistancia, (hull.size() - 1) );

    // Buscamos los contornos en la imagen binaria
    vector< vector< Point > > contours;
    findContours( binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );


    relevants.clear();
    fingers = 1;
    // hallamos los dectos en la convexidad (valles entre los dedos) y contamos los dedos
    findConvexityDefects(frame, contours, fingers);

    // dibujo texto con la cantidad de dedos
    if( fingers > 1 )
    {
        QString text( "Dedos: " + QString::number( fingers ) );
        putText( frame, text.toStdString(), Point( 10, 60 ), 1, 2, Scalar( 255, 0, 0 ) );
    }

    // dibujar imagen binaria minuatura
    drawLitleBinaryImage(frame, binaryCopy);

    return true;
}



//------------------------------METODOS AUXILIARES processFrames---------------------------------------

// 10mo que se llama.
// detecta los valles entre los dedos, dibuja circulos en las puntas y valles
// cuenta la cantidad de dedos.
void HandDetection::findConvexityDefects(Mat &frame, vector< vector< Point > > contours, int &fingers )
{
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
//                    qDebug() << "\n==============================";

                    float umbral = 0;  // umbral para la profundidad de la concavidad
                    float sumatoria = 0;
                    float promedio = 0;
                    float varianza = 0;

                    // Para el promedio
                    for( unsigned int j = 0; j < defects.size(); j++ )
                    {
                        // fixpt_depth is fixed-point approximation (with 8 fractional bits)
                        // of the distance between the farthest contour point and the hull
                        float depth = defects[j][3] / 256;

                        sumatoria += depth;
                    }

                    promedio = sumatoria/defects.size();
                    //qDebug() << "Promedio = " << promedio;

                    sumatoria = 0;  // Ponemos en cero para usarla para la varianza

                    // Para la varianza
                    for( unsigned int j = 0; j < defects.size(); j++ )
                    {
                        float depth = defects[j][3] / 256;
                        sumatoria = sumatoria + (depth-promedio) * (depth-promedio);
                    }

                    varianza = sqrt(sumatoria/defects.size());

//                    qDebug() << "\ndefects" << defects.size();

                    for( unsigned int j = 0; j < defects.size(); j++ )
                    {
                        // the farthest from the convex hull point within the defect
                        float depth = defects[j][3] / 256;

//                        if( depth > 70)
                        if( depth > promedio)
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

                            ///jr: tengo que controlar que la distancia entre dos puntos
                            /// sea mayor (en principio) a una distancia arbitraria
//                            qDebug() << "distance:" << distance(ptEnd, ptFar);
//                            if( distance(ptEnd, ptFar) < 1000)
//                                break; // con este valor a aprox 40 cm deja de detectar el dedo menique porque es muy corto


//                            qDebug() << "distance:" << distance(ptStart, ptFar);
//                            if( distance(ptStart, ptEnd) < 20)
//                                break; // con este valor a aprox 40 cm deja de detectar el dedo menique porque es muy corto

                            if( distanceSqrt( ptStart, ptEnd ) < 30 )  {
                                continue;
                            }

                            if( distanceSqrt( ptStart, ptFar ) < 30 )  {
                                continue;
                            }

                            if( distanceSqrt( ptEnd, ptFar ) < 30 )  {
                                continue;
                            }

//                            qDebug() << "ptStart" << j << " - " << ptStart.x << ptStart.y;
//                            qDebug() << "ptEnd" << j << " - " << ptEnd.x << ptEnd.y;
//                            qDebug() << "ptFar" << j << " - " << ptFar.x << ptFar.y;
//                            qDebug() << "distance ptStart, ptEnd:" << j << " - " << distanceSqrt(ptStart, ptEnd);
//                            qDebug() << "distance ptEnd, ptFar:" << j << " - " << distanceSqrt(ptEnd, ptFar);
//                            qDebug() << "distance ptStart, ptFar:" << j << " - " << distanceSqrt(ptStart, ptFar);


                            ///jr. le pongo esto para que no dibuje puntos debajo de 100px del centro de masa
                            ///if(ptStart.y > baricenter.y + 80 ) break;

//                            if ( cv::norm(ptStart - ptEnd) < varianzaDistancia )  {

                                circle( frame, ptStart, 8, Scalar( 255, 0, 0 ), 2 ); // circulo azul en punta de los dedos
                                circle( frame, ptFar, 8, Scalar( 0, 255, 0 ), 2 ); // verde. valles
                                circle( frame, ptEnd, 8, Scalar( 0, 0, 255 ), 2 ); // rojo punta dedos
//                            }

                            relevants.push_back( ptStart );
                            relevants.push_back( ptFar );
                            relevants.push_back( ptEnd );

                            fingers++;


                            ///jr. limito que solo detecte hasta 5 dedos
//                            if(fingers == 5){
////                                qDebug() << "dedos" << fingers;
//                                break;
//                            }
                        }
                    }
                    ///jr. limito que solo detecte hasta 5 dedos
//                    if(fingers == 5) break;
                }
            }
        }
    }
}

// 1ro que se llama
void HandDetection::filtrarPorColor(Mat &hsvFrame, std::vector< uint8_t > &mask)
{
    int sum = 0;
    for( int j = 0; j < hsvFrame.rows; j++ ){
        for( int i = 0; i < hsvFrame.cols; i++ ){
            Vec3b color = hsvFrame.at< Vec3b >( Point( i, j ) );

            if( color[1] >= refSkin->minimumHue && color[1] <= refSkin->maximumHue ){
                mask.push_back( 0 );
            }else{
                mask.push_back( 255 );
                // suma la cantidad de px en el rango del color de la piel por frame
                sum++;
            }
        }
    }

    if(sum > 5000){
        // cantidad de frames que analizo
        count++;
        sumatory+=sum;
        avgPx = sumatory/(double)count;
//        qDebug() << "promedio pixels blancos por mano" << avgPx;
        /*
         * -----test eleccion umbral para saber si existe una mano-----
         *
         * calcule el promedio de pixeles blancos cuando habia una mano en el frame
         * en buenas condiciones (se veia bien la mano en la imagen binaria)
         * luego de 2 pruebas de aprox 15 seg.
         * fui variando la distancia entre distancia media, corta y larga
         * resultado:
         * avgPx1 = 59860.1
         * avgPx2 = 58498.4
         * entonces voy a poner un unmbral que cuando se detecten 55000 px de color piel
         * hay UNA mano detectada
         *
         */
    }
}
// 2do que se llama
void HandDetection::erosionYdilatacion(Mat &binary, int erosion_size, int type)
{
    Mat matrix = getStructuringElement( type,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ) );

    // erocion y dilatacion a la imagen binaria
    // @params (src, dst, kernel) para ambos
    erode( binary, binary, matrix );
    dilate( binary, binary, matrix );
}

// 3ro que se llama
void HandDetection::PointsForConvexHull(vector< Point > &points, Mat &binary)
{
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
}

// 4to que se llama.
// obtengo los puntos mayores a minimumDistance que estan en el cierre convexo calculado
void HandDetection::calcBaricentersPoints(vector< Point > &hull, vector< Point > &baricenters, int minumunDistance)
{
    if( hull.size() > 1 )
    {
        if( distance( hull.at( 0 ), hull.at( ( hull.size() - 1 ) ) ) > minumunDistance )
        {
            baricenters.push_back( hull.at( 0 ) );
        }
    }

    for( unsigned int i = 1; i < hull.size(); i++ )
    {
        if( distance( hull.at( i ), hull.at( i - 1 ) ) > minumunDistance )
        {
            baricenters.push_back( hull.at( i ) );
        }
    }
}

// 5to que se llama.
// calcula el centro de masa a partir del vector de puntos de baricente
Point HandDetection::getBaricenter(vector<Point> baricenters, Mat &frame)
{
    Point baricenter;
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

        drawLinesFromPoint(baricenters, baricenter, frame);
    }

    return baricenter;
}

// 7mo que se llama.
// Calculo Promedio de la distancia entre los puntos del cierre convexo
float HandDetection::getPromediaDistancia(vector<Point> hull, double sumatoriaDistancia)
{
    return sumatoriaDistancia/(hull.size()-1);
}

// 8vo que se llama.
// calcula la varianza
double HandDetection::getVarianzaDist(vector<Point> hull, double distanciaEuclidian, float promediaDistancia)
{
    double sumatoriaVarianzaDistancia = 0;
    for( unsigned int i = 1; i < hull.size(); i++ ){
        sumatoriaVarianzaDistancia = sumatoriaVarianzaDistancia +
                (distanciaEuclidian-promediaDistancia) * (distanciaEuclidian-promediaDistancia);
    }

    return sumatoriaVarianzaDistancia;
}

// 9no que se llama.
// calcula la raiz de la varianza que es el desvio estandar
float HandDetection::getDesvioEstandar(double sumatoriaVarianzaDistancia, int size)
{
    return sqrt(sumatoriaVarianzaDistancia/size);
}

// llamado desde getBaricenter
// dibuja lineas partiendo desde el Point baricenter, y llegando a todos los puntos
// pasados en el vector de Points
void HandDetection::drawLinesFromPoint(vector<Point> baricenters, Point baricenter, Mat &frame)
{
    // Dibuja lineas grises desde el centro a los bordes
    for( unsigned int i = 0; i < baricenters.size(); i++ )
    {
        line( frame, baricenters.at( i ), baricenter, Scalar( 128, 128, 128 ), 1 );
    }
}

// porque calcula la distancia asi ? porque no le calcula la raiz cuadrada a todo eso ?
// supongo que para tener valores enteros y evitar variaciones si intenta calcular la raiz
// de un numero muy pequeño
double HandDetection::distance( Point a, Point b )
{
    return ( a.x - b.x ) * ( a.x - b.x ) + ( a.y - b.y ) * ( a.y - b.y );
}

// calculo distancia entre dos Points con raiz cuadrada
double HandDetection::distanceSqrt( Point a, Point b )
{
    return sqrt( ( a.x - b.x ) * ( a.x - b.x ) + ( a.y - b.y ) * ( a.y - b.y ) );
}


//-------------------------------METODOS de dibujo processFrames---------------------------------------

// 6to que se llama.
// Dibuja lineas que unen Points del vector hull
void HandDetection::drawLinesBetweenFingers(Mat &frame, vector<Point> hull, double &distanciaEuclidian, double &sumatoriaDistancia)
{
    for( unsigned int i = 1; i < hull.size(); i++ )
    {
        line( frame, hull.at( i ), hull.at( i - 1 ), Scalar( 128, 128, 128 ), 1 );

        distanciaEuclidian = cv::norm(hull.at( i ) - hull.at( i - 1 ));//Euclidian distance
        sumatoriaDistancia += distanciaEuclidian;
    }
}

// 11vo que se llama.
// Dibuja imagen miniatura binaria del frame actual,
//  poniendo en blanco los puntos de color en el rango de la piel
void HandDetection::drawLitleBinaryImage(Mat &frame, Mat &binaryCopy)
{
    // Mostramos miniatura
    Mat preview( binaryCopy.rows, binaryCopy.cols, CV_8UC3 );
    cvtColor( binaryCopy, preview, CV_GRAY2BGR );
    Mat previewResized( 96, 128, CV_8UC3 );
    cv::resize( preview, previewResized, previewResized.size(), 0, 0, INTER_CUBIC );
    previewResized.copyTo( frame( Rect( frame.cols - 135, frame.rows - 103, 128, 96 ) ) );
}


// deteccion de mano contando cant de pixeles con el color de la piel
/*
 * -----test eleccion umbral para saber si existe una mano-----
 *
 * MANO ABIERTA:
 * calcule el promedio de pixeles blancos cuando habia una mano en el frame
 * en buenas condiciones (se veia bien la mano en la imagen binaria)
 * luego de 2 pruebas de aprox 15 seg.
 * fui variando la distancia entre distancia media, corta y larga
 * resultado:
 * avgPx1 = 59860.1
 * avgPx2 = 58498.4
 *
 * unmbral que cuando se detecten 55000 px de color piel
 * hay UNA MANO ABIERTA detectada.
 *
 * MANO CERRADA:
 * mismo calculo. hay mas variacion, cuando esta cerca da valores muy altos
 * pero probando a una distancia "de trabajo" puede descender hasta valores
 * de 14000.
 * en avgPx1= 15229.5
 *
 * unmbral que cuando se detecten 12000 px de color piel
 * hay UNA MANO CERRADA detectada.
 *
 * conclusion: uso umbral de 12000 px.
 *
 * tal vez deberia limitar el maximo de pixeles e informar una anomalia en
 * la imagen.
 */
bool HandDetection::isHandInFrame(Mat &hsvFrame, int minHue, int maxHue)
{
    int sum = 0;
    for( int j = 0; j < hsvFrame.rows; j++ ){
        for( int i = 0; i < hsvFrame.cols; i++ ){
            Vec3b color = hsvFrame.at< Vec3b >( Point( i, j ) );

            if( !(color[1] >= minHue && color[1] <= maxHue) ){
                // suma la cantidad de px en el rango del color de la piel por frame
                sum++;
            }
        }
    }

    /* // calculo promedio
    if(sum > 5000){
        // cantidad de frames que analizo
        count++;
        sumatory+=sum;
        avgPx = sumatory/(double)count;
        qDebug() << "promedio pixels blancos por mano" << avgPx;
    }
    //*/

    if(sum >= 10000)  {
//        qDebug() << "Siiiiiii existe mano" << sum;

        if ( estado_hayMano_o_no == false )  {
            estado_hayMano_o_no = true;
            emit signal_cambioEstado_hayMano_o_no( true );
        }

        return true; // existe mano
    }
    else  {
//        qDebug() << "Nooooooo existe mano" << sum;

        if ( estado_hayMano_o_no == true )  {
            estado_hayMano_o_no = false;
            emit signal_cambioEstado_hayMano_o_no( false );
        }

        return false; // no existe mano
    }
}
