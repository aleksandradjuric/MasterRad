#include "prikaz_plana.h"
#include "ui_prikaz_plana.h"

Prikaz_plana::Prikaz_plana(QWidget *parent) :
    QWidget(parent)
{
    ui = std::make_shared<Ui::Prikaz_plana>();
    ui->setupUi(this);
}

void Prikaz_plana::postavi_tekst_plana(QString tekst)
{
    ui->txt_plan->setText(tekst);
}

Prikaz_plana::~Prikaz_plana()
{
}
