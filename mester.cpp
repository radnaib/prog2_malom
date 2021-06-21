/**
 * \file: mester.cpp
 * A játékmester tagfüggvényeinek és modulszintű függvényeinek megvalósítása
 */

#include <algorithm>
#include <cstdlib>
#include <cctype>

#include "memtrace.h"
#include "mester.h"

std::istream& Mester::bemenet = std::cin;
std::ostream& Mester::kimenet = std::cout;

namespace {

    /// A játékmester üzenetei az egyes lépések előtt:
    const char lepesUzenetek[6][200+1] = {
        "Add meg, hogy melyik mezore teszed a kovetkezo figurad! (A-X): \n",
        "Add meg, hogy melyik figuraddal szeretnel lepni! (A-X): \n",
        "Add meg, hogy hova szeretned csusztatni! (A-X): \n",
        "Add meg, hogy melyik figuraddal szeretnel ugrani! (A-X): \n",
        "Add meg, hogy hova szeretnel ugrani vele! (A-X): \n",
        "Add meg, hogy az ellenfel melyik figurajat szeretned levenni a tablarol!\n"
            "Ha nem szeretned levenni egyiket sem, azt kotojellel jelold! (A-X vagy -): \n"
    };


    /// A jatekInit() segédfüggvényei:

    /// A két játékost és a nevüket állítja be.
    void jatekInitJatekosok(std::istream& in, std::ostream& out,
                            Jatekos*& pJatekos1, Jatekos*& pJatekos2) {
        String j1Nev;
        out << "Udvozollek a malom jatekban! Ird be a neved: " << std::endl;
        in >> std::ws;
        getline(in, j1Nev);
        pJatekos1 = new Felhasznalo(j1Nev, in);

        out << "A gep ellen (1), vagy egy jatekostarsad ellen (2) szeretnel jatszani? (1/2): " << std::endl;
        int jatekmod;
        in >> jatekmod;
        if (jatekmod == 1) {
            pJatekos2 = new Bot("Boti");
            out << "Ellenfeled " << pJatekos2->getNev() << ", a robot." << std::endl;
        }
        else {
            String j2Nev;
            out << pJatekos1->getNev() << ", ird be az ellenfeled nevet: " << std::endl;
            in >> std::ws;
            getline(in, j2Nev);
            pJatekos2 = new Felhasznalo(j2Nev, in);
        }
    }

    /** Beállítja, hogy ki kezdjen. Az első felhasználó dönthet arról,
     hogy ő kezdjen, a másik, vagy a gép sorsolja. */
    void jatekInitKezdo(std::istream& in, std::ostream& out,
                        bool& j1Kezd, const String& j1Nev, const String& j2Nev) {
        out << j1Nev << ", ki kezd? Te (1), " << j2Nev
            << " (2), vagy sorsoljuk (3)? (1/2/3): ";
        int kezdesmod;
        in >> kezdesmod;

        if (kezdesmod == 1)
            j1Kezd = true;
        else if (kezdesmod == 2)
            j1Kezd = false;
        else {
            j1Kezd = (rand()%2 == 0 ? true : false);
            out << "A sorsolas eredmenye: ";
        }

        out << (j1Kezd ? j1Nev : j2Nev) << " kezd." << std::endl;
    }
}

/// Lokális függvények a nevek lekérdezésére.
namespace {
    /** A lépés elején, a soron következő szín alapján megadja,
     *  annak a játékosnak a pointerét, akinek a lepes függvényét meg kell hívni. */
    Jatekos const* aktJatekos(bool j1Kezd, const Szin& aktSzin,
                              Jatekos const* pJatekos1, Jatekos const* pJatekos2) {
        return ((j1Kezd && aktSzin == VILAGOS) || (!j1Kezd && aktSzin == SOTET))
                ? pJatekos1 : pJatekos2;
    }

    /** Az aktuális játékos nevével tér vissza (szín alapján). */
    String aktJatekosNev(bool j1Kezd, const Szin& aktSzin,
                             const String& j1Nev, const String& j2Nev) {
        return ((j1Kezd && aktSzin == VILAGOS) || (!j1Kezd && aktSzin == SOTET))
                ? j1Nev : j2Nev;
    }

    /** A játék eredményének kiírása előtt határozza meg mindkét játékos nevét, szín szerint. */
    void jatekNeveketBeallit(bool j1Kezd, const Szin& aktSzin,
                             const String& j1Nev, const String& j2Nev,
                             String& jAktNev, String& jEllenfelNev) {
        if ((j1Kezd && aktSzin == VILAGOS) || (!j1Kezd && aktSzin == SOTET)) {
            jAktNev = j1Nev;
            jEllenfelNev = j2Nev;
        }
        else {
            jAktNev = j2Nev;
            jEllenfelNev = j1Nev;
        }
    }
}

void Mester::jatekInit(std::istream& in, std::ostream& out) {
    jatekInitJatekosok(in, out, pJatekos1, pJatekos2);
    jatekInitKezdo(in, out, j1Kezd, pJatekos1->getNev(), pJatekos2->getNev());
    aktSzin = VILAGOS;

    naploStream << "Uj jatek indul. A jatekosok:" << std::endl;
    naploStream << pJatekos1->getNev() << " vs " << pJatekos2->getNev() << std::endl;
    naploStream << aktJatekosNev(j1Kezd, aktSzin, pJatekos1->getNev(), pJatekos2->getNev())
                << " kezd (Vilagos)" << std::endl;
    naploStream << std::setfill('-') << std::setw(50) << ""
                << std::setfill(' ') << std::endl;

    pTabla = new Tabla(out);
    figuraDbKezben = 9;
    vilagosDbTablan = 0;
    sotetDbTablan = 0;
    utolsoUtes = 0;
}

/// Lokális függvények a lépések bekéréséhez és ellenőrzéséhez.
namespace {

    /** A táblán az aktuális játékos- illetve az ellenfél korongjainak a számára
     *  ad referenciát. */
    int& aktDbTablan(const Szin& aktSzin, int& vilagosDbTablan, int& sotetDbTablan) {
        return (aktSzin == VILAGOS ? vilagosDbTablan : sotetDbTablan);
    }
    int& ellentettDbTablan(const Szin& aktSzin, int& vilagosDbTablan, int& sotetDbTablan) {
        return (aktSzin == VILAGOS ? sotetDbTablan : vilagosDbTablan);
    }

    /** A lepesOke segédfüggvénye, ellenőrzi, hogy levehető-e a melyiket által jelölt figura. */
    bool elveszMelyiketOke(const Tabla& tabla, const Szin& aktSzin, int melyiket) {
        if (melyiket == -1)
            return true;

        Figura const * const melyikFigurat = tabla[melyiket];
        if (melyikFigurat == NULL)
            return false;

        if (*melyikFigurat == aktSzin)
            return false;

        if (tabla.mindMalombanVan(szinEllentett(aktSzin)))
            return true;

        return (!tabla.malomResze(melyiket));
    }

    /** Ellenőrzi, hogy szabályos-e a tipus lépéstípusú lépés. */
    bool lepesOke(const Tabla& tabla, const Szin& aktSzin,
                  const LepesTipus& tipus, int& honnan, int& hova) {
        const Figura* mezoHonnan = (honnan == -1 ? NULL : tabla[honnan]);
        const Figura* mezoHova = (hova == -1 ? NULL : tabla[hova]);

        switch(tipus) {
            case LETESZ:
                return (hova != -1 && mezoHova == NULL);
                break;

            case TOLAS_HONNAN:
                return (honnan != -1 && mezoHonnan != NULL
                        && *mezoHonnan == aktSzin && tabla.vanUresSzomszedja(honnan));
                break;

            case TOLAS_HOVA:
                return (hova != -1 && mezoHova == NULL && Tabla::szomszedosak(honnan, hova));
                break;

            case UGRAS_HONNAN:
                return (honnan != -1 && mezoHonnan != NULL && *mezoHonnan == aktSzin);
                break;

            case UGRAS_HOVA:
                return (hova != -1 && tabla[hova] == NULL);
                break;

            case ELVESZ:
                return elveszMelyiketOke(tabla, aktSzin, honnan);
                break;

            default:
                return false;
                break;
        }
    }

    /** Balértékként használt függvény. Megadja, hogy az éppen beolvasott lépés
     *  a tolt figura/ugró figura/elvett figura pozícióját adja-e meg (honnan), vagy pedig a
     *  letevés/tolás/ugrás célmezőjét, hogy a megfelelőt olvashassuk be.  */
    int& honnanVagyHova(const LepesTipus& tipus, int& honnan, int& hova) {
        if (tipus == TOLAS_HONNAN || tipus == UGRAS_HONNAN || tipus == ELVESZ)
            return honnan;
        return hova;
    }

    /** Bekéri az aktuálisan lépő játékostól a lépését (honnan vagy hova, a lépéstől függően). */
    void lepestBeolvas(std::ostream& out, std::ostream& naploStream,
                       const Tabla& tabla, Jatekos const* const& pJatekos,
                       const Szin& aktSzin, const LepesTipus& tipus, int& honnan, int& hova) {
        out << lepesUzenetek[tipus];
        if (tipus != TOLAS_HOVA && tipus != UGRAS_HOVA)
            honnan = -1;
        hova = -1;

        int& poz = honnanVagyHova(tipus, honnan, hova);
        poz = pJatekos->lepes(out, tabla, aktSzin, tipus, honnan);
        while (!lepesOke(tabla, aktSzin, tipus, honnan, hova)) {
            out << "Szabalytalan a lepesed. Kerlek, ird be ujra! ( A-X / - ): ";
            naploStream << "Szabalytalan lepes (" << Tabla::pozToJel(poz) << ")" << std::endl;
            poz = pJatekos->lepes(out, tabla, aktSzin, tipus, honnan);
        }
    }
}

void Mester::tolasVagyUgras(Jatekos const* const& pjAkt, const LepesTipus& tipusHonnan,
                            const LepesTipus& tipusHova, int& honnan, int& hova) {
    std::ostream& out = Mester::kimenet;

    lepestBeolvas(out, naploStream, *pTabla, pjAkt, aktSzin, tipusHonnan, honnan, hova);
    lepestBeolvas(out, naploStream, *pTabla, pjAkt, aktSzin, tipusHova, honnan, hova);

    (*pTabla)[hova] = (*pTabla)[honnan];
    (*pTabla)[honnan] = NULL;

    naploStream << (tipusHonnan == TOLAS_HONNAN ? "Tolas: " : "Ugras: ");
    naploStream << Tabla::pozToJel(honnan) << " -> "
                << Tabla::pozToJel(hova) << std::endl;
}

void Mester::jatekLepes() {
    std::ostream& out = Mester::kimenet;
    Jatekos const* const pjAkt = aktJatekos(j1Kezd, aktSzin, pJatekos1, pJatekos2);
    const String& aktJNev = aktJatekosNev(j1Kezd, aktSzin, pJatekos1->getNev(), pJatekos2->getNev());
    out << aktJNev << ", te lepsz (" << szovegesSzin(aktSzin) << ")!" << std::endl;
    naploStream << aktJNev << " lep (" << szovegesSzin(aktSzin) << ")." << std::endl;

    int honnan, hova;

    /// Figura letevése | tolás szomszédos mezőre | ugrás:
    if (figuraDbKezben >= 1) {
        lepestBeolvas(out, naploStream, *pTabla, pjAkt, aktSzin, LETESZ, honnan, hova);

        (*pTabla)[hova] = new Figura(aktSzin);
        ++aktDbTablan(aktSzin, vilagosDbTablan, sotetDbTablan);

        if (aktSzin == SOTET)
            --figuraDbKezben;

        naploStream << "Letesz: " << Tabla::pozToJel(hova) << std::endl;
        naploStream << szovegesSzin(aktSzin) << " figurainak szama kezben: ";
        naploStream << (aktSzin == VILAGOS ? figuraDbKezben-1 : figuraDbKezben)
                    << " db" << std::endl;
    }
    else  {
        ++utolsoUtes;

        if (aktDbTablan(aktSzin, vilagosDbTablan, sotetDbTablan) >= 4)
            tolasVagyUgras(pjAkt, TOLAS_HONNAN, TOLAS_HOVA, honnan, hova);
        else
            tolasVagyUgras(pjAkt, UGRAS_HONNAN, UGRAS_HOVA, honnan, hova);
    }

    /// Figura (opcionális) leütése:
    if (pTabla->malomResze(hova)) {
        Tabla::rajzolJelek(out);
        Tabla::rajzolAllas(*pTabla);

        lepestBeolvas(out, naploStream, *pTabla, pjAkt, aktSzin, ELVESZ, honnan, hova);

        if (honnan != -1) {
            delete (*pTabla)[honnan];
            (*pTabla)[honnan] = NULL;

            utolsoUtes = 0;
            --ellentettDbTablan(aktSzin, vilagosDbTablan, sotetDbTablan);
        }

        naploStream << "Elvesz: " << Tabla::pozToJel(honnan) << std::endl;
    }

    naploStream << "Vilagos figurainak szama a tablan: "
                << vilagosDbTablan << " db" << std::endl;
    naploStream << "Sotet figurainak szama a tablan: "
                << sotetDbTablan << " db" << std::endl;
    naploStream << utolsoUtes/2 << " lepespar ota nem tortent utes" << std::endl << std::endl;
}

void Mester::allapotFrissites() {
    int aktDbTablan = (aktSzin == VILAGOS ? vilagosDbTablan : sotetDbTablan);
    if (figuraDbKezben == 0) {
        if (pTabla->beszorult(aktSzin))
            allapot = BESZORULT;

        else if (aktDbTablan <= 2)
            allapot = ELFOGYOTT;

        else if (utolsoUtes >= 26)
            allapot = DONTETLEN;
    }
}

namespace {

    /** Kiírja a beállított kimenetre a játszma eredményét. */
    void jatekEredmenyKiiras(std::ostream& out, std::ostream& naploStream,
                             const Allapot& allapot, const Tabla& tabla,
                             const String& jAktNev, const String& jEllenfelNev) {
        out << "A vegallapot:" << std::endl;
        Tabla::rajzolAllas(tabla);

        switch(allapot) {
            case BESZORULT:
                out << jAktNev << " jatekosnak nem maradt tobb lepese, "
                    << jEllenfelNev << " nyert!" << std::endl;
                naploStream << jAktNev << " beszorult. "
                    << jEllenfelNev << " nyert." << std::endl;
                break;

            case ELFOGYOTT:
                out << jAktNev << "jatekosnak kevesebb, mint harom figuraja maradt a tablan, "
                    << jEllenfelNev << " nyert!" << std::endl;
                naploStream << jAktNev << " figurai elfogytak. "
                    << jEllenfelNev << " nyert!" << std::endl;
                break;

            case DONTETLEN:
                out << "Tizenharom lepespar eltelt leteves es utes nelkul, "
                    << "igy dontetlen a jatszma." << std::endl;
                naploStream << "Dontetlen a 13 lepeses szabaly alapjan." << std::endl;
                break;

            default:
                break;
        }

        naploStream << std::setfill('-') << std::setw(50) << ""
                    << std::setfill(' ') << std::endl;
    }

    /** A jatek függvény segédfüggvénye; teljesen új játékot indít, ha a felhasználó azt írja be. */
    bool legyenUjJatek(std::istream& in, std::ostream& out, const String& j1Nev) {
        out << j1Nev << ", szeretnel visszavagot? (I/N): ";
        char chUj;
        in >> chUj;
        return (toupper(chUj) == 'I' || chUj == '\n');
    }
}

void Mester::jatek() {
    std::istream& in = Mester::bemenet;
    std::ostream& out = Mester::kimenet;

    bool ujJatek = true;
    while (ujJatek) {
        jatekInit(in, out);

        allapot = JATEK;
        while (allapot == JATEK) {
            Tabla::rajzolJelek(out);
            Tabla::rajzolAllas(*pTabla);
            jatekLepes();

            aktSzin = szinEllentett(aktSzin);
            allapotFrissites();
        }

        String jAktNev, jEllenfelNev;
        jatekNeveketBeallit(j1Kezd, aktSzin, pJatekos1->getNev(),
                            pJatekos2->getNev(), jAktNev, jEllenfelNev);
        jatekEredmenyKiiras(out, naploStream, allapot, *pTabla, jAktNev, jEllenfelNev);

        ujJatek = legyenUjJatek(in, out, pJatekos1->getNev());
        if (ujJatek) {
            delete pJatekos1;
            delete pJatekos2;
            delete pTabla;
        }
    }
}

Mester::~Mester() {
    delete pJatekos1;
    delete pJatekos2;
    delete pTabla;
}
