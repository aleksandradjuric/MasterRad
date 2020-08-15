#ifndef WIDGET_H
#define WIDGET_H

#include "domen.h"
#include "scena.h"
#include "oblast_za_crtanje.h"
#include "pomoc.h"
#include "planiranje.h"
#include <QWidget>
#include <QGraphicsView>
#include <QApplication>
#include <QDesktopWidget>

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
    Oblast_za_crtanje *da;
    Ui::Widget *ui;
    Domen *domen;
    Scena *scena;
    Pomoc *pomoc;
    Planiranje* planiranje;
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
