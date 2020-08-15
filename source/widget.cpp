#include "widget.h"
#include "ui_widget.h"
#include "prikaz_plana.h"
#include <QFileDialog>
#include <QCoreApplication>
#include <QMessageBox>
#include <QTextStream>
#include <QTime>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    da = ui->graphicsView;
    domen = new Domen();
    domen->oblast_za_crtanje = da;
    scena = new Scena();
    scena->oblast_za_crtanje = da;
}

Widget::~Widget()
{
    delete ui;
    delete scena;
    delete domen;
    delete da;
}

void Widget::on_btn_ucitaj_domen_clicked()
{
    ui->btn_ucitaj_domen->setDisabled(true);
    QString filePath = QFileDialog::getOpenFileName(this, "Učitaj domen", "C://", "Text files (*.txt)");
    std::string file = filePath.toStdString().c_str();
    if(filePath == "")
    {
        ui->btn_ucitaj_domen->setDisabled(false);
        return;
    }
    domen->inicijalizuj_domen(file);
    ui->btn_ucitaj_scenu->setDisabled(false);
}

void Widget::on_btn_ucitaj_scenu_clicked()
{
    ui->btn_ucitaj_scenu->setDisabled(true);
    QString filePath = QFileDialog::getOpenFileName(this, "Učitaj scenu", "C://", "Text files (*.txt)");
    std::string file = filePath.toStdString().c_str();
    if(filePath == "")
    {
        ui->btn_ucitaj_scenu->setDisabled(false);
        return;
    }
    scena->inicijalizuj_scenu(file);
    ui->btn_generisi_plan->setDisabled(false);
}

void Widget::on_btn_pomoc_clicked()
{
    pomoc = new Pomoc();
    pomoc->setMinimumSize(900,650);
    pomoc->setMaximumSize(900,650);
    pomoc->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    pomoc->move(QApplication::desktop()->screen()->rect().center() - pomoc->rect().center());
    pomoc->setWindowTitle("Pomoć");
    pomoc->show();
}

void Widget::on_btn_otvori_fajl_clicked()
{
    QFile file("Plan.txt");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        Prikaz_plana* prikaz_plana = new Prikaz_plana();
        prikaz_plana->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        prikaz_plana->move(QApplication::desktop()->screen()->rect().center() - prikaz_plana->rect().center());
        prikaz_plana->postavi_tekst_plana(in.readAll());
        prikaz_plana->setWindowTitle("Prikaz plana");
        prikaz_plana->show();
    }
}

void Widget::on_btn_generisi_plan_clicked()
{
    QTime timer;
    ui->btn_generisi_plan->setDisabled(true);
    planiranje = new Planiranje(*domen, scena);

    timer.start();
    bool sat = planiranje->generisi_plan();

    ui->lbl_rezultat->setText(ui->lbl_rezultat->text() + " " + (sat ? "SAT" : "UNSAT"));
    ui->lbl_vreme_izvrsavanja->setText(ui->lbl_vreme_izvrsavanja->text() + " " + QString::number(timer.elapsed()/1000.0) + "s");

    if (sat)
        scena->nacrtaj_plan();

    ui->btn_otvori_fajl->setDisabled(false);
    ui->btn_pocni_ponovo->setDisabled(false);
}

void Widget::on_btn_pocni_ponovo_clicked()
{
    ui->lbl_rezultat->setText("Rezultat je:");
    ui->lbl_vreme_izvrsavanja->setText("Vreme planiranja je:");

    scena->ocisti_scenu();
    domen->ocisti_domen();
    ui->btn_ucitaj_domen->setDisabled(false);
    ui->btn_otvori_fajl->setDisabled(true);
    ui->btn_pocni_ponovo->setDisabled(true);
}
