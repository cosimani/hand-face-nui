#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <QWidget>
#include <QPalette>
#include <QEvent>

#include "graph/nodo.hpp"
#include "theme/colorizer.hpp"

#include "image.h"
#include "common.hpp"

namespace Ui
{
    class Block;
}

class Block : public QWidget
{
    Q_OBJECT

private:

    Ui::Block *ui;
    bool active;

    Nodo *associatedNode;

public:

    explicit Block( Nodo *associatedNode, QWidget *parent = NULL );
    ~Block();

    bool getActive() const;
    void setActive( bool value );

    void setStyle( bool active );

    Nodo *getAssociatedNode() const;
    void setAssociatedNode( Nodo *value );

    void simulateClick();

protected:

    bool event( QEvent *event );
    void mousePressEvent( QMouseEvent* );

signals:

    void clicked( Nodo* );
};

#endif // BLOCK_HPP

