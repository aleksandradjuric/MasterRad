#include "oblast_za_crtanje.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QTime>
#include <QTimer>

//class Objekat_za_iscrtavanje
QPoint Objekat_za_iscrtavanje::Point () const
{
    return pozicija;
}

Objekat_za_iscrtavanje::Objekat_za_iscrtavanje(int x, int y, QColor _boja, int _velicina, Oblik _oblik, std::string _naziv)
{
    QPoint _centar = QPoint(x, y);
    pozicija = _centar;
    boja = _boja;
    velicina = _velicina;
    oblik = _oblik;
    naziv = _naziv;
}

Objekat_za_iscrtavanje::~Objekat_za_iscrtavanje()
{}


void Objekat_za_iscrtavanje::postavi_poziciju(QPoint nova_pozicija){
    pozicija = nova_pozicija;
}

void Objekat_za_iscrtavanje::promeni_poziciju(int x, int y)
{
    pozicija.setX(x);
    pozicija.setY(y);
}

int Objekat_za_iscrtavanje::uzmi_x_tacku_pozicije()
{
    return pozicija.x();
}

int Objekat_za_iscrtavanje::uzmi_velicinu()
{
    return velicina;
}

int Objekat_za_iscrtavanje::uzmi_y_tacku_pozicije()
{
    return pozicija.y();
}

QPoint Objekat_za_iscrtavanje::uzmi_poziciju()
{
    return pozicija;
}

QRectF Objekat_za_iscrtavanje::boundingRect() const
{
    return QRectF(5,5,595,595);
}

void Objekat_za_iscrtavanje::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush b;
    QPen p;
    b.setStyle(Qt::SolidPattern);
    b.setColor(Qt::white);
    p.setColor(boja);
    p.setWidth(3);

    painter->setBrush(b);
    painter->setPen(p);
    if(oblik == krug)
    {
        painter->setFont(QFont("times",velicina*0.3));
        painter->drawEllipse(pozicija.x() - velicina/2, pozicija.y() - velicina/2, velicina, velicina);
        painter->drawText(pozicija.x()-velicina*0.23, pozicija.y()+velicina*0.22, QString(naziv.c_str()));
    }
    else if(oblik == kvadrat)
    {
        painter->setFont(QFont("times",14));
        painter->drawRect(pozicija.x(), pozicija.y(), velicina*1.5, velicina);
        painter->drawText(pozicija.x(), pozicija.y() - 5, QString(naziv.c_str()));
    }
    else if(oblik == elipsa)
    {
        painter->setFont(QFont("times",velicina*0.3));
        painter->drawEllipse(pozicija.x() - velicina/2, pozicija.y() - velicina/2, velicina*1.5, velicina);
        painter->drawText(pozicija.x()-velicina*0.25, pozicija.y()+velicina*0.17, QString(naziv.c_str()));
    }

}

//class Putanje_za_iscrtavanje
Putanja_za_iscrtavanje::Putanja_za_iscrtavanje(Objekat_za_iscrtavanje* _prva_b_tacka, Objekat_za_iscrtavanje* _druga_b_tacka, int _duzina, bool _direktna_putanja)
{
    linija = new QGraphicsLineItem();
    if(_direktna_putanja)
        linija->setPen(QPen(Qt::black, 2, Qt::SolidLine));
    else
        linija->setPen(QPen(Qt::gray, 2, Qt::SolidLine));
    linija->setLine(_prva_b_tacka->uzmi_x_tacku_pozicije(), _prva_b_tacka->uzmi_y_tacku_pozicije(),
                    _druga_b_tacka->uzmi_x_tacku_pozicije(), _druga_b_tacka->uzmi_y_tacku_pozicije());
    duzina = new QGraphicsTextItem();
    duzina->setPlainText(QString::number(_duzina));

    int x_pozicija_teksta, y_pozicija_teksta, velicina;
    int prva_b_tacka_x, prva_b_tacka_y, druga_b_tacka_x, druga_b_tacka_y;

    velicina = _prva_b_tacka->uzmi_velicinu();
    prva_b_tacka_x = _prva_b_tacka->uzmi_x_tacku_pozicije();
    prva_b_tacka_y = _prva_b_tacka->uzmi_y_tacku_pozicije();
    druga_b_tacka_x = _druga_b_tacka->uzmi_x_tacku_pozicije();
    druga_b_tacka_y = _druga_b_tacka->uzmi_y_tacku_pozicije();

    x_pozicija_teksta = (prva_b_tacka_x > druga_b_tacka_x) ?
                        prva_b_tacka_x - (prva_b_tacka_x - druga_b_tacka_x)/2 :
                        druga_b_tacka_x - (druga_b_tacka_x - prva_b_tacka_x)/2;
    y_pozicija_teksta = (prva_b_tacka_y > druga_b_tacka_y) ?
                        prva_b_tacka_y - (prva_b_tacka_y - druga_b_tacka_y)/2 :
                        druga_b_tacka_y - (druga_b_tacka_y - prva_b_tacka_y)/2;

    duzina->setPos(x_pozicija_teksta, y_pozicija_teksta);
    duzina->setFont(QFont("times",14));
}

QGraphicsTextItem *Putanja_za_iscrtavanje::Duzina()
{
    return duzina;
}

QGraphicsLineItem* Putanja_za_iscrtavanje::Linija()
{
    return linija;
}

Putanja_za_iscrtavanje::~Putanja_za_iscrtavanje()
{}

//class Oblast_za_crtanje
void Oblast_za_crtanje::delay(int sekunde)
{
    QTime dieTime= QTime::currentTime().addSecs(sekunde);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

Oblast_za_crtanje::~Oblast_za_crtanje()
{
    unsigned i;
    unsigned duzina_vektora = b_tacke_za_iscrtavanje.size();
    for (i=0; i<duzina_vektora; i++)
    {
        Objekat_za_iscrtavanje* it = b_tacke_za_iscrtavanje[i];
        delete(it);
        it = NULL;
    }

    duzina_vektora = s_tacke_za_iscrtavanje.size();
    for (i=0; i<duzina_vektora; i++)
    {
        Objekat_za_iscrtavanje* it = s_tacke_za_iscrtavanje[i];
        delete(it);
        it = NULL;
    }

    duzina_vektora = povrsine_za_iscrtavanje.size();
    for (i=0; i<duzina_vektora; i++)
    {
        Objekat_za_iscrtavanje* it = povrsine_za_iscrtavanje[i];
        delete(it);
        it = NULL;
    }

    duzina_vektora = blokiranja_za_iscrtavanje.size();
    for (i=0; i<duzina_vektora; i++)
    {
        Objekat_za_iscrtavanje* it = blokiranja_za_iscrtavanje[i];
        delete(it);
        it = NULL;
    }

    duzina_vektora = putanje_za_iscrtavanje.size();
    for (i=0; i<duzina_vektora; i++)
    {
        Putanja_za_iscrtavanje* it = putanje_za_iscrtavanje[i];
        delete(it);
        it = NULL;
    }

    duzina_vektora = putanje_flojd_varsal.size();
    for (i=0; i<duzina_vektora; i++)
    {
        Putanja_za_iscrtavanje* it = putanje_flojd_varsal[i];
        delete(it);
        it = NULL;
    }

    duzina_vektora = predmeti_za_iscrtavanje.size();
    for (i=0; i<duzina_vektora; i++)
    {
        Objekat_za_iscrtavanje* it = predmeti_za_iscrtavanje[i];
        delete(it);
        it = NULL;
    }

    ocisti_scenu();
}

Oblast_za_crtanje::Oblast_za_crtanje(QWidget *parent)
    :QGraphicsView(parent)
{
    QGraphicsScene* qgraphics_scene = new QGraphicsScene(this);
    qgraphics_scene->setSceneRect(0, 0, 600, 600);
    setScene(qgraphics_scene);
    setMinimumSize(610, 610);
}

void Oblast_za_crtanje::nacrtaj_scenu(int indeks_b_tacke_robota, std::vector<std::pair<int, bool> > pozicije_predmeta)
{
    QPoint pozicija_b_tacke_robota = QPoint(b_tacke_za_iscrtavanje[indeks_b_tacke_robota]->uzmi_poziciju());
    robot_za_iscrtavanje = new Objekat_za_iscrtavanje(pozicija_b_tacke_robota.x()-30, pozicija_b_tacke_robota.y()-45, Qt::red, 40, Objekat_za_iscrtavanje::Oblik::kvadrat, "robot");
    scene()->addItem(robot_za_iscrtavanje);
    for(unsigned i=0; i<pozicije_predmeta.size(); i++){
        if(pozicije_predmeta[i].second)
            predmeti_za_iscrtavanje.push_back(new Objekat_za_iscrtavanje(pozicija_b_tacke_robota.x()-3, pozicija_b_tacke_robota.y()+25, Qt::black, 30, Objekat_za_iscrtavanje::Oblik::elipsa, "pred" + std::to_string(i)));
        else
        {
            QPoint pozicija_s_tacke_predmeta = s_tacke_za_iscrtavanje[pozicije_predmeta[i].first]->uzmi_poziciju();
            predmeti_za_iscrtavanje.push_back(new Objekat_za_iscrtavanje(pozicija_s_tacke_predmeta.x()-7, pozicija_s_tacke_predmeta.y()+25, Qt::black, 30, Objekat_za_iscrtavanje::Oblik::elipsa, "pred" + std::to_string(i)));
        }
        scene()->addItem(predmeti_za_iscrtavanje[i]);
    }
    scene()->update();
}

void Oblast_za_crtanje::azuriraj_stanje_scene(int indeks_b_tacke_robota, std::vector<std::pair<int, bool> > pozicije_predmeta)
{
    Oblast_za_crtanje::delay(1);
    QPoint pozicija_b_tacke_robota = b_tacke_za_iscrtavanje[indeks_b_tacke_robota]->uzmi_poziciju();
    robot_za_iscrtavanje->promeni_poziciju(pozicija_b_tacke_robota.x()-30, pozicija_b_tacke_robota.y()-45);
    for(unsigned i=0; i<pozicije_predmeta.size(); i++){
        if(pozicije_predmeta[i].second)
            predmeti_za_iscrtavanje[i]->promeni_poziciju(pozicija_b_tacke_robota.x()-7, pozicija_b_tacke_robota.y()-25);
        else
        {
            QPoint s_tacka_predmeta = s_tacke_za_iscrtavanje[pozicije_predmeta[i].first]->uzmi_poziciju();
            predmeti_za_iscrtavanje[i]->promeni_poziciju(s_tacka_predmeta.x()-7, s_tacka_predmeta.y()+25);
        }
    }
    scene()->update();
}

void Oblast_za_crtanje::nacrtaj_domen()
{
    unsigned i;
    //iscrtavanje povrsina
    for(i=0; i<povrsine_za_iscrtavanje.size(); i++)
        scene()->addItem(povrsine_za_iscrtavanje[i]);
    //iscrtavanje inicijalnih putanja
    for(i=0; i<putanje_za_iscrtavanje.size(); i++)
        scene()->addItem(putanje_za_iscrtavanje[i]->Linija());
    //iscrtavanje b tacaka
    for(i=0; i<b_tacke_za_iscrtavanje.size(); i++)
        scene()->addItem(b_tacke_za_iscrtavanje[i]);
    //iscrtavanje s tacaka
    for(i=0; i<s_tacke_za_iscrtavanje.size(); i++)
        scene()->addItem(s_tacke_za_iscrtavanje[i]);
    for(i=0; i<putanje_za_iscrtavanje.size(); i++)
        scene()->addItem(putanje_za_iscrtavanje[i]->Duzina());
    for(i=0; i<blokiranja_za_iscrtavanje.size(); i++)
        scene()->addItem(blokiranja_za_iscrtavanje[i]);
    scene()->update();

    Oblast_za_crtanje::delay(2);

    //iscrtavanje povrsina
    for(i=0; i<povrsine_za_iscrtavanje.size(); i++)
        scene()->removeItem(povrsine_za_iscrtavanje[i]);
    //iscrtavanje inicijalnih putanja
    for(i=0; i<putanje_za_iscrtavanje.size(); i++)
        scene()->removeItem(putanje_za_iscrtavanje[i]->Linija());
    //iscrtavanje b tacaka
    for(i=0; i<b_tacke_za_iscrtavanje.size(); i++)
        scene()->removeItem(b_tacke_za_iscrtavanje[i]);
    //iscrtavanje s tacaka
    for(i=0; i<s_tacke_za_iscrtavanje.size(); i++)
        scene()->removeItem(s_tacke_za_iscrtavanje[i]);
    for(i=0; i<putanje_za_iscrtavanje.size(); i++)
        scene()->removeItem(putanje_za_iscrtavanje[i]->Duzina());
    for(i=0; i<blokiranja_za_iscrtavanje.size(); i++)
        scene()->removeItem(blokiranja_za_iscrtavanje[i]);
    scene()->update();

    //iscrtavanje povrsina
    for(i=0; i<povrsine_za_iscrtavanje.size(); i++)
        scene()->addItem(povrsine_za_iscrtavanje[i]);
    //iscrtavanje inicijalnih putanja
    for(i=0; i<putanje_flojd_varsal.size(); i++){
        scene()->addItem(putanje_flojd_varsal[i]->Linija());
    }
    //iscrtavanje b tacaka
    for(i=0; i<b_tacke_za_iscrtavanje.size(); i++)
        scene()->addItem(b_tacke_za_iscrtavanje[i]);
    //iscrtavanje s tacaka
    for(i=0; i<s_tacke_za_iscrtavanje.size(); i++)
        scene()->addItem(s_tacke_za_iscrtavanje[i]);
    for(i=0; i<putanje_flojd_varsal.size(); i++)
        scene()->addItem(putanje_flojd_varsal[i]->Duzina());
    for(i=0; i<blokiranja_za_iscrtavanje.size(); i++)
        scene()->addItem(blokiranja_za_iscrtavanje[i]);
    scene()->update();
}

void Oblast_za_crtanje::dodaj_b_tacku(int x, int y, std::string naziv)
{
    b_tacke_za_iscrtavanje.push_back(new Objekat_za_iscrtavanje(x, y, Qt::blue, 80, Objekat_za_iscrtavanje::Oblik::krug, naziv));
}

void Oblast_za_crtanje::dodaj_s_tacku(int x, int y, std::string naziv)
{
    s_tacke_za_iscrtavanje.push_back(new Objekat_za_iscrtavanje(x, y, Qt::green, 40, Objekat_za_iscrtavanje::Oblik::krug, naziv));
}

void Oblast_za_crtanje::dodaj_povrsinu(int x, int y, std::string naziv)
{
    povrsine_za_iscrtavanje.push_back(new Objekat_za_iscrtavanje(x, y, Qt::red, 200, Objekat_za_iscrtavanje::Oblik::kvadrat, naziv));
}

Objekat_za_iscrtavanje* Oblast_za_crtanje::uzmi_b_tacku_za_iscrtavanje(int indeks)
{
    return b_tacke_za_iscrtavanje[indeks];
}

Objekat_za_iscrtavanje* Oblast_za_crtanje::uzmi_s_tacku_za_iscrtavanje(int indeks)
{
    return s_tacke_za_iscrtavanje[indeks];
}

void Oblast_za_crtanje::dodaj_putanju(Objekat_za_iscrtavanje* prva_b_tacka, Objekat_za_iscrtavanje* druga_b_tacka, int duzina, bool direktna_putanja)
{
    putanje_za_iscrtavanje.push_back(new Putanja_za_iscrtavanje(prva_b_tacka, druga_b_tacka, duzina, direktna_putanja));
}

void Oblast_za_crtanje::dodaj_putanju_flojd_varsal(Objekat_za_iscrtavanje* prva_b_tacka, Objekat_za_iscrtavanje* druga_b_tacka, int duzina, bool direktna_putanja)
{
    putanje_flojd_varsal.push_back(new Putanja_za_iscrtavanje(prva_b_tacka, druga_b_tacka, duzina, direktna_putanja));
}

void Oblast_za_crtanje::dodaj_blokiranje(Objekat_za_iscrtavanje* druga_s_tacka, std::string naziv)
{
    blokiranja_za_iscrtavanje.push_back(new Objekat_za_iscrtavanje(druga_s_tacka->uzmi_x_tacku_pozicije()-10,
                                                                   druga_s_tacka->uzmi_y_tacku_pozicije()-20,
                                                                   Qt::red, 25, Objekat_za_iscrtavanje::Oblik::krug, naziv));
}

void Oblast_za_crtanje::ocisti_scenu()
{
    b_tacke_za_iscrtavanje.clear();
    s_tacke_za_iscrtavanje.clear();
    povrsine_za_iscrtavanje.clear();
    blokiranja_za_iscrtavanje.clear();
    putanje_za_iscrtavanje.clear();
    putanje_flojd_varsal.clear();
    predmeti_za_iscrtavanje.clear();

    scene()->clear();
    scene()->update();
}

