#include "domen.h"
#include "oblast_za_crtanje.h"

Domen::Domen()
{
}

Domen::~Domen()
{}

void Domen::inicijalizuj_domen(string putanja_do_fajla)
{
    // promenljive koje ce biti koriscene za ucitavanje domena
    string naziv_povrsine = "";
    int indeks_povrsine;
    int indeks_b_tacke;
    int indeks_prve_b_tacke, indeks_druge_b_tacke, duzina_putanje;
    int indeks_prve_s_tacke, indeks_druge_s_tacke;
    int x, y; // pozicije za iscrtavanje

    // indikator koji govori koja informacija se trenutno ucitava
    enum Trenutno_se_konfigurise { undef, povrsina, s_tacka, b_tacka, putanja, blokiranje };
    Trenutno_se_konfigurise trenutno_se_konfigurise = undef;

    string linija;
    ifstream konfiguracioni_fajl_za_domen (putanja_do_fajla);

    if (konfiguracioni_fajl_za_domen.is_open())
    {
      while ( getline (konfiguracioni_fajl_za_domen, linija) )
      {
          if(linija.length() > 0) //proveriti da li ima oznaku \r za kraj reda
              linija = linija.at(linija.length()-1) == '\r' ? linija.substr(0, linija.length()-1) : linija;
          if(linija != ""){
              stringstream linija_stream(linija);
              if (linija == "Povrsine"){
                  trenutno_se_konfigurise = povrsina;
              }
              else if (linija == "B tacke"){
                  trenutno_se_konfigurise = b_tacka;
              }
              else if (linija == "S tacke"){
                  trenutno_se_konfigurise = s_tacka;
              }
              else if (linija == "Putanje"){
                  trenutno_se_konfigurise = putanja;
              }
              else if (linija == "Medjusobna blokiranja S tacaka"){
                  trenutno_se_konfigurise = blokiranje;
                  inicijalizuj_medjusobna_blokiranja();
              }
              else{
                  switch (trenutno_se_konfigurise) {
                  case povrsina:
                      linija_stream >> naziv_povrsine >> x >> y;
                      povrsine.push_back(Povrsina(naziv_povrsine));
                      oblast_za_crtanje->dodaj_povrsinu(x, y, naziv_povrsine);
                      break;
                  case b_tacka:
                      linija_stream >> indeks_povrsine >> x >> y;
                      b_tacke.push_back(B_tacka(indeks_povrsine));
                      oblast_za_crtanje->dodaj_b_tacku(x, y, "B" + to_string(b_tacke.size()-1));
                      break;
                  case s_tacka:
                      linija_stream >> indeks_b_tacke >> x >> y;
                      s_tacke.push_back(S_tacka(indeks_b_tacke));
                      oblast_za_crtanje->dodaj_s_tacku(x, y, "S" + to_string(s_tacke.size()-1));
                      break;
                  case putanja:
                      linija_stream >> indeks_prve_b_tacke >> indeks_druge_b_tacke >> duzina_putanje;
                      putanje.push_back(Putanja(indeks_prve_b_tacke, indeks_druge_b_tacke, duzina_putanje));
                      oblast_za_crtanje->dodaj_putanju(oblast_za_crtanje->uzmi_b_tacku_za_iscrtavanje(indeks_prve_b_tacke),
                                                       oblast_za_crtanje->uzmi_b_tacku_za_iscrtavanje(indeks_druge_b_tacke),
                                                       duzina_putanje, true);
                      break;
                  case blokiranje:
                      linija_stream >> indeks_prve_s_tacke >> indeks_druge_s_tacke;
                      medjusobna_blokiranja_s_tacaka[indeks_prve_s_tacke][indeks_druge_s_tacke] = true;
                      oblast_za_crtanje->dodaj_blokiranje(oblast_za_crtanje->uzmi_s_tacku_za_iscrtavanje(indeks_druge_s_tacke),
                                                          "S" + to_string(indeks_prve_s_tacke));
                      break;
                  default:
                      break;
                  }
              }
          }
      }
      konfiguracioni_fajl_za_domen.close();
      nadji_najkrace_putanje_izmedju_b_tacaka();
      for (unsigned i = 0; i < putanje.size(); i++) {
          if (putanje[i].uzmi_duzinu() > 0)
            oblast_za_crtanje->dodaj_putanju_flojd_varsal(oblast_za_crtanje->uzmi_b_tacku_za_iscrtavanje(putanje[i].uzmi_indeks_prve_b_tacke()),
                                                          oblast_za_crtanje->uzmi_b_tacku_za_iscrtavanje(putanje[i].uzmi_indeks_druge_b_tacke()),
                                                          putanje[i].uzmi_duzinu(), putanje[i].uzmi_sekvencu_b_tacki().size() == 1);
      }
    }
    else std::cout << "Nije moguce otvoriti fajl" << std::endl;

    oblast_za_crtanje->nacrtaj_domen();
}

int Domen::uzmi_duzinu_niza_povrsina()
{
    return povrsine.size();
}

Povrsina Domen::uzmi_povrsinu(int indeks_povrsine)
{
    return povrsine[indeks_povrsine];
}

int Domen::uzmi_duzinu_niza_b_tacaka()
{
    return b_tacke.size();
}

B_tacka Domen::uzmi_b_tacku(int indeks_b_tacke)
{
    return b_tacke[indeks_b_tacke];
}

int Domen::uzmi_duzinu_niza_s_tacaka()
{
    return s_tacke.size();
}

S_tacka Domen::uzmi_s_tacku(int indeks_s_tacke)
{
    return s_tacke[indeks_s_tacke];
}

int Domen::uzmi_duzinu_niza_putanja()
{
    return putanje.size();
}

Putanja Domen::uzmi_putanju(int indeks_putanje)
{
    return putanje[indeks_putanje];
}

int Domen::uzmi_duzinu_niza_medjusobnih_blokiranja_s_tacaka()
{
    return medjusobna_blokiranja_s_tacaka.size();
}

bool Domen::da_li_blokira(int indeks_s_tacke_1, int indeks_s_tacke_2)
{

    bool blokira = medjusobna_blokiranja_s_tacaka[indeks_s_tacke_1][indeks_s_tacke_2];
    return blokira;

}

void Domen::inicijalizuj_medjusobna_blokiranja()
{
    int duzina_niza_s_tacaka = uzmi_duzinu_niza_s_tacaka();
    for (int i=0; i<duzina_niza_s_tacaka; i++)
    {
        vector<bool> niz_j;
        for (int j=0; j<duzina_niza_s_tacaka; j++)
             niz_j.push_back(false);
        medjusobna_blokiranja_s_tacaka.push_back(niz_j);
    }
}

void Domen::nadji_najkrace_putanje_izmedju_b_tacaka()
{

    int duzina_niza_b_tacaka = uzmi_duzinu_niza_b_tacaka();
    int duzina_niza_putanja = uzmi_duzinu_niza_putanja();
    vector<vector<int>> pocetna_matrica_putanja;
    vector<vector<int>> rezultujuca_matrica_putanja;

    //inicijalizacije matrica neophodnih za primenu Flojd-Varsalovog algoritma
    for(int i=0; i<duzina_niza_b_tacaka; i++){
        vector<int> tacke;
        for(int j=0; j<duzina_niza_b_tacaka; j++){
            if (i == j)
                tacke.push_back(0);
            else
                tacke.push_back(INF);
        }
        rezultujuca_matrica_putanja.push_back(tacke);
        pocetna_matrica_putanja.push_back(tacke);
        matrica_prelaza.push_back(tacke);
    }

    //popunjavanje pocetne matrica putanja informacijama dobijenim iz domena
    for(int i=0; i<duzina_niza_putanja; i++)
    {
        int indeks_b_tacke1 = uzmi_putanju(i).uzmi_indeks_prve_b_tacke();
        int indeks_b_tacke2 = uzmi_putanju(i).uzmi_indeks_druge_b_tacke();
        int duzina_putanje = uzmi_putanju(i).uzmi_duzinu();
        if(pocetna_matrica_putanja[indeks_b_tacke1][indeks_b_tacke2] > duzina_putanje)
        {
           pocetna_matrica_putanja[indeks_b_tacke1][indeks_b_tacke2] =  duzina_putanje;
           pocetna_matrica_putanja[indeks_b_tacke2][indeks_b_tacke1] =  duzina_putanje;
        }
    }

    //inicijalizacija matrice prelaza
    for (int i=0; i<duzina_niza_b_tacaka; i++) {
        for (int j=0; j<duzina_niza_b_tacaka; j++) {
            rezultujuca_matrica_putanja[i][j] = pocetna_matrica_putanja[i][j];
            if (i != j && pocetna_matrica_putanja[i][j] != INF)
                matrica_prelaza[i][j] = i;
            else
                matrica_prelaza[i][j] = -1;
        }
    }

    //Flojd-Varsalov algoritam za nalazenje najkracih putanja u grafu
    for (int k = 0; k < duzina_niza_b_tacaka; k++) {
        for (int i = 0; i < duzina_niza_b_tacaka; i++) {
            for (int j = 0; j < duzina_niza_b_tacaka; j++) {
                if (rezultujuca_matrica_putanja[i][j] > (rezultujuca_matrica_putanja[i][k] + rezultujuca_matrica_putanja[k][j]) &&
                    rezultujuca_matrica_putanja[i][k] != INF && rezultujuca_matrica_putanja[k][j] != INF)
                {
                    rezultujuca_matrica_putanja[i][j] = rezultujuca_matrica_putanja[i][k] + rezultujuca_matrica_putanja[k][j];
                    matrica_prelaza[i][j] = matrica_prelaza[k][j];
                }
            }
        }
    }

    putanje.clear();
    for(int i=0; i<duzina_niza_b_tacaka; i++){
        for(int j=0; j<duzina_niza_b_tacaka; j++){
            vector<int> sekvenca_tacaka;
            int duzina = rezultujuca_matrica_putanja[i][j];
            if(i == j)
            {
                sekvenca_tacaka.push_back(i);
                putanje.push_back(Putanja(i,i,0,sekvenca_tacaka));
            }
            else if(duzina != INF)
            {
                pronadji_sekvencu_tacaka(i, j, sekvenca_tacaka);
                std::reverse(sekvenca_tacaka.begin(), sekvenca_tacaka.end());
                putanje.push_back(Putanja(i,j,rezultujuca_matrica_putanja[i][j],sekvenca_tacaka));
            }
        }
    }
}

void Domen::ispisi_matricu(vector<vector<int>> matrica){
    int duzina = matrica.size();
    for(int i=0; i<duzina; i++){
        for(int j=0; j<duzina; j++)
            cout << matrica[i][j] << " " ;
        cout << endl;
    }
    cout << endl;
}

void Domen::pronadji_sekvencu_tacaka(int i, int j, vector<int>& sekvenca_tacaka)
{
    if(i == j)
        return;
    else
    {
        if(matrica_prelaza[i][j] == -1)
            return;
        sekvenca_tacaka.push_back(j);
        pronadji_sekvencu_tacaka(i, matrica_prelaza[i][j], sekvenca_tacaka);
    }

}

vector<int> Domen::uzmi_sekvencu_tacaka_za_par_b_tacaka(int indeks_b_tacke_1, int indeks_b_tacke_2)
{
    int duzina_niza_putanja = putanje.size();
    for(int i = 0; i < duzina_niza_putanja; i++)
        if(putanje[i].uzmi_indeks_prve_b_tacke() == indeks_b_tacke_1 &&
           putanje[i].uzmi_indeks_druge_b_tacke() == indeks_b_tacke_2)
            return putanje[i].uzmi_sekvencu_b_tacki();

    return vector<int>();
}

void Domen::ocisti_domen()
{
    povrsine.clear();
    b_tacke.clear();
    s_tacke.clear();
    putanje.clear();
    medjusobna_blokiranja_s_tacaka.clear();
    matrica_prelaza.clear();
}

//class Putanja
Putanja::Putanja(int _indeks_b_tacke_1, int _indeks_b_tacke_2, int _duzina)
    : indeks_b_tacke_1(_indeks_b_tacke_1), indeks_b_tacke_2(_indeks_b_tacke_2), duzina(_duzina)
{}

Putanja::Putanja(int _indeks_b_tacke_1, int _indeks_b_tacke_2, int _duzina, vector<int> _sekvenca_b_tacki)
    : indeks_b_tacke_1(_indeks_b_tacke_1), indeks_b_tacke_2(_indeks_b_tacke_2), duzina(_duzina), sekvenca_b_tacki(_sekvenca_b_tacki)
{}

int Putanja::uzmi_indeks_prve_b_tacke()
{
    return indeks_b_tacke_1;
}

int Putanja::uzmi_indeks_druge_b_tacke()
{
    return indeks_b_tacke_2;
}

int Putanja::uzmi_duzinu()
{
    return duzina;
}

vector<int> Putanja::uzmi_sekvencu_b_tacki()
{
    return sekvenca_b_tacki;
}

//class Povrsina
Povrsina::Povrsina(string _naziv)
    : naziv(_naziv)
{}

string Povrsina::uzmi_ime_povrsine()
{
    return naziv;
}

//class B_tacka
B_tacka::B_tacka(int _indeks_povrsine)
    : indeks_povrsine(_indeks_povrsine)
{
}

int B_tacka::uzmi_indeks_povrsine()
{
    return indeks_povrsine;
}

//class S_tacka
S_tacka::S_tacka(int _indeks_b_tacke)
    : indeks_b_tacke(_indeks_b_tacke)
{}

int S_tacka::uzmi_indeks_b_tacke()
{
    return indeks_b_tacke;
}
