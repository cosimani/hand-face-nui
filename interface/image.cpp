#include "image.hpp"

Image::Image( QWidget *parent ) :
    QWidget( parent )
{

}

void Image::setImage( QString image, int fillPolicy )
{
    this->image.load( image );
    if( !this->image.isNull() )
    {
        this->repaint();
    }
    this->policy = fillPolicy;
}

void Image::paintEvent( QPaintEvent* )
{
    QPainter painter( this );

    if( !image.isNull() )
    {
        bool aspectDifference = image.width() / ( float )image.height() < this->width() / ( float )this->height();

        int newWidth = aspectDifference == policy ? this->width() : image.width() * this->height() / ( float )image.height();
        int newHeight = aspectDifference == policy ? image.height() * this->width() / ( float )image.width() : this->height();

        QImage resized = image.scaled( newWidth, newHeight );

        if( !resized.isNull() )
        {
            painter.drawImage( this->width() / ( float )2 - resized.width() / ( float )2,
                               this->height() / ( float )2 - resized.height() / ( float )2,
                               resized );
        }
    }
}

void Image::resizeEvent( QResizeEvent* )
{
    this->repaint();
}
