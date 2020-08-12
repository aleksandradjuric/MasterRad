#include "pomoc.h"
#include "ui_pomoc.h"

Pomoc::Pomoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Pomoc)
{
    ui->setupUi(this);
}

Pomoc::~Pomoc()
{
    delete ui;
}
