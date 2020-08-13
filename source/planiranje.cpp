#include "planiranje.h"
#include <z3++.h>

using namespace std;
using namespace z3;

Planiranje::Planiranje(Domen _domen, Scena* _scena)
{
    domen = _domen;
    scena = _scena;
}

bool Planiranje::generisi_plan()
{
    QFile file("Plan.txt");
    bool prvi_korak_je_zadovoljiv = true;
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);

        int i, j;
        int ukupna_duzina_predjene_putanje = 0;

        //definisanje konteksta
        context robot_kontekst;

        //definisanje konstanti i funkcija
        expr b                              = robot_kontekst.int_const("b");
        expr predmet                        = robot_kontekst.int_const("predmet");
        expr trenutna_pozicija              = robot_kontekst.int_const("trenutna_pozicija");
        expr duzina_putanje                 = robot_kontekst.int_const("duzina_putanje");
        z3::sort I                          = robot_kontekst.int_sort();
        z3::sort B                          = robot_kontekst.bool_sort();
        expr ukupna_predjena_putanja        = robot_kontekst.int_const("ukupna_predjena_putanja");
        expr maksimalna_duzina              = robot_kontekst.int_const("maksimalna_duzina");
        expr robot_je_u_b_tacki_izabranog_predmeta = robot_kontekst.bool_const("robot_je_u_b_tacki_izabranog_predmeta");
        expr robot_drzi_izabrani_predmet    = robot_kontekst.bool_const("robot_drzi_izabrani_predmet");
        z3::func_decl robot_drzi_predmet    = z3::function("robot_drzi_predmet", I, B);
        z3::func_decl blokira               = z3::function("blokira", I, I, B);
        z3::func_decl veci                  = z3::function("veci", I, I, B);
        z3::func_decl predmet_ceka_obradu   = z3::function("predmet_ceka_obradu", I, B);
        expr izabrani_predmet               = robot_kontekst.int_const("izabrani_predmet");
        expr boolean_true                   = robot_kontekst.bool_val(true);
        expr boolean_false                  = robot_kontekst.bool_val(false);
        z3::func_decl putanja               = z3::function("putanja", I, I, B);
        expr izabrano_mesto_u_masini        = robot_kontekst.int_const("izabrano_mesto_u_masini");
        z3::func_decl slobodno              = z3::function("slobodno", I, B);
        z3::func_decl izabrani_predmet_nije_u_masini  = z3::function("izabrani_predmet_nije_u_masini", I, B);
        z3::func_decl predmet_je_obradjen   = z3::function("predmet_je_obradjen", I, B);
        expr izabrano_mesto_u_skladistu     = robot_kontekst.int_const("izabrano_mesto_u_skladistu");
        expr robot_je_u_b_tacki_masine      = robot_kontekst.bool_const("robot_je_u_b_tacki_masine");
        expr pozicija_izabranog_predmeta    = robot_kontekst.int_const("pozicija_izabranog_predmeta");

        //inicijalizacija smt resavaca
        solver robot_resavac(robot_kontekst);
        expr dodatni_uslov = boolean_true;

        //dodavanje informacija o putanjama
        expr uslov_za_sve_putanje = boolean_false; //((trenutna_pozicija == b) && (duzina_putanje == 0));
        int duzina_niza_putanja = domen.uzmi_duzinu_niza_putanja();
        for(i=0; i<duzina_niza_putanja; i++)
        {
            expr uslov_za_putanju1 = putanja(trenutna_pozicija, b) == boolean_true;
            expr uslov_za_putanju2 = (trenutna_pozicija == domen.uzmi_putanju(i).uzmi_indeks_prve_b_tacke());
            expr uslov_za_putanju3 = (b == domen.uzmi_putanju(i).uzmi_indeks_druge_b_tacke());
            expr uslov_za_putanju4 = (duzina_putanje == domen.uzmi_putanju(i).uzmi_duzinu());
            uslov_za_sve_putanje = uslov_za_sve_putanje || (uslov_za_putanju1 && uslov_za_putanju2 && uslov_za_putanju3 && uslov_za_putanju4);
        }
        robot_resavac.add(uslov_za_sve_putanje);
        //stream << uslov_za_sve_putanje.to_string() << "\n";

        //izdvajanje indeksa b tacke masine za obradu i skladista
        int indeks_b_tacke_masine, indeks_b_tacke_skladista;
        int duzina_niza_b_tacaka = domen.uzmi_duzinu_niza_b_tacaka();
        for(i=0; i<duzina_niza_b_tacaka; i++){
            if(domen.uzmi_povrsinu(domen.uzmi_b_tacku(i).uzmi_indeks_povrsine()).uzmi_ime_povrsine() == "masina_za_obradu"){
                indeks_b_tacke_masine = i;
            }
            if(domen.uzmi_povrsinu(domen.uzmi_b_tacku(i).uzmi_indeks_povrsine()).uzmi_ime_povrsine() == "skladiste"){
                indeks_b_tacke_skladista = i;
            }
        }

        //dodavanje ogranicenja za maksimalnu duzinu predjene putanje
        robot_resavac.add(maksimalna_duzina == 200);
        robot_resavac.add(ukupna_predjena_putanja < maksimalna_duzina);

        //dodavanje informacija o tome koje pozicije u masini za obradu blokiraju jedne druge
        //cuva indeks s tacaka koji predstavljaju mesta u masini
        vector<int> s_tacke_masine;
        int duzina_niza_s_tacaka = domen.uzmi_duzinu_niza_s_tacaka();
        for(i=0; i<duzina_niza_s_tacaka; i++)
            if(domen.uzmi_s_tacku(i).uzmi_indeks_b_tacke() == indeks_b_tacke_masine)
                s_tacke_masine.push_back(i);
        int duzina_niza_s_tacaka_masine = s_tacke_masine.size();

        //cuva indeks s tacaka koji predstavljaju mesta u skladistu
        vector<int> s_tacke_skladista;
        for(i=0; i<duzina_niza_s_tacaka; i++)
            if(domen.uzmi_s_tacku(i).uzmi_indeks_b_tacke() == indeks_b_tacke_skladista)
                s_tacke_skladista.push_back(i);
        int duzina_niza_s_tacaka_skladista = s_tacke_skladista.size();

        // dodavanje informacija o tome koje s tacke blokiraju jedne druge
        expr informacije_o_blokiranju = boolean_true;
        for(i=0; i<duzina_niza_s_tacaka; i++)
            for(j=0; j<duzina_niza_s_tacaka; j++)
                if(domen.da_li_blokira(i, j))
                    informacije_o_blokiranju = informacije_o_blokiranju && (blokira(robot_kontekst.int_val(i), robot_kontekst.int_val(j)) == boolean_true);
        robot_resavac.add(informacije_o_blokiranju);

        //dodavanje informacija o odnosu velicine predmeta
        int duzina_niza_predmeta = scena->uzmi_duzinu_niza_predmeta();
        expr odnos_velicina = boolean_true;
        for(i=0; i<duzina_niza_predmeta; i++)
            for(j=0; j<duzina_niza_predmeta; j++)
                if(scena->uzmi_predmet(i).uzmi_velicinu() > scena->uzmi_predmet(j).uzmi_velicinu())
                    odnos_velicina = odnos_velicina && (veci(robot_kontekst.int_val(i), robot_kontekst.int_val(j)) == boolean_true);
        robot_resavac.add(odnos_velicina);
        //ISPIS stream << "informacije o odnosu velicine predmeta: " << "\n" << robosint_resavac.to_smt2() << "\n";

        /* PAKOVANJE PREDMETA U MASINU ZA OBRADU */
        bool uslov_petlje = false;
        for (int i=0; i<duzina_niza_predmeta; i++)
            uslov_petlje = uslov_petlje || ((domen.uzmi_s_tacku(scena->uzmi_predmet(i).uzmi_poziciju()).uzmi_indeks_b_tacke() != indeks_b_tacke_masine) && scena->uzmi_predmet(i).da_li_ceka_obradu());

        //ispis inicijalne scene
        stream << "INICIJALNA SCENA:\n";
        for(int i=0; i<duzina_niza_predmeta; i++)
            stream << "Predmet " << i << " se nalazi u tacki s[" << scena->uzmi_predmet(i).uzmi_poziciju() << "]\n";
        stream << "Robot se nalazi u tacki b[" << scena->uzmi_trenutnu_poziciju_robota() << "]\n";
        stream << "--------------------------------\n" ;

        //iscrtavanje inicijalne scene
        scena->dodaj_stanje_scene(scena->uzmi_trenutnu_poziciju_robota());

        while (uslov_petlje && prvi_korak_je_zadovoljiv)
        {
            robot_resavac.push();

            //uslov za duzinu putanje
            robot_resavac.add(ukupna_predjena_putanja == ukupna_duzina_predjene_putanje);

            /* preduslov za korak "nadji mesto" */
            //izabrani predmet ceka obradu (dodavanje trenutnog stanja predmeta)
            expr stanje_predmeta = boolean_false;
            for(i=0; i< duzina_niza_predmeta; i++)
                if(scena->uzmi_predmet(i).da_li_ceka_obradu())
                    stanje_predmeta = stanje_predmeta || (izabrani_predmet == i);

            stanje_predmeta = (stanje_predmeta) && predmet_ceka_obradu(izabrani_predmet) == boolean_true;
            robot_resavac.add(stanje_predmeta);

            //masina za obradu nije puna + izabrano mesto pripada masini + izabrano mesto je prazno (dodavanje trenutnog stanja)
            expr stanje_slobodnih_mesta_u_masini = boolean_false;
            bool cvor_je_zauzet;

            //cuva indeks s tacaka koje predstavljaju slobodna mesta u masini za obradu
            vector<int> slobodna_mesta_u_masini;
            for(i=0; i<duzina_niza_s_tacaka_masine; i++){
                    cvor_je_zauzet = false;
                    for(j=0; j<duzina_niza_predmeta; j++)
                        if(scena->uzmi_predmet(j).uzmi_poziciju() == s_tacke_masine[i])
                        {
                            cvor_je_zauzet = true;
                            break;
                        }
                    //ako nijedan predmet nije u toj s tacki, a ona pripada masini za obradu, onda masina nije puna
                    if(cvor_je_zauzet == false)
                        slobodna_mesta_u_masini.push_back(s_tacke_masine[i]);

            }
            int duzina_niza_slobodnih_mesta_u_masini = slobodna_mesta_u_masini.size();

            for(i=0; i<duzina_niza_slobodnih_mesta_u_masini; i++)
                stanje_slobodnih_mesta_u_masini = stanje_slobodnih_mesta_u_masini || (izabrano_mesto_u_masini == slobodna_mesta_u_masini[i]);

            stanje_slobodnih_mesta_u_masini = (stanje_slobodnih_mesta_u_masini) && slobodno(izabrano_mesto_u_masini) == boolean_true;
            robot_resavac.add(stanje_slobodnih_mesta_u_masini);

            //predmet nije u masini (dodavanje trenutnog stanja)
            expr uslov_izabrani_predmet_nije_u_masini = boolean_false;
                    for(int i=0; i<duzina_niza_predmeta; i++)
                        uslov_izabrani_predmet_nije_u_masini = uslov_izabrani_predmet_nije_u_masini || (izabrani_predmet_nije_u_masini(izabrani_predmet) == (domen.uzmi_s_tacku(scena->uzmi_predmet(i).uzmi_poziciju()).uzmi_indeks_b_tacke() != indeks_b_tacke_masine ? boolean_true : boolean_false) && (izabrani_predmet == i));
            robot_resavac.add(uslov_izabrani_predmet_nije_u_masini);

            //uslovi koji treba da budu ispunjeni za korak "nadji mesto"
            robot_resavac.add(predmet_ceka_obradu(izabrani_predmet) == boolean_true);
            robot_resavac.add(slobodno(izabrano_mesto_u_masini) == boolean_true);

            //izabrano mesto u masini za obradu ne treba da blokira ostala prazna mesta u masini
            expr izabrano_mesto_ne_blokira_prazno_mesto = boolean_true;
            for(i=0; i<duzina_niza_slobodnih_mesta_u_masini; i++){
                //if(slobodna_mesta_u_masini[i] != izabrano_mesto_u_masini)
                izabrano_mesto_ne_blokira_prazno_mesto = izabrano_mesto_ne_blokira_prazno_mesto && (blokira(izabrano_mesto_u_masini, robot_kontekst.int_val(slobodna_mesta_u_masini[i])) == boolean_false);
            }
            robot_resavac.add(izabrano_mesto_ne_blokira_prazno_mesto);
    //        stream << izabrano_mesto_ne_blokira_prazno_mesto.to_string() << "\n";

            //ne postoji veci predmet koji ceka obradu od izabranog predmeta koji nije vec u masini
            expr ne_postoji_veci_van_masine = boolean_true;
            for (int i=0; i<duzina_niza_predmeta; i++)
                if((domen.uzmi_s_tacku(scena->uzmi_predmet(i).uzmi_poziciju()).uzmi_indeks_b_tacke() != indeks_b_tacke_masine) && (scena->uzmi_predmet(i).da_li_ceka_obradu()))
                    ne_postoji_veci_van_masine = ne_postoji_veci_van_masine && (veci(robot_kontekst.int_val(i), izabrani_predmet) == boolean_false);
            robot_resavac.add(ne_postoji_veci_van_masine);
            robot_resavac.add(izabrani_predmet_nije_u_masini(izabrani_predmet) == boolean_true);
            robot_resavac.add(dodatni_uslov); //odsecanje - ako je pao na nekom od naredna dva koraka
            //ISPIS stream << robosint_resavac.to_smt2() << "\n";

            int izabrani_predmet_dobijen_iz_modela;
            int izabrano_mesto_u_masini_dobijeno_iz_modela;
            int duzina_putanje_dobijena_iz_modela;
            //ISPIS stream << robosint_resavac << "\n";

            switch (robot_resavac.check())
            {
                case unknown:
                case unsat: stream << "* korak NADJI MESTO je UNSAT *\n";
                            prvi_korak_je_zadovoljiv = false;
                            stream << "--------------------------------\n";
                            break;
                case sat:   stream << "* korak NADJI MESTO je SAT *\n";
                        prvi_korak_je_zadovoljiv = true;
                        model dobijeni_model = robot_resavac.get_model();
                        //obilazak modela
                        for (unsigned i = 0; i < dobijeni_model.size(); i++) {
                            func_decl v = dobijeni_model[i];
                            if(v.name().str() == "izabrani_predmet")
                                izabrani_predmet_dobijen_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                            if(v.name().str() == "izabrano_mesto_u_masini")
                                izabrano_mesto_u_masini_dobijeno_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                        }
                        stream << "izabrani_predmet_dobijen_iz_modela: predmet " << izabrani_predmet_dobijen_iz_modela << "\n" << "izabrano_mesto_u_masini_dobijeno_iz_modela: s[" << izabrano_mesto_u_masini_dobijeno_iz_modela << "]\n";
                        stream << "trenutna predjena duzina putanje je: " << ukupna_duzina_predjene_putanje << "\n";
                break;
            }
            robot_resavac.pop();

            if(prvi_korak_je_zadovoljiv) // ako je prvi korak nezadovoljiv, cela formula je nezadovoljiva
            {
                robot_resavac.push();

                //uslov za duzinu putanje
                robot_resavac.add(ukupna_predjena_putanja == (ukupna_duzina_predjene_putanje + duzina_putanje));

                //dodavanje modela dobijenog u prethodnom koraku
                expr uslov_iz_prethodnih_koraka = (izabrani_predmet == izabrani_predmet_dobijen_iz_modela) && (izabrano_mesto_u_masini == izabrano_mesto_u_masini_dobijeno_iz_modela);
                robot_resavac.add(uslov_iz_prethodnih_koraka);
                bool drugi_korak_je_zadovoljiv = true;

                /* preduslovi za korak "pokupi" */
                //robot ne drzi nijedan predmet (dodavanje trenutnog stanja)
                expr uslov_robot_drzi_predmet = boolean_false;
                for(int i=0; i<duzina_niza_predmeta; i++)
                    uslov_robot_drzi_predmet = uslov_robot_drzi_predmet || ((robot_drzi_predmet(predmet) == (scena->uzmi_predmet(i).da_li_robot_drzi_predmet() ? boolean_true : boolean_false)) && (predmet == i));

                //izabrani predmet nema puno mesto u masini za obradu koje ga blokira (dodavanje trenutnog stanja)
                //cuva indeks s tacaka koji predstavljaju zauzeta mesta u masini
                int s_tacka_predmeta = scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela).uzmi_poziciju();
                vector<int> zauzete_s_tacke_povrsina;
                for(i=0; i<duzina_niza_predmeta; i++){
                    if( domen.uzmi_s_tacku(s_tacka_predmeta).uzmi_indeks_b_tacke() != indeks_b_tacke_masine && domen.uzmi_s_tacku(s_tacka_predmeta).uzmi_indeks_b_tacke() != indeks_b_tacke_skladista)
                        zauzete_s_tacke_povrsina.push_back(s_tacka_predmeta);
                }
                int duzina_niza_zauzetih_s_tacaka_povrsina = zauzete_s_tacke_povrsina.size();

                //dodavanje uslova koji treba da budu ispunjeni za korak "pokupi"
                //robot ne sme da drzi nijedan predmet
                robot_resavac.add(uslov_robot_drzi_predmet == boolean_false);
                int pozicija_robota_pre_koraka_pokupi = scena->uzmi_trenutnu_poziciju_robota();
                vector<int> sekvenca_b_tacaka;

                // postoji putanja od trenutne pozicije do pozicije izabranog predmeta
                //opis putanja je vec dodat, dodaju se samo uslovi koji treba da budu ispunjeni:
                robot_resavac.add(trenutna_pozicija == pozicija_robota_pre_koraka_pokupi);
                robot_resavac.add(putanja(trenutna_pozicija, b) == boolean_true);
                robot_resavac.add(b == domen.uzmi_s_tacku(scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela).uzmi_poziciju()).uzmi_indeks_b_tacke());

                //izabrani predmet nema drugi predmet koji ga blokira
                int indeks_s_tacke_izabranog_predmeta = scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela).uzmi_poziciju();
                expr ne_postoji_predmet_koji_blokira_izabrani_predmet = boolean_true;
                for(i=0; i<duzina_niza_zauzetih_s_tacaka_povrsina; i++)
                    ne_postoji_predmet_koji_blokira_izabrani_predmet = ne_postoji_predmet_koji_blokira_izabrani_predmet && (blokira(robot_kontekst.int_val(zauzete_s_tacke_povrsina[i]), robot_kontekst.int_val(indeks_s_tacke_izabranog_predmeta)) == boolean_false);
                robot_resavac.add(ne_postoji_predmet_koji_blokira_izabrani_predmet);

                int b_dobijeno_iz_modela;
                //ISPIS stream << robosint_resavac.to_smt2() << "\n";
                switch (robot_resavac.check())
                {
                    case unknown:
                    case unsat: stream << "* korak POKUPI je UNSAT *\n";
                                drugi_korak_je_zadovoljiv = false;
                                dodatni_uslov = dodatni_uslov && ((izabrani_predmet != izabrani_predmet_dobijen_iz_modela) || (izabrano_mesto_u_masini != izabrano_mesto_u_masini_dobijeno_iz_modela));
                                stream << "2. korak nezadovoljiv, povratak na prvi korak\n" ;
                                stream << "--------------------------------\n";
                                break;
                    case sat:   stream << "* korak POKUPI je SAT *\n";
                            model dobijeni_model = robot_resavac.get_model();
    //                        std::cout << dobijeni_model << std::endl;
                            drugi_korak_je_zadovoljiv = true;
                            // obilazak modela
                            for (unsigned i = 0; i < dobijeni_model.size(); i++) {
                                func_decl v = dobijeni_model[i];
                                if(v.name().str() == "b")
                                    b_dobijeno_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                                if(v.name().str() == "duzina_putanje"){
                                    duzina_putanje_dobijena_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                                    ukupna_duzina_predjene_putanje += duzina_putanje_dobijena_iz_modela;
                                }
                            }
                            sekvenca_b_tacaka = domen.uzmi_sekvencu_tacaka_za_par_b_tacaka(pozicija_robota_pre_koraka_pokupi, b_dobijeno_iz_modela);
                            for (int i = 0; i < sekvenca_b_tacaka.size(); i++)
                                scena->dodaj_stanje_scene(sekvenca_b_tacaka[i]);
                            scena->promeni_trenutnu_poziciju_robota(b_dobijeno_iz_modela);
                            stream << "pozicija robota je promenjena \n" << "trenutna pozicija robota je b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << "\n";
                            stream << "do cvora b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << " je predjen put " << duzina_putanje_dobijena_iz_modela << "\n";
                            Predmet& p = scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela);
                            p.uhvati_predmet();
                            scena->dodaj_stanje_scene(scena->uzmi_trenutnu_poziciju_robota());
                            stream << "robot je pokupio predmet " << izabrani_predmet_dobijen_iz_modela <<  "\n";
                            stream << "trenutna predjena duzina putanje je: " << ukupna_duzina_predjene_putanje << "\n";
                            break;
                }
                robot_resavac.pop();
                if(drugi_korak_je_zadovoljiv)
                {
                    robot_resavac.push();

                    //uslov za duzinu putanje
                    robot_resavac.add(ukupna_predjena_putanja == (ukupna_duzina_predjene_putanje + duzina_putanje));

                    //dodavanje modela dobijenog u prethodnim koracima
                    uslov_iz_prethodnih_koraka = uslov_iz_prethodnih_koraka; // sadrzi indeks b tacke izabranog predmeta
                    robot_resavac.add(uslov_iz_prethodnih_koraka);

                    /*preduslovi koji treba da budu ispunjeni za korak "smesti" */
                    //robot se nalazi u b tacki izabranog predmeta (uzimanje trenutnog stanja)
                    int trenutna_pozicija_robota = scena->uzmi_trenutnu_poziciju_robota();
                    expr uslov_robot_je_u_b_tacki_izabranog_predmeta = robot_je_u_b_tacki_izabranog_predmeta == ((trenutna_pozicija_robota == b_dobijeno_iz_modela) ? boolean_true : boolean_false);
                    robot_resavac.add(uslov_robot_je_u_b_tacki_izabranog_predmeta);

                    //robot drzi izabrani predmet (uzimanje trenutnog stanja)
                    expr uslov_robot_drzi_izabrani_predmet = robot_drzi_izabrani_predmet == (scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela).da_li_robot_drzi_predmet() ? boolean_true : boolean_false);
                    robot_resavac.add(uslov_robot_drzi_izabrani_predmet);

                    //uslovi koji treba da budu ispunjeni za korak "smesti"
                    //postoji putanja od b tacke izabranog predmeta do masine
                    //opis putanja je vec dodat, dodaju se samo uslovi koji treba da budu ispunjeni:
                    robot_resavac.add(trenutna_pozicija == scena->uzmi_trenutnu_poziciju_robota());
                    robot_resavac.add(putanja(trenutna_pozicija, b) == boolean_true);
                    robot_resavac.add(b == indeks_b_tacke_masine);
                    //robot se nalazi u b tacki izabranog predmeta
                    robot_resavac.add(robot_je_u_b_tacki_izabranog_predmeta == boolean_true);
                    //robot drzi izabrani predmet
                    robot_resavac.add(robot_drzi_izabrani_predmet == boolean_true);

                    //ISPIS stream << robosint_resavac.to_smt2() << "\n";
                    Predmet& p = scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela);
                    switch (robot_resavac.check())
                    {
                        case unknown:
                        case unsat: stream << "* korak SMESTI je UNSAT *\n";
                                    dodatni_uslov = dodatni_uslov && ((izabrani_predmet != izabrani_predmet_dobijen_iz_modela) || (izabrano_mesto_u_masini != izabrano_mesto_u_masini_dobijeno_iz_modela));
                                    stream << "3. korak nezadovoljiv - vracanje na prethodne pozicije\n";
                                    p.spusti_predmet();
                                    scena->dodaj_stanje_scene(trenutna_pozicija_robota);
                                    stream << "robot je vratio predmet " << izabrani_predmet_dobijen_iz_modela <<  " na njegovu prethodnu poziciju\n";
//                                  scena->promeni_trenutnu_poziciju_robota(pozicija_robota_pre_koraka_pokupi);
                                    stream << "trenutna pozicija robota: b[" << scena->uzmi_trenutnu_poziciju_robota() << "]\n";
                                    stream << "--------------------------------\n";
                                    break;
                        case sat:   stream << "* korak SMESTI je SAT *\n";
                                    model dobijeni_model = robot_resavac.get_model();
//                                  std::cout << dobijeni_model << std::endl;
                                    // obilazak modela
                                    for (unsigned i = 0; i < dobijeni_model.size(); i++) {
                                        func_decl v = dobijeni_model[i];
                                        if(v.name().str() == "duzina_putanje"){
                                            duzina_putanje_dobijena_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                                            ukupna_duzina_predjene_putanje += duzina_putanje_dobijena_iz_modela;
                                        }
                                    }
                                    scena->promeni_trenutnu_poziciju_robota(indeks_b_tacke_masine);
                                    stream << "pozicija robota je promenjena \n" << "trenutna pozicija robota je b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << "\n";
                                    stream << "do cvora b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << " je predjen put " << duzina_putanje_dobijena_iz_modela << "\n";
                                    sekvenca_b_tacaka = domen.uzmi_sekvencu_tacaka_za_par_b_tacaka(trenutna_pozicija_robota, scena->uzmi_trenutnu_poziciju_robota());
                                    for (int i = 0; i < sekvenca_b_tacaka.size(); i++)
                                        scena->dodaj_stanje_scene(sekvenca_b_tacaka[i]);
                                    p.spusti_predmet();
                                    p.promeni_poziciju(izabrano_mesto_u_masini_dobijeno_iz_modela);
                                    scena->dodaj_stanje_scene(scena->uzmi_trenutnu_poziciju_robota());
                                    stream << "robot je spustio predmet " << izabrani_predmet_dobijen_iz_modela <<  " na poziciju s[" << izabrano_mesto_u_masini_dobijeno_iz_modela << "]\n";
                                    dodatni_uslov = boolean_true;
                                    stream << "Robot je u b cvoru masine, a on je: " << scena->uzmi_trenutnu_poziciju_robota() << "\n";
                                    stream << "trenutna predjena duzina putanje je: " << ukupna_duzina_predjene_putanje << "\n";
                                    stream << "--------------------------------\n" ;
                        break;
                    }
                    robot_resavac.pop();
                }

            }

            uslov_petlje = false;
            for (int i=0; i<duzina_niza_predmeta; i++)
                uslov_petlje = uslov_petlje || ((domen.uzmi_s_tacku(scena->uzmi_predmet(i).uzmi_poziciju()).uzmi_indeks_b_tacke() != indeks_b_tacke_masine) && scena->uzmi_predmet(i).da_li_ceka_obradu());
          }
        stream << "Korak pakovanja predmeta u masinu za obradu je: " << (prvi_korak_je_zadovoljiv ? "SAT" : "UNSAT") << "\n";

        if(prvi_korak_je_zadovoljiv){
            scena->zapocni_obradu();
            stream << "--------------------------------" << "\n";
            stream << "Obrada u toku..." << "\n";
            scena->zavrsi_obradu();
            stream << "Obrada je zavrsena." << "\n";
            stream << "--------------------------------" << "\n";

            /* PAKOVANJE OBRADJENIH PREDMETA U SKLADISTE */
            bool uslov_petlje = false;
            for (int i=0; i<duzina_niza_predmeta; i++)
                uslov_petlje = uslov_petlje || ((domen.uzmi_s_tacku(scena->uzmi_predmet(i).uzmi_poziciju()).uzmi_indeks_b_tacke() != indeks_b_tacke_skladista) && !(scena->uzmi_predmet(i).da_li_ceka_obradu()));

            //prvi_korak_je_zadovoljiv = true;
            dodatni_uslov = boolean_true;

            while (uslov_petlje && prvi_korak_je_zadovoljiv)
            {
                robot_resavac.push();

                //uslov za duzinu putanje
                robot_resavac.add(ukupna_predjena_putanja == ukupna_duzina_predjene_putanje);

                /* preduslov za korak "nadji mesto" */
                //izabrani predmet je obradjen (dodavanje trenutnog stanja predmeta)
                expr stanje_predmeta= boolean_false;
                for(i=0; i< duzina_niza_predmeta; i++)
                {
                    if(!scena->uzmi_predmet(i).da_li_ceka_obradu())
                        stanje_predmeta = stanje_predmeta || (izabrani_predmet == i);
                }
                stanje_predmeta = (stanje_predmeta) && predmet_je_obradjen(izabrani_predmet) == boolean_true;
                robot_resavac.add(stanje_predmeta);

                //skladiste nije puno + izabrano mesto pripada skladistu + izabrano mesto je prazno (dodavanje trenutnog stanja)
                expr stanje_slobodnih_mesta_u_skladistu = boolean_false;
                bool cvor_je_zauzet;

                //cuva indeks s tacaka koje predstavljaju slobodna mesta u skladistu
                vector<int> slobodna_mesta_u_skladistu;
                for(i=0; i<duzina_niza_s_tacaka_skladista; i++){
                        cvor_je_zauzet = false;
                        for(j=0; j<duzina_niza_predmeta; j++)
                            if(scena->uzmi_predmet(j).uzmi_poziciju() == s_tacke_skladista[i])
                            {
                                cvor_je_zauzet = true;
                                break;
                            }
                        //ako nijedan predmet nije u toj s tacki, a ona pripada skladistu, onda ono nije puno
                        if(cvor_je_zauzet == false)
                            slobodna_mesta_u_skladistu.push_back(s_tacke_skladista[i]);

                }
                int duzina_niza_slobodnih_mesta_u_skladistu = slobodna_mesta_u_skladistu.size();

                for(i=0; i<duzina_niza_slobodnih_mesta_u_skladistu; i++)
                    stanje_slobodnih_mesta_u_skladistu = stanje_slobodnih_mesta_u_skladistu || (izabrano_mesto_u_skladistu == slobodna_mesta_u_skladistu[i]);

                stanje_slobodnih_mesta_u_skladistu = (stanje_slobodnih_mesta_u_skladistu) && slobodno(izabrano_mesto_u_skladistu) == boolean_true;
                robot_resavac.add(stanje_slobodnih_mesta_u_skladistu);

                expr uslov_za_poziciju_izabranog_predmeta = boolean_false;
                    for(int i=0; i<duzina_niza_predmeta; i++)
                        if(domen.uzmi_s_tacku(scena->uzmi_predmet(i).uzmi_poziciju()).uzmi_indeks_b_tacke() == indeks_b_tacke_masine)
                            uslov_za_poziciju_izabranog_predmeta = uslov_za_poziciju_izabranog_predmeta || ((pozicija_izabranog_predmeta == scena->uzmi_predmet(i).uzmi_poziciju()) && (izabrani_predmet == i));
                robot_resavac.add(uslov_za_poziciju_izabranog_predmeta);

                //uslovi koji treba da budu ispunjeni za korak "nadji mesto"
                robot_resavac.add(predmet_je_obradjen(izabrani_predmet) == boolean_true);
                robot_resavac.add(slobodno(izabrano_mesto_u_skladistu) == boolean_true);

                //izabrano mesto u skladistu ne treba da blokira ostala prazna mesta u skladistu
                expr izabrano_mesto_ne_blokira_prazno_mesto = boolean_true;
                for(i=0; i<duzina_niza_slobodnih_mesta_u_skladistu; i++){
                    if(slobodna_mesta_u_skladistu[i] != izabrano_mesto_u_skladistu)
                        izabrano_mesto_ne_blokira_prazno_mesto = izabrano_mesto_ne_blokira_prazno_mesto && (blokira(izabrano_mesto_u_skladistu, robot_kontekst.int_val(slobodna_mesta_u_skladistu[i])) == boolean_false);
                }
                robot_resavac.add(izabrano_mesto_ne_blokira_prazno_mesto);
                robot_resavac.add(dodatni_uslov); //odsecanje - ako je pao na nekom od naredna dva koraka
                //ISPIS stream << robosint_resavac.to_smt2() << "\n";

                int izabrani_predmet_dobijen_iz_modela;
                int izabrano_mesto_u_skladistu_dobijeno_iz_modela;
                //ISPIS stream << robosint_resavac << "\n";
                int duzina_putanje_dobijena_iz_modela;

                switch (robot_resavac.check())
                {
                    case unknown:
                    case unsat: stream << "* korak NADJI MESTO je UNSAT *\n";
                            prvi_korak_je_zadovoljiv = false;
                            stream << "--------------------------------\n";
                            break;
                    case sat:   stream << "* korak NADJI MESTO je SAT *\n";
                            prvi_korak_je_zadovoljiv = true;
                            model dobijeni_model = robot_resavac.get_model();
                            //obilazak modela
                            for (unsigned i = 0; i < dobijeni_model.size(); i++) {
                                func_decl v = dobijeni_model[i];
                                if(v.name().str() == "izabrani_predmet")
                                    izabrani_predmet_dobijen_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                                if(v.name().str() == "izabrano_mesto_u_skladistu")
                                    izabrano_mesto_u_skladistu_dobijeno_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                            }
                            stream << "izabrani_predmet_dobijen_iz_modela: predmet " << izabrani_predmet_dobijen_iz_modela << "\n" << "izabrano_mesto_u_skladistu_dobijeno_iz_modela: s[" << izabrano_mesto_u_skladistu_dobijeno_iz_modela << "]\n";
                            stream << "trenutna predjena duzina putanje je: " << ukupna_duzina_predjene_putanje << "\n";
                    break;
                }
                robot_resavac.pop();

                if(prvi_korak_je_zadovoljiv) // ako je prvi korak nezadovoljiv, cela formula je nezadovoljiva
                {
                    robot_resavac.push();

                    //uslov za duzinu putanje
                    robot_resavac.add(ukupna_predjena_putanja == (ukupna_duzina_predjene_putanje + duzina_putanje));

                    //dodavanje modela dobijenog u prethodnom koraku
                    expr uslov_iz_prethodnih_koraka = (izabrani_predmet == izabrani_predmet_dobijen_iz_modela) && (izabrano_mesto_u_skladistu == izabrano_mesto_u_skladistu_dobijeno_iz_modela);
                    robot_resavac.add(uslov_iz_prethodnih_koraka);
                    bool drugi_korak_je_zadovoljiv = true;

                    /* preduslovi za korak "pokupi" */
                    //robot ne drzi nijedan predmet (dodavanje trenutnog stanja)
                    expr uslov_robot_drzi_predmet = boolean_false;
                    for(int i=0; i<duzina_niza_predmeta; i++)
                        uslov_robot_drzi_predmet = uslov_robot_drzi_predmet || ((robot_drzi_predmet(predmet) == (scena->uzmi_predmet(i).da_li_robot_drzi_predmet() ? boolean_true : boolean_false)) && (predmet == i));

                    //izabrani predmet nema puno mesto u masini za obradu koje ga blokira (dodavanje trenutnog stanja)
                    //cuva indeks s tacaka koje predstavljaju zauzeta mesta u masini
                    vector<int> zauzeta_mesta_u_masini;
                    for(i=0; i<duzina_niza_s_tacaka_masine; i++){
                        for(j=0; j<duzina_niza_predmeta; j++)
                            if(scena->uzmi_predmet(j).uzmi_poziciju() == s_tacke_masine[i])
                            {
                                zauzeta_mesta_u_masini.push_back(s_tacke_masine[i]);
                                break;
                            }
                    }
                    int duzina_niza_zauzetih_mesta_u_masini = zauzeta_mesta_u_masini.size();

                    //dodavanje uslova koji treba da budu ispunjeni za korak "pokupi"
                    //robot ne sme da drzi nijedan predmet
                    robot_resavac.add(uslov_robot_drzi_predmet == boolean_false);
                    int pozicija_robota_pre_koraka_pokupi = scena->uzmi_trenutnu_poziciju_robota();
                    vector<int> sekvenca_b_tacaka;

                    // postoji putanja od trenutne pozicije do b tacke skladista
                    //opis putanja je vec dodat, dodaju se samo uslovi koji treba da budu ispunjeni:
                    robot_resavac.add(trenutna_pozicija == pozicija_robota_pre_koraka_pokupi);
                    robot_resavac.add(putanja(trenutna_pozicija, b) == boolean_true);
                    robot_resavac.add(b == indeks_b_tacke_masine);

                    //izabrani predmet puno mesto u masini za obradu koje ga blokira
                    int s_tacka_masine_izabranog_predmeta = scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela).uzmi_poziciju();
                    expr ne_postoji_predmet_koji_blokira_izabrani_predmet = boolean_true;
                    for(i=0; i<duzina_niza_zauzetih_mesta_u_masini; i++)
                        ne_postoji_predmet_koji_blokira_izabrani_predmet = ne_postoji_predmet_koji_blokira_izabrani_predmet && (blokira(robot_kontekst.int_val(zauzeta_mesta_u_masini[i]), robot_kontekst.int_val(s_tacka_masine_izabranog_predmeta)) == boolean_false);

                    robot_resavac.add(ne_postoji_predmet_koji_blokira_izabrani_predmet);

                    //ISPIS stream << robosint_resavac.to_smt2() << "\n";
                    switch (robot_resavac.check())
                    {
                        case unknown:
                        case unsat: stream << "* korak POKUPI je UNSAT *\n";
                                    drugi_korak_je_zadovoljiv = false;
                                    dodatni_uslov = dodatni_uslov && ((izabrani_predmet != izabrani_predmet_dobijen_iz_modela) || (izabrano_mesto_u_skladistu != izabrano_mesto_u_skladistu_dobijeno_iz_modela));
                                    stream << "2. korak nezadovoljiv, povratak na prvi korak\n" ;
                                    stream << "--------------------------------\n";
                                    break;
                        case sat:   stream << "* korak POKUPI je SAT *\n";
                                model dobijeni_model = robot_resavac.get_model();
     //                        std::cout << dobijeni_model << std::endl;
                                drugi_korak_je_zadovoljiv = true;
                                // obilazak modela
                                for (unsigned i = 0; i < dobijeni_model.size(); i++) {
                                    func_decl v = dobijeni_model[i];
                                    if(v.name().str() == "duzina_putanje"){
                                        duzina_putanje_dobijena_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                                        ukupna_duzina_predjene_putanje += duzina_putanje_dobijena_iz_modela;
                                    }
                                }
                                sekvenca_b_tacaka = domen.uzmi_sekvencu_tacaka_za_par_b_tacaka(pozicija_robota_pre_koraka_pokupi, indeks_b_tacke_masine);
                                for (int i = 0; i < sekvenca_b_tacaka.size(); i++)
                                    scena->dodaj_stanje_scene(sekvenca_b_tacaka[i]);
                                scena->promeni_trenutnu_poziciju_robota(indeks_b_tacke_masine);
                                stream << "pozicija robota je promenjena \n" << "trenutna pozicija robota je b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << "\n";
                                stream << "do cvora b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << " je predjen put " << duzina_putanje_dobijena_iz_modela << "\n";
                                Predmet& p = scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela);
                                p.uhvati_predmet();
                                scena->dodaj_stanje_scene(scena->uzmi_trenutnu_poziciju_robota());
                                stream << "robot je pokupio predmet " << izabrani_predmet_dobijen_iz_modela <<  "\n";
                                stream << "trenutna predjena duzina putanje je: " << ukupna_duzina_predjene_putanje << "\n";
                                break;
                    }
                    robot_resavac.pop();
                    if(drugi_korak_je_zadovoljiv)
                    {
                        robot_resavac.push();

                        //uslov za duzinu putanje
                        robot_resavac.add(ukupna_predjena_putanja == (ukupna_duzina_predjene_putanje + duzina_putanje));

                        //dodavanje modela dobijenog u prethodnim koracima
                        robot_resavac.add(uslov_iz_prethodnih_koraka);

                        /*preduslovi koji treba da budu ispunjeni za korak "smesti" */
                        //robot se nalazi u b tacki masine za obradu (uzimanje trenutnog stanja)
                        int trenutna_pozicija_robota = scena->uzmi_trenutnu_poziciju_robota();
                        expr uslov_robot_je_u_b_tacki_masine = robot_je_u_b_tacki_masine == ((trenutna_pozicija_robota == indeks_b_tacke_masine) ? boolean_true : boolean_false);
                        robot_resavac.add(uslov_robot_je_u_b_tacki_masine);

                        //robot drzi izabrani predmet (uzimanje trenutnog stanja)
                        expr uslov_robot_drzi_izabrani_predmet = robot_drzi_izabrani_predmet == (scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela).da_li_robot_drzi_predmet() ? boolean_true : boolean_false);
                        robot_resavac.add(uslov_robot_drzi_izabrani_predmet);

                        //uslovi koji treba da budu ispunjeni za korak "smesti"
                        //postoji putanja od b tacke masine za obradu do skladista
                        //opis putanja je vec dodat, dodaju se samo uslovi koji treba da budu ispunjeni:
                        robot_resavac.add(trenutna_pozicija == indeks_b_tacke_masine);
                        robot_resavac.add(putanja(trenutna_pozicija, b) == boolean_true);
                        robot_resavac.add(b == indeks_b_tacke_skladista);

                        //robot se nalazi u b tacki izabranog predmeta
                        robot_resavac.add(robot_je_u_b_tacki_masine == boolean_true);

                        //robot drzi izabrani predmet
                        robot_resavac.add(robot_drzi_izabrani_predmet == boolean_true);

                        //ISPIS stream << robosint_resavac.to_smt2() << "\n";
                        Predmet& p = scena->uzmi_predmet(izabrani_predmet_dobijen_iz_modela);
                        switch (robot_resavac.check())
                        {
                            case unknown:
                            case unsat: stream << "* korak SMESTI je UNSAT *\n";
                                        dodatni_uslov = dodatni_uslov && ((izabrani_predmet != izabrani_predmet_dobijen_iz_modela) || (izabrano_mesto_u_skladistu != izabrano_mesto_u_skladistu_dobijeno_iz_modela));
                                        stream << "3. korak nezadovoljiv - vracanje na prethodne pozicije\n";
                                        p.spusti_predmet();
                                        scena->dodaj_stanje_scene(trenutna_pozicija_robota);
                                        stream << "robot je vratio predmet " << izabrani_predmet_dobijen_iz_modela <<  " na njegovu prethodnu poziciju\n";
        //                                scena->promeni_trenutnu_poziciju_robota(pozicija_robota_pre_koraka_pokupi);
                                        stream << "trenutna pozicija robota: b[" << scena->uzmi_trenutnu_poziciju_robota() << "]\n";
                                        stream << "--------------------------------\n";
                                        break;
                            case sat:   stream << "* korak SMESTI je SAT *\n";
                                        model dobijeni_model = robot_resavac.get_model();
        //                                std::cout << dobijeni_model << std::endl;
                                        // obilazak modela
                                        for (unsigned i = 0; i < dobijeni_model.size(); i++) {
                                            func_decl v = dobijeni_model[i];
                                            if(v.name().str() == "duzina_putanje"){
                                                duzina_putanje_dobijena_iz_modela = dobijeni_model.get_const_interp(v).get_numeral_int();
                                                ukupna_duzina_predjene_putanje += duzina_putanje_dobijena_iz_modela;
                                            }
                                        }
                                        sekvenca_b_tacaka = domen.uzmi_sekvencu_tacaka_za_par_b_tacaka(indeks_b_tacke_masine, indeks_b_tacke_skladista);
                                        for (int i = 0; i < sekvenca_b_tacaka.size(); i++)
                                            scena->dodaj_stanje_scene(sekvenca_b_tacaka[i]);
                                        scena->promeni_trenutnu_poziciju_robota(indeks_b_tacke_skladista);
                                        stream << "pozicija robota je promenjena \n" << "trenutna pozicija robota je b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << "\n";
                                        stream << "do cvora b[" << scena->uzmi_trenutnu_poziciju_robota() << "]" << " je predjen put " << duzina_putanje_dobijena_iz_modela << "\n";
                                        p.spusti_predmet();
                                        p.promeni_poziciju(izabrano_mesto_u_skladistu_dobijeno_iz_modela);
                                        scena->dodaj_stanje_scene(scena->uzmi_trenutnu_poziciju_robota());
                                        stream << "robot je spustio predmet " << izabrani_predmet_dobijen_iz_modela <<  " na poziciju s[" << izabrano_mesto_u_skladistu_dobijeno_iz_modela << "]\n";
                                        dodatni_uslov = boolean_true;
                                        stream << "robot je u b cvoru skladista, a on je: b[" << scena->uzmi_trenutnu_poziciju_robota() << "]\n";
                                        stream << "trenutna predjena duzina putanje je: " << ukupna_duzina_predjene_putanje << "\n";
                                        stream << "--------------------------------\n" ;
                            break;
                        }
                        robot_resavac.pop();
                    }

                }

                uslov_petlje = false;
                for (int i=0; i<duzina_niza_predmeta; i++)
                    uslov_petlje = uslov_petlje || ((domen.uzmi_s_tacku(scena->uzmi_predmet(i).uzmi_poziciju()).uzmi_indeks_b_tacke() != indeks_b_tacke_skladista) && !(scena->uzmi_predmet(i).da_li_ceka_obradu()));
            }
            stream << "Korak pakovanja predmeta u skladiste je: " << (prvi_korak_je_zadovoljiv ? "SAT" : "UNSAT") << "\n";
        }
        file.close();
    }

    return prvi_korak_je_zadovoljiv;
}
