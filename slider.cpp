#include "slider.h"
#include "ui_slider.h"
#include <QPainter>

Slider::Slider(QWidget *parent) : QWidget(parent),
                                  ui(new Ui::Slider)
{
    ui->setupUi(this);

    ui->qslider->setValue( 0 );
    ui->dial->setValue( 0 );
    this->setRango( 0, 100 );
    ui->label->setText( "" );

    this->setWindowOpacity( 0.75 );

    connect(ui->qslider, SIGNAL( valueChanged( int ) ), ui->dial, SLOT( setValue( int ) ) );
    connect(ui->dial, SIGNAL( valueChanged( int ) ), ui->qslider, SLOT( setValue( int ) ) );
}

Slider::~Slider()
{
    delete ui;
}

void Slider::setRango(int minimo, int maximo)
{
    ui->qslider->setRange( minimo, maximo );
    ui->dial->setRange( minimo, maximo );
}

int Slider::getValorActual() const
{
    return ui->qslider->value();
}

void Slider::setValorActual(int value)
{
    ui->qslider->setValue( value );
}

void Slider::setTexto(QString texto)
{
    ui->label->setText( texto );
}

void Slider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);


}
