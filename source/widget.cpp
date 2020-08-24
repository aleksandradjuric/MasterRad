#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    ui = std::make_shared<Ui::Widget>();
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    std::shared_ptr<Oblast_za_crtanje> graphicsView(ui->graphicsView);
    da = graphicsView;
    domen = std::make_shared<Domen>();
    domen->oblast_za_crtanje = da;
    scena = std::make_shared<Scena>();
    scena->oblast_za_crtanje = da;
    connect(da.get(), SIGNAL(on_azuriraj_slajderSignal(int)), this, SLOT(on_azurirajSlajderTriggered(int)));
}

Widget::~Widget()
{
    disconnect(da.get(), SIGNAL(on_azuriraj_slajderSignal(int)), this, SLOT(on_azurirajSlajderTriggered(int)));
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
    ui->lbl_maksimalna_duzina_putanje->setDisabled(false);
    ui->sb_maksimalna_duzina_putanje->setDisabled(false);
}

void Widget::on_btn_pomoc_clicked()
{
    pomoc = std::make_shared<Pomoc>();
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
        prikaz_plana = std::make_shared<Prikaz_plana>();
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
    int maksimalna_duzinja_putanje = ui->sb_maksimalna_duzina_putanje->value();
    planiranje = std::make_shared<Planiranje>(domen, scena, maksimalna_duzinja_putanje);

    timer.start();
    bool sat = planiranje->generisi_plan();

    int duzina_niza_stanja_scene = 0;

    ui->lbl_rezultat->setText(ui->lbl_rezultat->text() + " " + (sat ? "SAT" : "UNSAT"));
    ui->lbl_vreme_izvrsavanja->setText(ui->lbl_vreme_izvrsavanja->text() + " " + QString::number(timer.elapsed()/1000.0) + "s");

    if (sat)
    {
        ui->btn_pocni_animaciju->setDisabled(false);
        duzina_niza_stanja_scene = scena->uzmi_duzinu_stanja_scene();
        ui->slider_animacija->setMaximum(duzina_niza_stanja_scene-1);
    }

    ui->btn_otvori_fajl->setDisabled(false);
    ui->btn_pocni_ponovo->setDisabled(false);
    ui->lbl_rezultat->setDisabled(false);
    ui->lbl_vreme_izvrsavanja->setDisabled(false);
    ui->lbl_maksimalna_duzina_putanje->setDisabled(true);
    ui->sb_maksimalna_duzina_putanje->setDisabled(true);
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
    ui->btn_pocni_animaciju->setDisabled(true);
    ui->lbl_maksimalna_duzina_putanje->setDisabled(true);
    ui->sb_maksimalna_duzina_putanje->setDisabled(true);
    ui->lbl_rezultat->setDisabled(true);
    ui->lbl_vreme_izvrsavanja->setDisabled(true);
    ui->btn_zaustavi_animaciju->setDisabled(true);
    ui->slider_animacija->setDisabled(true);
    ui->slider_animacija->setValue(0);
}

void Widget::on_btn_pocni_animaciju_clicked()
{
    ui->btn_zaustavi_animaciju->setDisabled(false);
    ui->btn_pocni_animaciju->setDisabled(true);
    ui->slider_animacija->setDisabled(true);
    ui->btn_pocni_ponovo->setDisabled(true);
    scena->promeni_stanje_animacije(true);
    int trenutni_frejm = ui->slider_animacija->value();
    scena->nacrtaj_plan(trenutni_frejm);
}

void Widget::on_btn_zaustavi_animaciju_clicked()
{
    ui->btn_zaustavi_animaciju->setDisabled(true);
    ui->btn_pocni_animaciju->setDisabled(false);
    ui->slider_animacija->setDisabled(false);
    ui->btn_pocni_ponovo->setDisabled(false);
    scena->promeni_stanje_animacije(false);
}

void Widget::on_azurirajSlajderTriggered(int vrednost)
{
    ui->slider_animacija->setValue(vrednost);
}

void Widget::on_slider_animacija_sliderMoved(int pozicija)
{
    if (ui->slider_animacija->isEnabled())
        scena->nacrtaj_stanje_scene(pozicija);
}
