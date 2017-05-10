#include "theme.hpp"

void Theme::init( QColor backgroundColor,
                  QColor backColor,
                  QColor highlightColor )
{
    this->setBackgroundColor( backgroundColor );
    this->setBackColor( backColor );
    this->setHighlightColor( highlightColor );
}

Theme::Theme( QColor backgroundColor,
              QColor backColor,
              QColor highlightColor,
              QObject *parent ) :
    QObject( parent )
{
    this->init( backgroundColor,
                backColor,
                highlightColor );
}

Theme::~Theme()
{

}

QColor Theme::getBackgroundColor() const
{
    return backgroundColor;
}

void Theme::setBackgroundColor( const QColor &value )
{
    backgroundColor = value;
}

QColor Theme::getBackColor() const
{
    return backColor;
}

void Theme::setBackColor( const QColor &value )
{
    backColor = value;
}

QColor Theme::getHighlightColor() const
{
    return highlightColor;
}

void Theme::setHighlightColor( const QColor &value )
{
    highlightColor = value;
}
