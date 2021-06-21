/**
 * \file: jatekos.cpp
 * A játékosok nem inline függvényeinek definíciói
 */

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <ctime>

#include "memtrace.h"
#include "jatekos.h"

int Felhasznalo::lepes(std::ostream& os, const Tabla& tabla,
                       const Szin& aktSzin, const LepesTipus& tipus, int honnan) const {
    char mezoJel;
    Felhasznalo::bemenet >> mezoJel;
    mezoJel = toupper(mezoJel);

    while (strchr(Tabla::getJelek().c_str(), mezoJel) == NULL) {
        os << "Ervenytelen jelet irtal be, ismeteld meg ( A-X / - ): \n";
        Felhasznalo::bemenet >> mezoJel;
        mezoJel = toupper(mezoJel);
    }
    return Tabla::jelToPoz(mezoJel);
}


enum Mezo {U, V, S}; /**< Felsorolt típus Üres | Világos | Sötét mezők egyszerűbb reprezentálásához. */

namespace {
    /** A következő négy függvény a többi függvényt egyszerűsíti.
    Megkeresik az adott pozíciót tartalmazó malomhelyek mezőit, és egyszerűbben kezelhetővé alakítják. */

    Mezo szinToMezo(const Szin& szin) {
        if (szin == VILAGOS)
            return V;
        else
            return S;
    }

    Mezo figuraToMezo(Figura const * const& pFigura) {
        if (pFigura == NULL)
            return U;
        else if (*pFigura == VILAGOS)
            return V;
        else
            return S;
    }

    void intCsere(int& i1, int& i2) {
        int temp = i1;
        i1 = i2;
        i2 = temp;
    }

    void malomMezok(const Tabla& tabla, int poz, int honnan,
                    Mezo& v1, Mezo& v2, Mezo& f1, Mezo& f2) {
        int vPoz1 = Tabla::malmokIndex(2*poz, 0);
        int vPoz2 = Tabla::malmokIndex(2*poz, 1);
        int fPoz1 = Tabla::malmokIndex(2*poz+1, 0);
        int fPoz2 = Tabla::malmokIndex(2*poz+1, 1);

        if (fPoz1 == honnan || fPoz2 == honnan) {
            intCsere(vPoz1, fPoz1);
            intCsere(vPoz2, fPoz2);
        }
        if (vPoz2 == honnan)
            intCsere(vPoz1, vPoz2);

        v1 = figuraToMezo(tabla[vPoz1]);
        v2 = figuraToMezo(tabla[vPoz2]);
        f1 = figuraToMezo(tabla[fPoz1]);
        f2 = figuraToMezo(tabla[fPoz2]);
    }


    /** A következő hat függvény számítja a bot lépéseit, típusonként eltérő feltételrendszerrel,
     *  prioritások használatával. */

    int botLetesz(const Tabla& tabla, const Szin& aktSzin, int honnan) {
        int prior[24] = {0};
        int maxPrior = 0;
        int maxPriorDb = 24;

        for (int poz = 0; poz < 24; ++poz) {
            if (tabla[poz] != NULL)
                prior[poz] = 0;

            else {
                Mezo akt = szinToMezo(aktSzin);
                Mezo ellen = szinToMezo(szinEllentett(aktSzin));
                Mezo v1, v2, f1, f2;
                malomMezok(tabla, poz, honnan, v1, v2, f1, f2);

                int aktDb = (v1 == akt ? 1 : 0) + (v2 == akt ? 1 : 0)
                               + (f1 == akt ? 1 : 0) + (f2 == akt ? 1 : 0);
                int ellenDb = (v1 == ellen ? 1 : 0) + (v2 == ellen ? 1 : 0)
                               + (f1 == ellen ? 1 : 0) + (f2 == ellen ? 1 : 0);

                if ((v1 == akt && v2 == akt) || (f1 == akt && f2 == akt))
                    prior[poz] = 6;
                else if ((v1 == ellen && v2 == ellen) || (f1 == ellen && f2 == ellen))
                    prior[poz] = 5;
                else if (aktDb >= 2)
                    prior[poz] = 4;
                else if (ellenDb >= 2)
                    prior[poz] = 3;
                else if (aktDb >= ellenDb)
                    prior[poz] = 2;
                else
                    prior[poz] = 1;
            }

            if (prior[poz] > maxPrior) {
                maxPrior = prior[poz];
                maxPriorDb = 1;
            }
            else if (prior[poz] == maxPrior)
                ++maxPriorDb;
        }

        int valasztottMax = rand()%maxPriorDb + 1;
        int db = 0, poz = 0;
        while (db < valasztottMax && poz < 24) {
            if (prior[poz] == maxPrior)
                ++db;
            ++poz;
        }
        --poz;

        return poz;
    }

    /** Ez a függvény összetettebb a többinél. Külön megvizsgálja azt, hogy
    van-e olyan mező, ahova tud saját figurát tolni, és azzal malmot készít vagy akadályoz meg. */
    int botTolasHonnan(const Tabla& tabla, const Szin& aktSzin, int honnan) {
        int prior[24] = {0};

        for (int poz = 0; poz < 24; ++poz) {
            if (tabla[poz] == NULL) {
                Mezo akt = szinToMezo(aktSzin);
                Mezo ellen = szinToMezo(szinEllentett(aktSzin));

                int vPoz1 = Tabla::malmokIndex(2*poz, 0);
                int vPoz2 = Tabla::malmokIndex(2*poz, 1);
                int fPoz1 = Tabla::malmokIndex(2*poz+1, 0);
                int fPoz2 = Tabla::malmokIndex(2*poz+1, 1);

                Mezo v1 = figuraToMezo(tabla[vPoz1]);
                Mezo v2 = figuraToMezo(tabla[vPoz2]);
                Mezo f1 = figuraToMezo(tabla[fPoz1]);
                Mezo f2 = figuraToMezo(tabla[fPoz2]);

                if ((v1 == akt && v2 == akt)) {
                    if (f1 == akt)
                        prior[fPoz1] = 6;
                    if (f2 == akt)
                        prior[fPoz2] = 6;
                }
                else if ((f1 == akt && f2 == akt)) {
                    if (v1 == akt)
                        prior[vPoz1] = 6;
                    if (v2 == akt)
                        prior[vPoz2] = 6;
                }

                if ((v1 == ellen && v2 == ellen)) {
                    if (f1 == akt)
                        prior[fPoz1] = 5;
                    if (f2 == akt)
                        prior[fPoz2] = 5;
                }
                else if ((f1 == ellen && f2 == ellen)) {
                    if (v1 == akt)
                        prior[vPoz1] = 5;
                    if (v2 == akt)
                        prior[vPoz2] = 5;
                }
            }
        }

        for (int poz = 0; poz < 24; ++poz) {
            if (prior[poz] == 0) {
                Mezo akt = szinToMezo(aktSzin);
                Mezo ellen = szinToMezo(szinEllentett(aktSzin));
                Mezo v1, v2, f1, f2;
                malomMezok(tabla, poz, honnan, v1, v2, f1, f2);

                int aktDb = (v1 == akt ? 1 : 0) + (v2 == akt ? 1 : 0)
                                + (f1 == akt ? 1 : 0) + (f2 == akt ? 1 : 0);
                int ellenDb = (v1 == ellen ? 1 : 0) + (v2 == ellen ? 1 : 0)
                                + (f1 == ellen ? 1 : 0) + (f2 == ellen ? 1 : 0);

                if (f1 == ellen && f2 == ellen)
                    prior[poz] = 1;
                else if (ellenDb >= 2)
                    prior[poz] = 2;
                else if (aktDb <= 1)
                    prior[poz] = 3;
                else if (aktDb >= 3)
                    prior[poz] = 4;
            }
        }

        for (int poz = 0; poz < 24; ++poz) {
            if (tabla[poz] == NULL || *tabla[poz] != aktSzin
                || !tabla.vanUresSzomszedja(poz))
                prior[poz] = 0;
        }

        int maxPrior = 0;
        int maxPriorDb = 24;
        for (int poz = 0; poz < 24; ++poz) {
            if (prior[poz] > maxPrior) {
                maxPrior = prior[poz];
                maxPriorDb = 1;
            }
            else if (prior[poz] == maxPrior)
                ++maxPriorDb;
        }

        int valasztottMax = rand()%maxPriorDb + 1;
        int db = 0, poz = 0;
        while (db < valasztottMax && poz < 24) {
            if (prior[poz] == maxPrior)
                ++db;
            ++poz;
        }
        --poz;

        return poz;
    }

    int botTolasHova(const Tabla& tabla, const Szin& aktSzin, int honnan) {
        int prior[24] = {0};
        int maxPrior = 0;
        int maxPriorDb = 24;

        for (int poz = 0; poz < 24; ++poz) {
            if (tabla[poz] != NULL || !Tabla::szomszedosak(honnan, poz))
                prior[poz] = 0;

            else {
                Mezo akt = szinToMezo(aktSzin);
                Mezo ellen = szinToMezo(szinEllentett(aktSzin));
                Mezo v1, v2, f1, f2;
                malomMezok(tabla, poz, honnan, v1, v2, f1, f2);

                int aktDb = (v1 == akt ? 1 : 0) + (v2 == akt ? 1 : 0)
                               + (f1 == akt ? 1 : 0) + (f2 == akt ? 1 : 0);
                int ellenDb = (v1 == ellen ? 1 : 0) + (v2 == ellen ? 1 : 0)
                               + (f1 == ellen ? 1 : 0) + (f2 == ellen ? 1 : 0);

                if ((f1 == akt && f2 == akt))
                    prior[poz] = 6;
                else if ((v1 == ellen && v2 == ellen))
                    prior[poz] = 5;
                else if (aktDb >= 2)
                    prior[poz] = 4;
                else if (ellenDb >= 2)
                    prior[poz] = 3;
                else if (aktDb >= ellenDb)
                    prior[poz] = 2;
                else
                    prior[poz] = 1;
            }

            if (prior[poz] > maxPrior) {
                maxPrior = prior[poz];
                maxPriorDb = 1;
            }
            else if (prior[poz] == maxPrior)
                ++maxPriorDb;
        }

        int valasztottMax = rand()%maxPriorDb + 1;
        int db = 0, poz = 0;
        while (db < valasztottMax && poz < 24) {
            if (prior[poz] == maxPrior)
                ++db;
            ++poz;
        }
        --poz;

        return poz;
    }

    int botUgrasHonnan(const Tabla& tabla, const Szin& aktSzin, int honnan) {
        int prior[24] = {0};
        int maxPrior = 0;
        int maxPriorDb = 24;

        for (int poz = 0; poz < 24; ++poz) {
            if (tabla[poz] == NULL || *tabla[poz] != aktSzin)
                prior[poz] = 0;

            else {
                Mezo akt = szinToMezo(aktSzin);
                Mezo ellen = szinToMezo(szinEllentett(aktSzin));
                Mezo v1, v2, f1, f2;
                malomMezok(tabla, poz, honnan, v1, v2, f1, f2);

                int aktDb = (v1 == akt ? 1 : 0) + (v2 == akt ? 1 : 0)
                               + (f1 == akt ? 1 : 0) + (f2 == akt ? 1 : 0);
                int ellenDb = (v1 == ellen ? 1 : 0) + (v2 == ellen ? 1 : 0)
                               + (f1 == ellen ? 1 : 0) + (f2 == ellen ? 1 : 0);

                if ((v1 == ellen && v2 == ellen) || (f1 == ellen && f2 == ellen))
                    prior[poz] = 1;
                else if (aktDb >= 2)
                    prior[poz] = 2;
                else if (ellenDb >= 2)
                    prior[poz] = 3;
                else if (ellenDb >= 1)
                    prior[poz] = 4;
                else
                    prior[poz] = 5;
            }

            if (prior[poz] > maxPrior) {
                maxPrior = prior[poz];
                maxPriorDb = 1;
            }
            else if (prior[poz] == maxPrior)
                ++maxPriorDb;
        }

        int valasztottMax = rand()%maxPriorDb + 1;
        int db = 0, poz = 0;
        while (db < valasztottMax && poz < 24) {
            if (prior[poz] == maxPrior)
                ++db;
            ++poz;
        }
        --poz;

        return poz;
    }

    int botUgrasHova(const Tabla& tabla, const Szin& aktSzin, int honnan) {
        int prior[24] = {0};
        int maxPrior = 0;
        int maxPriorDb = 24;

        for (int poz = 0; poz < 24; ++poz) {
            if (tabla[poz] != NULL || poz == honnan)
                prior[poz] = 0;

            else {
                Mezo akt = szinToMezo(aktSzin);
                Mezo ellen = szinToMezo(szinEllentett(aktSzin));
                Mezo v1, v2, f1, f2;
                malomMezok(tabla, poz, honnan, v1, v2, f1, f2);

                int aktDb = (v1 == akt ? 1 : 0) + (v2 == akt ? 1 : 0)
                               + (f1 == akt ? 1 : 0) + (f2 == akt ? 1 : 0);
                int ellenDb = (v1 == ellen ? 1 : 0) + (v2 == ellen ? 1 : 0)
                               + (f1 == ellen ? 1 : 0) + (f2 == ellen ? 1 : 0);

                if ((v1 == akt && v2 == akt) || (f1 == akt && f2 == akt))
                    prior[poz] = 6;
                else if ((v1 == ellen && v2 == ellen) || (f1 == ellen && f2 == ellen))
                    prior[poz] = 5;
                else if (aktDb >= 2)
                    prior[poz] = 4;
                else if (ellenDb >= 2)
                    prior[poz] = 3;
                else if (aktDb + ellenDb <= 1)
                    prior[poz] = 2;
                else
                    prior[poz] = 1;
            }

            if (prior[poz] > maxPrior) {
                maxPrior = prior[poz];
                maxPriorDb = 1;
            }
            else if (prior[poz] == maxPrior)
                ++maxPriorDb;
        }

        int valasztottMax = rand()%maxPriorDb + 1;
        int db = 0, poz = 0;
        while (db < valasztottMax && poz < 24) {
            if (prior[poz] == maxPrior)
                ++db;
            ++poz;
        }
        --poz;

        return poz;
    }

    /** Ha a bot látja, hogy beszorult az ellenfele, akkor nem veszi le korongot.
     *  Egyébként folytatja a prioritások számolását, hasonlóan a többi lépéstípushoz. */
    int botElvesz(const Tabla& tabla, const Szin& aktSzin, int honnan) {
        if (tabla.beszorult(szinEllentett(aktSzin)))
            return -1;

        int prior[24] = {0};
        int maxPrior = 0;
        int maxPriorDb = 24;

        for (int poz = 0; poz < 24; ++poz) {
            if (tabla[poz] == NULL || *tabla[poz] == aktSzin ||
                (!tabla.mindMalombanVan(szinEllentett(aktSzin)) && tabla.malomResze(poz)))
                prior[poz] = 0;

            else {
                Mezo akt = szinToMezo(aktSzin);
                Mezo ellen = szinToMezo(szinEllentett(aktSzin));
                Mezo v1, v2, f1, f2;
                malomMezok(tabla, poz, honnan, v1, v2, f1, f2);

                int aktDb = (v1 == akt ? 1 : 0) + (v2 == akt ? 1 : 0)
                               + (f1 == akt ? 1 : 0) + (f2 == akt ? 1 : 0);
                int ellenDb = (v1 == ellen ? 1 : 0) + (v2 == ellen ? 1 : 0)
                               + (f1 == ellen ? 1 : 0) + (f2 == ellen ? 1 : 0);

                if ((v1 == akt && v2 == akt) || (f1 == akt && f2 == akt))
                    prior[poz] = 6;
                else if ((v1 == ellen && v2 == ellen) || (f1 == ellen && f2 == ellen))
                    prior[poz] = 5;
                else if (aktDb >= 2)
                    prior[poz] = 4;
                else if (ellenDb >= 2)
                    prior[poz] = 3;
                else if (aktDb >= ellenDb)
                    prior[poz] = 2;
                else
                    prior[poz] = 1;
            }

            if (prior[poz] > maxPrior) {
                maxPrior = prior[poz];
                maxPriorDb = 1;
            }
            else if (prior[poz] == maxPrior)
                ++maxPriorDb;
        }

        int valasztottMax = rand()%maxPriorDb + 1;
        int db = 0, poz = 0;
        while (db < valasztottMax && poz < 24) {
            if (prior[poz] == maxPrior)
                ++db;
            ++poz;
        }
        --poz;

        return poz;
    }
}

int Bot::lepes(std::ostream& os, const Tabla& tabla,
               const Szin& aktSzin, const LepesTipus& tipus, int honnan) const {

    typedef int (*BotLepesFptr)(const Tabla&, const Szin&, int);
    BotLepesFptr botLepesek[6] = { botLetesz, botTolasHonnan, botTolasHova,
                                   botUgrasHonnan, botUgrasHova, botElvesz };
    return botLepesek[tipus](tabla, aktSzin, honnan);
}
