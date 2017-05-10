#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <QWidget>
#include <QPalette>
#include <QEvent>

#include "graph/node.hpp"
#include "theme/colorizer.hpp"

#include "image.hpp"

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

    Node *associatedNode;

public:

    explicit Block( Node *associatedNode, QWidget *parent = NULL );
    ~Block();

    bool getActive() const;
    void setActive( bool value );

    void setStyle( bool active );

    Node *getAssociatedNode() const;
    void setAssociatedNode( Node *value );

    void simulateClick();

protected:

    bool event( QEvent *event );
    void mousePressEvent( QMouseEvent* );

signals:

    void clicked( Node* );
};

#endif // BLOCK_HPP

