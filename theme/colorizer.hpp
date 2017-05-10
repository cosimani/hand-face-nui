#ifndef COLORIZER_HPP
#define COLORIZER_HPP

#include <QObject>
#include <QVector>

#include "theme.hpp"

class Colorizer : public QObject
{
    Q_OBJECT

private:

    int themeIndex;

    int getThemeIndex() const;
    void setThemeIndex( int value );

    QVector< Theme* > *themes;
    void initThemes();

    Colorizer( QObject *parent = NULL );
    static Colorizer *instance;

public:

    static Colorizer *getInstance();
    ~Colorizer();

    QVector< Theme* > *getThemes() const;
    void setThemes( QVector< Theme* > *value );

    Theme *getTheme();
    Theme *getNextTheme();
    Theme *getPreviousTheme();
};

#endif // COLORIZER_HPP

