#include "interface.hpp"
#include "ui_interface.h"

#include <QDebug>

Interface::Interface( QWidget *parent ) : QWidget( parent ),
                                          ui( new Ui::Interface ),
                                          cameraWidget( new CameraWidget )
{
    this->setGraph( new Graph( this ) );
    this->setCamera( new Camera( this ) );
    this->setBlockSelectionTimer( new QTimer( this ) );
    this->setBlockSelection( false );

    ui->setupUi( this );
    ui->scrollArea->setFixedHeight( SCROLL_AREA_HEIGHT );
    this->setStyle();

    connect( this->ui->calibrateButton,
             SIGNAL( pressed() ),
             this->getCamera(),
             SLOT( calibrate() ) );

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

    connect( ui->pbMano, SIGNAL( clicked( bool ) ),
             this, SLOT( slot_laManoEstaAbierta( bool ) ) );

    connect( this->getCamera(), SIGNAL( signal_cursorTracking( QPoint ) ),
             this, SLOT( slot_posicionFeature( QPoint ) ) );


//    ui->cameraWidget->setVisible( false );

}

Interface::~Interface()
{

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

    Node *initialNode = this->getGraph()->get( INITIAL_NODE_ID );

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

void Interface::createAndSet( Node *node )
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
        Node *newNode = this->getGraph()->get( node->getChildren().at( i ) );

        if( newNode )
        {
            Block *block = new Block( newNode, this );
            this->ui->blocksLayout->addWidget( block );
            connect( block, SIGNAL( clicked( Node* ) ), SLOT( createAndSet( Node* ) ) );
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

void Interface::slot_controlarSlider(int index)
{
    ui->slider->setEnabled( true );
    ui->sentenceLabel->setText( QString::number( index ) );

    this->getCamera()->setTipoDeteccion( Camera::OnlySmile );
}

void Interface::slot_volverMenuInicio()
{
    ui->slider->setEnabled( false );
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
    if ( abierta == true && this->getCamera()->getTipoDeteccionActual() == Camera::Features )
        this->getCamera()->setTipoDeteccion( Camera::OnlySmile );
    else if ( abierta == false )  {
        this->getCamera()->setIsNeedCalibrated( true );
        this->getCamera()->setTipoDeteccion( Camera::Features );
    }

    abierta ? qDebug() << "Mano abierta" : qDebug() << "Mano cerrada";
    abierta ? ui->lInfo->setText( "Mano abierta" ) :
              ui->lInfo->setText( "Mano cerrada" );

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

    ui->lPosicionX->setText( QString::number(target.x() ) );

    ui->slider->setValue( valor_nuevo );
}


