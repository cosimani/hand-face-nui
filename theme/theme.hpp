#ifndef THEME_HPP
#define THEME_HPP

#include <QObject>
#include <QColor>

class Theme : public QObject
{
    Q_OBJECT

private:

    void init( QColor backgroundColor,
               QColor backColor,
               QColor highlightColor );

    QColor backgroundColor;
    QColor backColor;
    QColor highlightColor;

public:

    explicit Theme( QColor backgroundColor,
                    QColor backColor,
                    QColor highlightColor,
                    QObject *parent = NULL );
    ~Theme();

    QColor getBackgroundColor() const;
    void setBackgroundColor( const QColor &value );

    QColor getBackColor() const;
    void setBackColor( const QColor &value );

    QColor getHighlightColor() const;
    void setHighlightColor( const QColor &value );
};

#endif // THEME_HPP

