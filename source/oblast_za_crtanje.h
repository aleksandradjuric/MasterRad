#ifndef OBLAST_ZA_CRTANJE_H
#define OBLAST_ZA_CRTANJE_H

#include <QGraphicsItem>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QRectF>
#include <QPen>
#include <QGraphicsView>
#include <QCoreApplication>
#include <QEventLoop>
#include <QTime>
#include <QTimer>
#include <memory>

class Objekat_za_iscrtavanje : public QGraphicsItem
{
public:
    enum Oblik { krug, kvadrat, elipsa, ispunjeni_kvadrat };
    Objekat_za_iscrtavanje(int x, int y, QColor _boja, int _velicina, Oblik _oblik, std::string _naziv);
    ~Objekat_za_iscrtavanje();
    QPoint Point () const;
    int uzmi_x_tacku_pozicije();
    int uzmi_y_tacku_pozicije();
    int uzmi_velicinu();
    QPoint uzmi_poziciju();
    void promeni_poziciju(int x, int y);
    void postavi_poziciju(QPoint nova_pozicija);
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
private:
    QPoint pozicija;
    int velicina;
    QColor boja;
    Oblik oblik;
    std::string naziv;
};

class Putanja_za_iscrtavanje
{
public:
    Putanja_za_iscrtavanje(std::shared_ptr<Objekat_za_iscrtavanje> _prva_b_tacka, std::shared_ptr<Objekat_za_iscrtavanje> _druga_b_tacka, int _duzina, bool _direktna_putanja);
    ~Putanja_za_iscrtavanje();
    std::shared_ptr<QGraphicsLineItem> Linija();
    std::shared_ptr<QGraphicsTextItem> Duzina();
private:
    std::shared_ptr<QGraphicsLineItem> linija;
    std::shared_ptr<QGraphicsTextItem> duzina;
    bool direktna_putanja;
};

class Oblast_za_crtanje : public QGraphicsView
{
    Q_OBJECT

public:
    Oblast_za_crtanje(QWidget *parent);
    ~Oblast_za_crtanje();
    void dodaj_b_tacku(int x, int y, std::string naziv);
    void dodaj_s_tacku(int x, int y, std::string naziv);
    void dodaj_povrsinu(int x, int y, std::string naziv);
    void dodaj_blokiranje(std::shared_ptr<Objekat_za_iscrtavanje> druga_s_tacka, std::string naziv);
    std::shared_ptr<Objekat_za_iscrtavanje> uzmi_b_tacku_za_iscrtavanje(int indeks);
    std::shared_ptr<Objekat_za_iscrtavanje> uzmi_s_tacku_za_iscrtavanje(int indeks);
    void dodaj_putanju(std::shared_ptr<Objekat_za_iscrtavanje> prva_b_tacka, std::shared_ptr<Objekat_za_iscrtavanje> druga_b_tacka, int duzina, bool direktna_putanja);
    void dodaj_putanju_flojd_varsal(std::shared_ptr<Objekat_za_iscrtavanje> prva_b_tacka, std::shared_ptr<Objekat_za_iscrtavanje> druga_b_tacka, int duzina, bool direktna_putanja);
    void nacrtaj_scenu(int indeks_b_tacke_robota, std::vector<std::pair<int, bool>>& pozicije_predmeta);
    void azuriraj_stanje_scene(int indeks_b_tacke_robota, std::vector<std::pair<int, bool>>& pozicije_predmeta, bool animacija_u_toku);
    void nacrtaj_domen();
    void ocisti_scenu();
    void delay(int sekunde);
    void azuriraj_slajder(int i);
signals:
    void on_azuriraj_slajderSignal(int);
private:
    std::vector<std::shared_ptr<Objekat_za_iscrtavanje>> b_tacke_za_iscrtavanje;
    std::vector<std::shared_ptr<Objekat_za_iscrtavanje>> s_tacke_za_iscrtavanje;
    std::vector<std::shared_ptr<Objekat_za_iscrtavanje>> povrsine_za_iscrtavanje;
    std::vector<std::shared_ptr<Objekat_za_iscrtavanje>> blokiranja_za_iscrtavanje;
    std::vector<std::shared_ptr<Putanja_za_iscrtavanje>> putanje_za_iscrtavanje;
    std::vector<std::shared_ptr<Putanja_za_iscrtavanje>> putanje_flojd_varsal;
    std::vector<std::shared_ptr<Objekat_za_iscrtavanje>> predmeti_za_iscrtavanje;
    std::shared_ptr<Objekat_za_iscrtavanje> robot_za_iscrtavanje;
};

#endif // OBLAST_ZA_CRTANJE_H
