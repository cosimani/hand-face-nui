#include "interface.hpp"
#include "ui_interface.h"

#include <QDebug>

Interface::Interface( QWidget *parent ) : QWidget( parent ),
                                          ui( new Ui::Interface ),
                                          cameraWidget( new CameraWidget ),
                                          adb( new AdminDB( this ) )
{
    this->setGraph( new Graph( this ) );
    this->setCamera( new Camera( this ) );
    this->setBlockSelectionTimer( new QTimer( this ) );
    this->setBlockSelection( false );

    ui->setupUi( this );
    ui->scrollArea->setFixedHeight( SCROLL_AREA_HEIGHT );
    this->setStyle();

    connect( this->getCamera(),
             SIGNAL( positionDetected( int ) ),
             SLOT( activateBlock( int ) ) );

    connect( this->getCamera(),
             SIGNAL( selectionDetected( int ) ),
             SLOT( clickBlock( int ) ) );

    connect( blockSelectionTimer,
             SIGNAL( timeout() ),
             SLOT( unblockSelection() ) );

    connect( camera, SIGNAL( signal_newCameraFrame(cv::Mat*) ),
             cameraWidget, SLOT( slot_setCameraTexture(cv::Mat*) ) );

    connect( camera, SIGNAL( signal_nonFaceDetected() ),
             this, SLOT( phraseReset() ) );

    connect( camera, SIGNAL( signal_sonrisa() ),
             this, SLOT( slot_volverMenuInicio() ) );

    connect( this, SIGNAL( signal_opcionFinalElegida( int ) ),
             this, SLOT( slot_controlarSlider( int ) ) );

    connect( this->getCamera(), SIGNAL( signal_cursorTracking( QPoint ) ),
             this, SLOT( slot_posicionFeature( QPoint ) ) );

    connect(ui->imgHand, SIGNAL(signal_clicked(bool)),
            this, SLOT(slot_laManoEstaAbierta(bool)));


    /// Configuracion de los sliders

    ui->slider1->setEnabled( false );
    ui->slider2->setEnabled( false );
    ui->slider3->setEnabled( false );
    ui->slider4->setEnabled( false );

    ui->slider1->setTexto( "Luz roja" );
    ui->slider2->setTexto( "Luz ambiente" );
    ui->slider3->setTexto( "Luz de entrada" );
    ui->slider4->setTexto( "Luz azul" );

//    ui->cameraWidget->setVisible( false );

    // -----------------------------------jr------------------------------------------

    ///jr: arranca la imagen de la mano abierta
    /// pero no se muestra hasta que no se haya seleccionado la opcion
    //  qDebug() << "WIDTH ELEGIDA:" << this->width()/5; ///jr: 111 en mi maquina
    ui->imgHand->setMaximumWidth(this->width()/5);
    ui->imgHand->setVisible(false);
    QString iHand( DATA_PATH );
    iHand.append( ICONS_PATH );
    iHand.append( "open_hand.png" );
    qDebug() << iHand.isEmpty();
    ui->imgHand->setImage( iHand, ADJUST );

    // creo la base de datos y la tabla si no exixten
    QString dbname(DATABASE_PATH);
    dbname.append(DATABASE_NAME);
    getAdb()->conectar(dbname);
    // creo base de datos si no estaba creada y creo tabla si no lo estaba
    initDB();
}


void Interface::initDB()
{
    /*create tabla (funciona)
CREATE TABLE IF NOT EXISTS estados(id INTEGER PRIMARY KEY ASC,
                        luz_roja tinyint not null,
                        luz_ambiente tinyint not null,
                        uz_entrada tinyint not null,
                        luz_azul tinyint not null);
    */
    QString tableEstados("CREATE TABLE IF NOT EXISTS estados(id INTEGER PRIMARY KEY ASC,"
                         "luz_roja tinyint not null,"
                         "luz_ambiente tinyint not null,"
                         "luz_entrada tinyint not null,"
                         "luz_azul tinyint not null"
                         ");"
                    );

    QSqlQuery query = getAdb()->getDB().exec(tableEstados);

    bool tablaNoExiste = query.isValid();
    if(tablaNoExiste){
        qDebug() << "query valid";
        // no habia registros en la base de datos
        QString s1 = QString::number(ui->slider1->getValorActual());
        QString s2 = QString::number(ui->slider2->getValorActual());
        QString s3 = QString::number(ui->slider3->getValorActual());
        QString s4 = QString::number(ui->slider4->getValorActual());

        dbInsertInitialValues(s1, s2, s3, s4);
    } else {
        qDebug() << "query INvalid";
        // tengo que cargar los valres de la base de datos a los sliders
        query = getAdb()->getDB().exec("SELECT * FROM estados");
        query.next();
        QSqlRecord registro = query.record();
        QSqlField field;
        field = registro.field("luz_roja");
        QString s1 = field.value().toString();
        field = registro.field("luz_ambiente");
        QString s2 = field.value().toString();
        field = registro.field("luz_entrada");
        QString s3 = field.value().toString();
        field = registro.field("luz_azul");
        QString s4 = field.value().toString();

        qDebug() << "luz_roja" << s1;
        qDebug() << "luz_ambiente" << s2;
        qDebug() << "luz_entrada" << s3;
        qDebug() << "luz_azul" << s4;

        ui->slider1->setValorActual(s1.toInt());
        ui->slider2->setValorActual(s2.toInt());
        ui->slider3->setValorActual(s3.toInt());
        ui->slider4->setValorActual(s4.toInt());
    }
}

// inserta valores actuales de los insert, para obtener un registro inicia
void Interface::dbInsertInitialValues(QString s1, QString s2, QString s3, QString s4, int id){
    QString insert("INSERT INTO estados(id, luz_roja, luz_ambiente, luz_entrada, luz_azul)"
                   "VALUES("+ QString::number(id) + "," + s1 + "," + s2 + "," + s3 + "," + s4 +");");
    QSqlQuery query = getAdb()->getDB().exec(insert);
    qDebug() << "INITIAL INSERT:" << query.lastError();
}

// actualiza el registro con los valores de los sliders
void Interface::updateEstados(QString s1, QString s2, QString s3, QString s4)
{
    QSqlQuery query = getAdb()->getDB().exec("UPDATE estados SET "
                                             "luz_roja ="+ s1 +","
                                              "luz_ambiente =" + s2 + ","
                                              "luz_entrada =" + s3 + ","
                                              "luz_azul ="+ s4 +";");
//                                              "WHERE id =1;");  en esta linea tira unrecognized token:  /0WHERE/
    //si la consulta tiene errores lo veo llamando a lastError
    qDebug() << "UPDATE:" << query.lastError();
}

Interface::~Interface()
{
    delete camera;
}

void Interface::setStyle()
{
    this->setAutoFillBackground( true );

    QPalette palette( this->palette() );
    palette.setColor( QPalette::Background, Colorizer::getInstance()->getTheme()->getBackgroundColor() );
    palette.setColor( QPalette::WindowText, QColor( Qt::white ) );
    this->setPalette( palette );
}

void Interface::changeStyle()
{
     Colorizer::getInstance()->getNextTheme();

     QList< Block* > blocksList = this->findChildren< Block* >();
     for( int i = 0; i < blocksList.size(); i++ )
     {
         blocksList.at( i )->setStyle( false );
     }

     this->setStyle();
}

AdminDB *Interface::getAdb() const
{
    return adb;
}

void Interface::setAdb(AdminDB *value)
{
    adb = value;
}

Graph *Interface::getGraph() const
{
    return graph;
}

void Interface::setGraph( Graph *value )
{
    graph = value;
}

Camera *Interface::getCamera() const
{
    return camera;
}

void Interface::setCamera( Camera *value )
{
    camera = value;
}

QTimer *Interface::getBlockSelectionTimer() const
{
    return blockSelectionTimer;
}

void Interface::setBlockSelectionTimer( QTimer *value )
{
    blockSelectionTimer = value;
}

bool Interface::getBlockSelection() const
{
    return blockSelection;
}

void Interface::setBlockSelection( bool value )
{
    blockSelection = value;
}

bool Interface::initInterface( QFile *file )
{
    if( !this->getGraph()->initGraph( file ) )
    {
        // No se pudo iniciar el grafo

        return false;
    }

    Nodo *initialNode = this->getGraph()->get( INITIAL_NODE_ID );

    if( !initialNode )
    {
        // No se encontro el nodo inicial

        return false;
    }

    this->createAndSet( initialNode );

    return true;
}

void Interface::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() )
    {
    case Qt::Key_S:  // Cambia los colores de la interfaz
        this->changeStyle();
        break;

    case Qt::Key_I:  // Reinicia la frase
        this->phraseReset();
        break;

    case Qt::Key_C:  // Cambia de camara

        break;

    case Qt::Key_W:  // Mostrar el widget de la camara para ver que esta tomando
        if ( cameraWidget->isVisible() )  {
            cameraWidget->setParent(NULL);
//            cameraWidget->move( 0, this->height() );
            cameraWidget->hide();
        }
        else  {
//            cameraWidget->setWindowFlags( Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Window );
            cameraWidget->setWindowFlags( Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint );

            cameraWidget->resize( 640/4, 480/4 );
            cameraWidget->move( 10, this->height()-cameraWidget->height()-10 );

            // Le quitamos el focu para que no impida detectar las teclas
            cameraWidget->setFocusPolicy(Qt::NoFocus);

            cameraWidget->setParent(this);

            cameraWidget->show();
        }
        break;

    case Qt::Key_Escape:

        camera->getVideoCapture()->release();
        this->close();

        break;

    case Qt::Key_Space:  // Para calibrar. Igual a sonreir por primera vez, o pulsar el boton Calibrar
        this->getCamera()->calibrate();
        break;

    default:
        break;
    }
}

void Interface::resizeEvent(QResizeEvent *e)
{
//    cameraWidget->setWindowFlags( cameraWidget->windowFlags() | Qt::WindowStaysOnTopHint );
    cameraWidget->setWindowFlags( Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint );

    cameraWidget->resize( 640/4, 480/4 );
    cameraWidget->move( 10, e->size().height()-cameraWidget->height()-10 );

    // Le quitamos el focu para que no impida detectar las teclas
    cameraWidget->setFocusPolicy(Qt::NoFocus);

//    cameraWidget->show();
}

void Interface::createAndSet( Nodo *node )
{
    QList< Block* > blocksList = this->findChildren< Block* >();
    qDeleteAll( blocksList );

    this->ui->nodeTitleLabel->setText( node->getTitle() );

    if( node->getText() != "" )
    {
        QString sentence( ui->sentenceLabel->text() );
        sentence.append( " " );
        sentence.append( node->getText().toUpper() );
        this->ui->sentenceLabel->setText( sentence );
    }

    if( node->getChildren().size() == 1 &&
        node->getChildren().at( 0 ) == "none" )  // Aca arma la frase, aca se deberia abrir el QSlider
    {
        //this->getSpeaker()->read( this->ui->sentenceLabel->text() );

        return;
    }

    for( int i = 0; i < node->getChildren().size(); i++ )
    {
        Nodo *newNode = this->getGraph()->get( node->getChildren().at( i ) );

        if( newNode )
        {
            Block *block = new Block( newNode, this );
            this->ui->blocksLayout->addWidget( block );
            connect( block, SIGNAL( clicked( Nodo* ) ), SLOT( createAndSet( Nodo* ) ) );
        }
    }

    this->getCamera()->setSize( node->getChildren().size() );
}

/**
 * @brief Interface::activateBlock Ilumina el boton en la posicion index. Si es -1, desilumina todos.
 * @param index
 */
void Interface::activateBlock( int index )
{
    QList< Block* > blocksList = this->findChildren< Block* >();
    if( index >= 0 && index < blocksList.size() )
    {
        for( int i = 0; i < blocksList.size(); i++ )
        {
            blocksList.at( i )->setActive( false );
        }
        blocksList.at( index )->setActive( true );
    }
    else if ( index == -1 )  {

        ui->sentenceLabel->setText( "" );

        for( int i = 0; i < blocksList.size(); i++ )
        {
            blocksList.at( i )->setActive( false );
        }
    }

}

void Interface::clickBlock( int index )
{
    if( !this->getBlockSelection() )
    {
        QString sound( APPLICATION_PATH );
        sound.append( SOUNDS_PATH );
        sound.append( "selected.wav" );
        QSound::play( sound );

        QList< Block* > blocksList = this->findChildren< Block* >();
        if( index >= 0 && index < blocksList.size() )
        {
            blocksList.at( index )->simulateClick();

            emit signal_opcionFinalElegida( index );

        }

        this->setBlockSelection( true );
        blockSelectionTimer->start( 1000 );
    }
}

void Interface::unblockSelection()
{
    blockSelectionTimer->stop();
    this->setBlockSelection( false );
}

void Interface::phraseReset()
{
    this->ui->sentenceLabel->clear();
    this->createAndSet( this->getGraph()->get( INITIAL_NODE_ID ) );

    qDebug() << "phraseReset";
}

/**
  slot que se activa cuando se selecciona la imagen con la sonrisa.
*/
void Interface::slot_controlarSlider(int index)
{
    switch( index )  {
    case 0:
        ui->slider1->setEnabled( true );
        ui->sentenceLabel->setText( "Luz roja" );
        ui->imgHand->setVisible(true);
        break;
    case 1:
        ui->slider2->setEnabled( true );
        ui->sentenceLabel->setText( "Luz ambiente" );
        ui->imgHand->setVisible(true);
        break;
    case 2:
        ui->slider3->setEnabled( true );
        ui->sentenceLabel->setText( "Luz de entrada" );
        ui->imgHand->setVisible(true);
        break;
    case 3:
        ui->slider4->setEnabled( true );
        ui->sentenceLabel->setText( "Luz azul" );
        ui->imgHand->setVisible(true);
        break;
    default:
        ui->sentenceLabel->setText( "" );
    }

    this->getCamera()->setTipoDeteccion( Camera::OnlySmile );
}

void Interface::slot_volverMenuInicio()
{
    ui->slider1->setEnabled( false );
    ui->slider2->setEnabled( false );
    ui->slider3->setEnabled( false );
    ui->slider4->setEnabled( false );
    ui->sentenceLabel->setText( "" );

    ///jr: vuelvo invisible la imagen de la mano cuando sale de la seleccion
    ui->imgHand->setVisible(false);

    ui->sentenceLabel->clear();
    this->phraseReset();

    this->getCamera()->setTipoDeteccion( Camera::HaarCascades );
}

/**
 * @brief Interface::slot_laManoEstaAbierta Se invoca este slot cuando la deteccion de la mano cambia de estado.
 * Cuando la mano se extiende, se invoca esta slot con true. Si la mano se cierra, invoca con false.
 * @param abierta
 */
void Interface::slot_laManoEstaAbierta(bool abierta)
{
    QString hand(DATA_PATH);
    hand.append(ICONS_PATH);

    if ( abierta == true && this->getCamera()->getTipoDeteccionActual() == Camera::Features ){
        this->getCamera()->setTipoDeteccion( Camera::OnlySmile );

        hand.append("open_hand.png");
        ui->imgHand->setImage( hand, ADJUST);

        ///jr: TODO: aca guarda los valores de los sliders
        QString s1 = QString::number(ui->slider1->getValorActual());
        QString s2 = QString::number(ui->slider2->getValorActual());
        QString s3 = QString::number(ui->slider3->getValorActual());
        QString s4 = QString::number(ui->slider4->getValorActual());

        updateEstados(s1,s2,s3,s4);
    }
    else if ( abierta == false ){
        this->getCamera()->setIsNeedCalibrated( true );
        this->getCamera()->setTipoDeteccion( Camera::Features );
        hand.append("closed_hand.png");
        ui->imgHand->setImage(hand, ADJUST);
    }

    abierta ? qDebug() << "Mano abierta" : qDebug() << "Mano cerrada";
}

void Interface::slot_posicionFeature( QPoint target )
{
    int minSlider = 0;
    int maxSlider = 100;

    int minMouse = 310;
    int maxMouse = 345;

    float relacion = float( maxSlider - minSlider ) / float( maxMouse - minMouse );

    int valor_nuevo = float( target.x() - minMouse ) * relacion;

    if ( valor_nuevo <= 0 )  valor_nuevo = 0;
    if ( valor_nuevo >= 100 )  valor_nuevo = 100;

    valor_nuevo = 100 - valor_nuevo;

    qDebug() << QString::number(target.x() );

    if ( ui->slider1->isEnabled() )
        ui->slider1->setValorActual( valor_nuevo );

    if ( ui->slider2->isEnabled() )
        ui->slider2->setValorActual( valor_nuevo );

    if ( ui->slider3->isEnabled() )
        ui->slider3->setValorActual( valor_nuevo );

    if ( ui->slider4->isEnabled() )
        ui->slider4->setValorActual( valor_nuevo );

}


