#ifndef SCENA_H
#define SCENA_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "oblast_za_crtanje.h"

using namespace std;

class Predmet
{
public:
    Predmet(int _pozicija, int _velicina, bool _ceka_obradu, bool _robot_drzi_predmet);

    int uzmi_poziciju();
    void promeni_poziciju(int indeks_s_cvora);
    int uzmi_velicinu();
    //velicina se ne menja za vreme izvrsavanja, dodaje se konstruktorom
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

class Scena
{
public:
    Scena();
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
    void nacrtaj_plan();
    vector<std::pair<int, bool> > uzmi_pozicije_predmeta(vector<Predmet> stanje_predmeta);
    void ocisti_scenu();

    Oblast_za_crtanje* oblast_za_crtanje;
private:
    vector<Predmet> predmeti;
    vector<std::pair<int, vector<Predmet>>> stanja_scene; //sluzi za iscrtavanje plana
    bool obrada_u_toku;
    int trenutna_pozicija_robota; //indeks b cvora u kome se robot nalazi  
};

#endif // SCENA_H
