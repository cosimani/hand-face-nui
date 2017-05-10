#include "colorizer.hpp"

Colorizer *Colorizer::instance = NULL;

Colorizer::Colorizer( QObject *parent ) :
    QObject( parent ),
    themes( new QVector< Theme* >() )
{
    this->initThemes();
}

Colorizer *Colorizer::getInstance()
{
    if( !instance )
    {
        instance = new Colorizer();
    }
    return instance;
}

Colorizer::~Colorizer()
{
    if( this->getThemes() )
    {
        for( int i = 0; i < this->getThemes()->size(); i++ )
        {
            delete this->getThemes()->at( i );
        }
    }
    delete this->getThemes();
    delete this->getInstance();
}

QVector< Theme* > *Colorizer::getThemes() const
{
    return themes;
}

void Colorizer::setThemes( QVector< Theme* > *value )
{
    themes = value;
}

Theme *Colorizer::getTheme()
{
    return this->getThemes()->at( this->getThemeIndex() );
}

int Colorizer::getThemeIndex() const
{
    return themeIndex;
}

void Colorizer::setThemeIndex( int value )
{
    themeIndex = value;
}

Theme *Colorizer::getNextTheme()
{
    this->setThemeIndex( this->getThemeIndex() == this->getThemes()->size() - 1 ? 0 : this->getThemeIndex() + 1 );

    return this->getTheme();
}

Theme *Colorizer::getPreviousTheme()
{
    this->setThemeIndex( this->getThemeIndex() == 0 ? this->getThemes()->size() - 1 : this->getThemeIndex() - 1 );

    return this->getTheme();
}

void Colorizer::initThemes()
{
    int backgroundLevel = 25;
    int backLevel       = 50;
    int highLightLevel  = 100;

    for( int r = 1; r >= 0; r-- )
    {
        for( int g = 1; g >= 0; g-- )
        {
            for( int b = 1; b >= 0; b-- )
            {
                if( r || g || b )
                {
                    Theme *theme = new Theme( QColor( r ? backgroundLevel : 255, g ? backgroundLevel : 255, b ? backgroundLevel : 255 ),
                                              QColor( r ? backLevel       : 255, g ? backLevel       : 255, b ? backLevel       : 255 ),
                                              QColor( r ? highLightLevel  : 255, g ? highLightLevel  : 255, b ? highLightLevel  : 255 ) );

                    this->getThemes()->append( theme );
                }
            }
        }
    }

    this->setThemeIndex( this->getThemes()->size() ? 0 : -1 );
}
