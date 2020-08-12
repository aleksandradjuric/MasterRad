#ifndef DOMEN_H
#define DOMEN_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <QPoint>
#include <QGraphicsItem>
#include "oblast_za_crtanje.h"

#define INF 99999

using namespace std;


class Povrsina
{
public:
    Povrsina(string _naziv);

    string uzmi_ime_povrsine();
    //nece se menjati tokom izvrsavanja programa, inicijalizuje se konstruktorom
private:
    string naziv;
};

class B_tacka
{
public:
    B_tacka(int _indeks_povrsine);
    int uzmi_indeks_povrsine();
    //nece se menjati tokom izvrsavanja programa, inicijalizuje se konstruktorom
private:
    int indeks_povrsine;
};

class S_tacka
{
public:
    S_tacka(int _indeks_b_tacke);
    int uzmi_indeks_b_tacke();
    //nece se menjati tokom izvrsavanja programa, inicijalizuje se konstruktorom
private:
    int indeks_b_tacke;
};

class Putanja
{
public:
    Putanja(int _indeks_b_tacke_1, int _indeks_b_tacke_2, int _duzina);
    Putanja(int _indeks_b_tacke_1, int _indeks_b_tacke_2, int _duzina, vector<int> _sekvenca_b_tacki);
    int uzmi_indeks_prve_b_tacke();
    int uzmi_indeks_druge_b_tacke();
    int uzmi_duzinu();
    vector<int> uzmi_sekvencu_b_tacki();
    //informacije ce se dodati konstruktorom jednom tokom inicijalizacije, nakon primene Flojd-Varsalovog algoritma, nece se menjati
private:
    int indeks_b_tacke_1;
    int indeks_b_tacke_2;
    int duzina;
    vector<int> sekvenca_b_tacki; //putanja koju je Flojd-Varsal nasao kao najkracu za ovaj par tacaka
};

class Domen
{
public:
    Domen();
    ~Domen();

    int uzmi_duzinu_niza_povrsina();
    Povrsina uzmi_povrsinu(int indeks_povrsine);
    int uzmi_duzinu_niza_b_tacaka();
    B_tacka uzmi_b_tacku(int indeks_b_tacke);
    int uzmi_duzinu_niza_s_tacaka();
    S_tacka uzmi_s_tacku(int indeks_s_tacke);
    int uzmi_duzinu_niza_putanja();
    Putanja uzmi_putanju(int indeks_putanje);
    int uzmi_duzinu_niza_medjusobnih_blokiranja_s_tacaka();
    bool da_li_blokira(int indeks_s_tacke_1, int indeks_s_tacke_2);
    void inicijalizuj_domen(string putanja_do_fajla);
    vector<int> uzmi_sekvencu_tacaka_za_par_b_tacaka(int indeks_b_tacke_1, int indeks_b_tacke_2);
    void ocisti_domen();

    Oblast_za_crtanje* oblast_za_crtanje;

private:
    vector<Povrsina> povrsine;
    vector<B_tacka> b_tacke;
    vector<S_tacka> s_tacke;
    vector<Putanja> putanje;
    vector<vector<bool>> medjusobna_blokiranja_s_tacaka;
    //neophodno za primenu Flojd-Varsalovog algoritma
    vector<vector<int>> matrica_prelaza;  //pomocna matrica
    void ispisi_matricu(vector<vector<int>> matrica);
    void pronadji_sekvencu_tacaka(int i, int j, vector<int>& sekvenca_tacaka);
    void nadji_najkrace_putanje_izmedju_b_tacaka();
    void inicijalizuj_medjusobna_blokiranja();
};

#endif // DOMEN_H
