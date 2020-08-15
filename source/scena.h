#ifndef SCENA_H
#define SCENA_H

#include "oblast_za_crtanje.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Predmet
{
public:
    Predmet(int _pozicija, int _velicina, bool _ceka_obradu, bool _robot_drzi_predmet);
    int uzmi_poziciju();
    void promeni_poziciju(int indeks_s_cvora);
    int uzmi_velicinu();
    bool da_li_ceka_obradu();
    void promeni_status_obrade(bool status);
    bool da_li_robot_drzi_predmet();
    void uhvati_predmet();
    void spusti_predmet();
private:
    int pozicija; //indeks s tacke u kojoj je predmet smesten
    int velicina;
    bool ceka_obradu;
    bool robot_drzi_predmet;
};

class Scena : public QObject
{
    Q_OBJECT

public:
    explicit Scena(QObject* parent = 0);
    ~Scena();
    bool da_li_je_obrada_u_toku();
    void zapocni_obradu();
    void zavrsi_obradu();
    int uzmi_trenutnu_poziciju_robota();
    void promeni_trenutnu_poziciju_robota(int indeks_b_cvora);
    Predmet& uzmi_predmet(int indeks_predmeta);
    int uzmi_duzinu_niza_predmeta();
    void inicijalizuj_scenu(string putanja_do_fajla);
    void dodaj_stanje_scene(int pozicija);
    void nacrtaj_plan(int trenutni_frejm);
    void nacrtaj_stanje_scene(int trenutni_frejm);
    vector<std::pair<int, bool> > uzmi_pozicije_predmeta(vector<Predmet> stanje_predmeta);
    void ocisti_scenu();
    int uzmi_duzinu_stanja_scene();
    void promeni_stanje_animacije(bool stanje);
    Oblast_za_crtanje* oblast_za_crtanje;
private:
    vector<Predmet> predmeti;
    vector<std::pair<int, vector<Predmet>>> stanja_scene; //sluzi za iscrtavanje plana
    bool obrada_u_toku;
    int trenutna_pozicija_robota; //indeks b cvora u kome se robot nalazi  
    bool animacija_u_toku;
signals:
    void on_azuriraj_slajderSignal(int);
};

#endif // SCENA_H
