#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QApplication>
#include <QDesktopWidget>
#include "domen.h"
#include "scena.h"
#include "oblast_za_crtanje.h"
#include "pomoc.h"
#include "planiranje.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_btn_ucitaj_domen_clicked();
    void on_btn_ucitaj_scenu_clicked();
    void on_btn_pomoc_clicked();
    void on_btn_otvori_fajl_clicked();

    void on_btn_generisi_plan_clicked();

    void on_btn_pocni_ponovo_clicked();

private:
    Oblast_za_crtanje *da;
    Ui::Widget *ui;
    Domen *domen;
    Scena *scena;
    Pomoc *pomoc;
    Planiranje* planiranje;
};

#endif // WIDGET_H
