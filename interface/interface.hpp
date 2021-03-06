#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#define INITIAL_NODE_ID "inicio"

#include <QWidget>
#include <QPalette>
#include <QList>
#include <QKeyEvent>

#ifndef RASPBERRY

#include <QSound>

#endif

#include <QSqlError>
#include <QSqlField>

#include "graph/graph.hpp"
#include "theme/colorizer.hpp"
#include "interface/block.hpp"
#include "processor/camera.hpp"
#include "interface/camerawidget.h"
#include "database/admindb.h"

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

    // aqui almacenamos el primer valor de x cuando el usuario quiere controlar el slidar.
    // Este valor va a corresponder al valor 50 del slider (siendo su rango entre 0 y 100.
    int valorInicialFeature;

    void setStyle();
    void changeStyle();

#ifndef RASPBERRY

    CameraWidget * cameraWidget;

#endif

    AdminDB * adb;
    void initDB();
    void updateEstados(QString s1, QString s2, QString s3, QString s4);
    void dbInsertInitialValues(QString s1, QString s2, QString s3, QString s4, int id = 1);

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

    AdminDB *getAdb() const;
    void setAdb(AdminDB *value);

protected:

    void keyPressEvent( QKeyEvent *event );
    void resizeEvent(QResizeEvent *e);

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

    void slot_capturarFondo();

    void slot_cambioEstado_hayMano( bool hay );
    void slot_estado_manoAbierta_o_cerrada( bool abierta );

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

