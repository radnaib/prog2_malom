/**
 * \file: tabla.cpp
 * A játéktábla és a figura statikus tagjainak és a tagfüggvényeinek a definíciói
 */

#include <stdexcept>

#include "memtrace.h"
#include "mystring.h"
#include "tabla.h"

void Figura::rajzol(std::ostream& os) {
    os << (szin == VILAGOS ? 'V' : 'S');
}

int Tabla::elek[32][2] = {
    {0, 1}, {0, 9}, {1, 2}, {1, 4},
    {2, 14}, {3, 4}, {3, 10}, {4, 5},
    {4, 7}, {5, 13}, {6, 7}, {6, 11},
    {7, 8}, {8, 12}, {9, 10}, {9, 21},
    {10, 11}, {10, 18}, {11, 15}, {12, 13},
    {12, 17}, {13, 14}, {13, 20}, {14, 23},
    {15, 16}, {16, 17}, {16, 19}, {18, 19},
    {19, 20}, {19, 22}, {21, 22}, {22, 23}
};

int Tabla::malmok[48][2] = {
    {1, 2}, {9, 21}, {0, 2}, {4, 7}, {0, 1}, {14, 23},
    {4, 5}, {10, 18}, {3, 5}, {1, 7}, {3, 4}, {13, 20},
    {7, 8}, {11, 15}, {6, 8}, {1, 4}, {6, 7}, {12, 17},
    {10, 11}, {0, 21}, {9, 11}, {3, 18}, {9, 10}, {6, 15},
    {13, 14}, {8, 17}, {12, 14}, {5, 20}, {12, 13}, {2, 23},
    {16, 17}, {6, 11}, {15, 17}, {19, 22}, {15, 16}, {8, 12},
    {19, 20}, {3, 10}, {18, 20}, {16, 22}, {18, 19}, {5, 13},
    {22, 23}, {0, 9}, {21, 23}, {16, 19}, {21, 22}, {2, 14}
};

String Tabla::jelek = String("ABCDEFGHIJKLMNOPQRSTUVWX-"); // '-' jelentése: nem kíván ütni

const String& Tabla::getJelek() {
    return jelek;
}

const char Tabla::rajz[25][7+1] = {
    "\n",
    "-----", "-----",
    "\n| ", "---", "---",
    " |\n| | ", "-", "-", " | |\n",
    "-", "-", "   ", "-", "-",
    "\n| | ", "-", "-", " | |\n| ",
    "---", "---", " |\n",
    "-----", "-----",
    "\n\n"
};

char Tabla::pozToJel(int i) {
    if (i == -1)
        return Tabla::jelek[24];

    else if (i < 0 || i >= 24)
        throw std::invalid_argument("Tabla intToJel: ervenytelen mezokod.");
    return Tabla::jelek[i];
}

int Tabla::jelToPoz(char ch) {
    int poz = 0;
    while (Tabla::jelek[poz] != ch)
        ++poz;

    if (poz == 24)
        return -1;

    if (poz >= 25)
        throw std::invalid_argument("Tabla jelToInt: ervenytelen mezojel.");
    return poz;
}

int Tabla::malmokIndex(int malomIdx, int mezoIdx) {
    if (malomIdx < 0 || malomIdx >= 48 || mezoIdx < 0 || mezoIdx >= 2)
        throw std::invalid_argument("Tabla malmokIndex: tulindexeles.");

    return Tabla::malmok[malomIdx][mezoIdx];
}

/// Lokális segédfüggvények élek kereséséhez
namespace {
    void intCsere(int& i1, int& i2) {
        int temp = i1;
        i1 = i2;
        i2 = temp;
    }

    int elekHasonlit(int cs1, int cs2, int poz1, int poz2) {
        if (cs1 < poz1)
            return -1;
        else if (cs1 > poz1)
            return +1;
        else {
            if (cs2 < poz2)
                return -1;
            else if (cs2 > poz2)
                return +1;
            else
                return 0;
        }
    }
}

bool Tabla::szomszedosak(int poz1, int poz2) {
    if (poz1 < 0 || poz1 >= 24 || poz2 < 0 || poz2 >= 24)
        throw std::invalid_argument("Tabla::szomszedosak: ervenytelen mezokod");

    if (poz1 > poz2)
        intCsere(poz1, poz2);

    // Bináris keresést alkalmaz.
    int also = 0, felso = 31;
    int kozepso = (also + felso)/2;
    int cs1 = Tabla::elek[kozepso][0];
    int cs2 = Tabla::elek[kozepso][1];
    int relacio = elekHasonlit(cs1, cs2, poz1, poz2);

    while (also <= felso && relacio != 0) {
        if (relacio > 0)
            felso = kozepso - 1;
        else if (relacio < 0)
            also = kozepso + 1;

        kozepso = (also + felso)/2;
        cs1 = Tabla::elek[kozepso][0];
        cs2 = Tabla::elek[kozepso][1];
        relacio = elekHasonlit(cs1, cs2, poz1, poz2);
    }

    return (relacio == 0);
}

void Tabla::rajzolJelek(std::ostream& kimenet) {
    kimenet << "A tabla mezoinek a kodjai:" << std::endl;
    for (int i = 0; i < 24; ++i)
        kimenet << Tabla::rajz[i] << Tabla::jelek[i];
    kimenet << Tabla::rajz[24];
}

void Tabla::rajzolAllas(const Tabla& tabla) {
    tabla.kimenet << "A jatek allasa:" << std::endl;
    for (int i = 0; i < 24; ++i) {
        tabla.kimenet << Tabla::rajz[i];
        if (tabla.mezok[i] == NULL)
            tabla.kimenet << ' ';
        else
            tabla.mezok[i]->rajzol(tabla.kimenet);
    }
    tabla.kimenet << Tabla::rajz[24];
}

/// Lokális segédfüggvények az általános tagfüggvények törzseinek kompaktabbá tételéhez
namespace {
    void tablaMasolas(Tabla& masolat, const Tabla& eredeti) {
        for (int i = 0; i < 24; ++i) {
            if (eredeti[i] != NULL)
                masolat[i] = new Figura(eredeti[i]->getSzin());
        }
    }

    void tablaTorles(Tabla& torlendo) {
        for (int i = 0; i < 24; ++i) {
            if (torlendo[i] != NULL)
                delete torlendo[i];
        }
    }
}

Tabla::Tabla(std::ostream& outputStream)
    : kimenet(outputStream) {
    for (int i = 0; i < 24; ++i)
        mezok[i] = NULL;
}

Tabla::Tabla(const Tabla& eredeti)
    : kimenet(eredeti.kimenet) {
    tablaMasolas(*this, eredeti);
}

Tabla& Tabla::operator=(const Tabla& eredeti) {
    if (this != &eredeti) {
        tablaTorles(*this);
        tablaMasolas(*this, eredeti);
    }
    return *this;
}

Tabla::~Tabla() {
    tablaTorles(*this);
}

Figura*& Tabla::operator[](int poz) {
    if (poz < 0 || poz >= 24)
        throw std::invalid_argument("Tabla index operator: ervenytelen mezokod.");
    return mezok[poz];
}

const Figura* Tabla::operator[](int poz) const {
    if (poz < 0 || poz >= 24)
        throw std::invalid_argument("Tabla const index operator: ervenytelen mezokod.");
    return mezok[poz];
}

bool Tabla::malomResze(int poz) const {
    if (poz < 0 || poz >= 24)
        throw std::invalid_argument("Tabla::malomResze: ervenytelen mezokod.");

    if (mezok[poz] == NULL)
        return false;

    const Szin aktSzin = mezok[poz]->getSzin();

    // Vízszintesen
    Figura* vMezo2 = mezok[ Tabla::malmok[2*poz][0] ];
    Figura* vMezo3 = mezok[ Tabla::malmok[2*poz][1] ];
    if (vMezo2 != NULL && *vMezo2 == aktSzin &&
        vMezo3 != NULL && *vMezo3 == aktSzin)
        return true;

    // Függőlegesen
    Figura* fMezo2 = mezok[ Tabla::malmok[2*poz + 1][0] ];
    Figura* fMezo3 = mezok[ Tabla::malmok[2*poz + 1][1] ];
    if (fMezo2 != NULL && *fMezo2 == aktSzin &&
        fMezo3 != NULL && *fMezo3 == aktSzin)
        return true;

    return false;
}

bool Tabla::mindMalombanVan(const Szin& aktSzin) const {
    for (int poz = 0; poz < 24; ++poz) {
        if (mezok[poz] != NULL && *mezok[poz] == aktSzin) {
            if (!malomResze(poz))
                return false;
        }
    }
    return true;
}

bool Tabla::vanUresSzomszedja(int poz) const {
    for (int i = 0; i < 32; ++i) {
        int poz1 = Tabla::elek[i][0];
        int poz2 = Tabla::elek[i][1];
        if (poz1 == poz || poz2 == poz) {
            if (poz1 != poz)
                intCsere(poz1, poz2);

            if (mezok[poz] != NULL && mezok[poz2] == NULL)
                return true;
        }
    }

    return false;
}

bool Tabla::beszorult(const Szin& aktSzin) const {
    for (int el = 0; el < 32; ++el) {
        Figura* vegpont1 = mezok[ Tabla::elek[el][0] ];
        Figura* vegpont2 = mezok[ Tabla::elek[el][1] ];
        if ((vegpont1 == NULL && (vegpont2 != NULL && *vegpont2 == aktSzin)) ||
            (vegpont2 == NULL && (vegpont1 != NULL && *vegpont1 == aktSzin)))
            return false;
    }
    return true;
}
