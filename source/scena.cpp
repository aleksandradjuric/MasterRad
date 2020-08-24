#include "scena.h"

Scena::Scena()
{}

Scena::~Scena()
{}

bool Scena::da_li_je_obrada_u_toku()
{
    return obrada_u_toku;
}

void Scena::zapocni_obradu()
{
    obrada_u_toku = true;
}

void Scena::zavrsi_obradu()
{
    int i;
    int duzina_niza_predmeta = predmeti.size();
    for(i=0; i<duzina_niza_predmeta; i++)
        predmeti[i].promeni_status_obrade(false);
    obrada_u_toku = false;
}

int Scena::uzmi_trenutnu_poziciju_robota()
{
    return trenutna_pozicija_robota;
}

void Scena::promeni_trenutnu_poziciju_robota(int indeks_b_cvora)
{
    trenutna_pozicija_robota = indeks_b_cvora;
}

Predmet& Scena::uzmi_predmet(int indeks_predmeta)
{
    return predmeti[indeks_predmeta];
}

int Scena::uzmi_duzinu_niza_predmeta()
{
    return predmeti.size();
}

void Scena::inicijalizuj_scenu(string putanja_do_fajla)
{
    obrada_u_toku = false;

    // promenljive koje ce biti koriscene za ucitavanje scene
    int pocetna_pozicija_robota;
    int indeks_s_tacke, velicina_predmeta;
    string string_da_li_ceka_obradu, string_da_li_robot_drzi_predmet;
    bool da_li_ceka_obradu, da_li_robot_drzi_predmet;

    // indikator koji govori koja informacija se trenutno ucitava
    enum Trenutno_se_konfigurise { undef, pozicija_robota, predmet};
    Trenutno_se_konfigurise trenutno_se_konfigurise = undef;

    string linija;
    ifstream konfiguracioni_fajl_za_scenu (putanja_do_fajla);

    if (konfiguracioni_fajl_za_scenu.is_open())
    {
      while ( getline (konfiguracioni_fajl_za_scenu, linija) )
      {
          if(linija.length() > 0) //proveriti da li ima oznaku \r za kraj reda
              linija = linija.at(linija.length()-1) == '\r' ? linija.substr(0, linija.length()-1) : linija;
          if(linija != ""){
              stringstream linija_stream(linija);
              if (linija == "Pocetna pozicija robota"){
                  trenutno_se_konfigurise = pozicija_robota;
              }
              else if (linija == "Predmeti"){
                  trenutno_se_konfigurise = predmet;
              }
              else{
                  switch (trenutno_se_konfigurise) {
                  case pozicija_robota:
                      linija_stream >> pocetna_pozicija_robota;
                      trenutna_pozicija_robota = pocetna_pozicija_robota;
                      break;
                  case predmet:
                      linija_stream >> indeks_s_tacke >> velicina_predmeta >> string_da_li_ceka_obradu >> string_da_li_robot_drzi_predmet;
                      da_li_ceka_obradu = (string_da_li_ceka_obradu == "true");
                      da_li_robot_drzi_predmet = (string_da_li_robot_drzi_predmet == "true");
                      predmeti.push_back(Predmet(indeks_s_tacke, velicina_predmeta, da_li_ceka_obradu, da_li_robot_drzi_predmet));
                      break;
                  default:
                      break;
                  }
              }
          }
      }
      konfiguracioni_fajl_za_scenu.close();
      vector<std::pair<int, bool>> pozicije_predmeta = uzmi_pozicije_predmeta(predmeti);
      oblast_za_crtanje->nacrtaj_scenu(trenutna_pozicija_robota, pozicije_predmeta);
    }
    else std::cout << "Nije moguce otvoriti fajl" << std::endl;
}

void Scena::dodaj_stanje_scene(int pozicija)
{
    stanja_scene.push_back(make_pair(pozicija, predmeti));
}

vector<std::pair<int, bool>> Scena::uzmi_pozicije_predmeta(vector<Predmet>& stanje_predmeta)
{
    vector<std::pair<int, bool>> pozicije_predmeta;
    for(unsigned i=0; i<stanje_predmeta.size(); i++)
    {
        pozicije_predmeta.push_back(make_pair(stanje_predmeta[i].uzmi_poziciju(), stanje_predmeta[i].da_li_robot_drzi_predmet()));
    }
    return pozicije_predmeta;
}

void Scena::nacrtaj_plan(int trenutni_frejm)
{
    for (unsigned i=trenutni_frejm; i<stanja_scene.size(); i++)
    {
        if (!animacija_u_toku)
            break;
        nacrtaj_stanje_scene(i);
        oblast_za_crtanje->azuriraj_slajder(i);
    }
}

void Scena::nacrtaj_stanje_scene(int trenutni_frejm)
{
    vector<std::pair<int, bool>> pozicije_predmeta = uzmi_pozicije_predmeta(stanja_scene[trenutni_frejm].second);
    oblast_za_crtanje->azuriraj_stanje_scene(stanja_scene[trenutni_frejm].first, pozicije_predmeta, animacija_u_toku);
}

void Scena::ocisti_scenu()
{
    oblast_za_crtanje->ocisti_scenu();
    predmeti.clear();
    stanja_scene.clear();
}

int Scena::uzmi_duzinu_stanja_scene()
{
    return stanja_scene.size();
}

void Scena::promeni_stanje_animacije(bool stanje)
{
    animacija_u_toku = stanje;
}

//class Predmet
Predmet::Predmet(int _pozicija, int _velicina, bool _ceka_obradu, bool _robot_drzi_predmet)
    : pozicija(_pozicija), velicina(_velicina), ceka_obradu(_ceka_obradu), robot_drzi_predmet(_robot_drzi_predmet)
{}

int Predmet::uzmi_poziciju()
{
    return pozicija;
}

void Predmet::promeni_poziciju(int indeks_s_cvora)
{
    pozicija = indeks_s_cvora;
}

int Predmet::uzmi_velicinu()
{
    return velicina;
}

bool Predmet::da_li_ceka_obradu()
{
    return ceka_obradu;
}

void Predmet::promeni_status_obrade(bool status)
{
    ceka_obradu = status;
}

bool Predmet::da_li_robot_drzi_predmet()
{
    return robot_drzi_predmet;
}

void Predmet::uhvati_predmet()
{
    robot_drzi_predmet = true;
}

void Predmet::spusti_predmet()
{
    robot_drzi_predmet = false;
}

