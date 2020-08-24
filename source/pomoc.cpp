#include "pomoc.h"
#include "ui_pomoc.h"

Pomoc::Pomoc(QWidget *parent) :
    QWidget(parent)
{
    ui = std::make_shared<Ui::Pomoc>();
    ui->setupUi(this);
}

Pomoc::~Pomoc()
{}
