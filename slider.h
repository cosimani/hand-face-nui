#ifndef SLIDER_H
#define SLIDER_H

#include <QWidget>

namespace Ui {
class Slider;
}

class Slider : public QWidget
{
    Q_OBJECT

public:
    explicit Slider(QWidget *parent = 0);
    ~Slider();

    void setRango(int minimo, int maximo);

    int getValorActual() const;
    void setValorActual(int value);

    void setTexto( QString texto );

private:
    Ui::Slider *ui;

protected:
    void paintEvent(QPaintEvent *);

};

#endif // SLIDER_H
