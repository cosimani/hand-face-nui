#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#define INITIAL_NODE_ID "inicio"

#include <QWidget>
#include <QPalette>
#include <QList>
#include <QKeyEvent>
#include <QSound>

#include "graph/graph.hpp"
#include "theme/colorizer.hpp"
#include "interface/block.hpp"
#include "processor/camera.hpp"
#include "interface/camerawidget.h"

#include "common.hpp"

namespace Ui
{
    class Interface;
}

class Interface : public QWidget
{
    Q_OBJECT

private:

    Ui::Interface *ui;
    Graph *graph;
    Camera *camera;

    QTimer *blockSelectionTimer;
    bool blockSelection;

    void setStyle();
    void changeStyle();

    CameraWidget * cameraWidget;

public:

    explicit Interface( QWidget *parent = NULL );
    ~Interface();

    Graph *getGraph() const;
    void setGraph( Graph *value );

    Camera *getCamera() const;
    void setCamera( Camera *value );

    QTimer *getBlockSelectionTimer() const;
    void setBlockSelectionTimer( QTimer *value );

    bool getBlockSelection() const;
    void setBlockSelection( bool value );

    bool initInterface( QFile *file );

protected:

    void keyPressEvent( QKeyEvent *event );
    void resizeEvent( QResizeEvent * e );

private slots:

    void createAndSet( Nodo *node );
    void activateBlock( int index );
    void clickBlock( int index );
    void unblockSelection();
    void phraseReset();

    void slot_controlarSlider( int index );
    void slot_volverMenuInicio();
    void slot_laManoEstaAbierta( bool abierta );
    void slot_posicionFeature( QPoint target );

signals:
    /**
     * @brief signal_opcionFinalElegida Envia la posicion del boton en donde se hace clic. Esta pensado para
     * que el menu de opciones no sea anidado con mas opciones dentro de los botones. Hay que tener relacionado el index
     * que corresponde a cual opcion.
     * @param index
     */
    void signal_opcionFinalElegida( int index );

};

#endif // INTERFACE_HPP

