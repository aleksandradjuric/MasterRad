#ifndef WIDGET_H
#define WIDGET_H

#include "domen.h"
#include "scena.h"
#include "oblast_za_crtanje.h"
#include "pomoc.h"
#include "planiranje.h"
#include "prikaz_plana.h"
#include <QWidget>
#include <QGraphicsView>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QCoreApplication>
#include <QMessageBox>
#include <QTextStream>
#include <QTime>
#include <memory>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
private:
    std::shared_ptr<Ui::Widget> ui;
    std::shared_ptr<Oblast_za_crtanje> da;
    std::shared_ptr<Domen> domen;
    std::shared_ptr<Scena> scena;
    std::shared_ptr<Pomoc> pomoc;
    std::shared_ptr<Prikaz_plana> prikaz_plana;
    std::shared_ptr<Planiranje> planiranje;
public slots:
    void on_azurirajSlajderTriggered(int);
private slots:
    void on_btn_ucitaj_domen_clicked();
    void on_btn_ucitaj_scenu_clicked();
    void on_btn_pomoc_clicked();
    void on_btn_otvori_fajl_clicked();
    void on_btn_generisi_plan_clicked();
    void on_btn_pocni_ponovo_clicked();
    void on_btn_pocni_animaciju_clicked();
    void on_btn_zaustavi_animaciju_clicked();
    void on_slider_animacija_sliderMoved(int pozicija);
};

#endif // WIDGET_H
