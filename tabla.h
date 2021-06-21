#ifndef TABLA_H_INCLUDED
#define TABLA_H_INCLUDED

/**
 * \file: tabla.h
 * A játéktábla deklarációja és a figura típus megvalósítása
 */

#include <iostream>

#include "mystring.h"

/**
 * \brief Felsorolt típus a figurák (és a játékosok) színeinek.
 */
enum Szin {VILAGOS, SOTET};

/**
 * Visszatér egy enum Szin ellentettjével (VILAGOS / SOTET).
 * \param eredeti: a megadott szín
 * \return az ellentétes szín
 */
inline Szin szinEllentett(const Szin& eredeti) {
    return (eredeti == VILAGOS ? SOTET : VILAGOS);
}

/**
 * A szín szöveges nevét adja vissza ("Világos" / "Sötét").
 * \param eredeti: a szín
 * \return az szín szövegesen
 */
inline String szovegesSzin(const Szin& szin) {
    return (szin == VILAGOS ? String("Vilagos") : String("Sotet"));
}

/**
 * \brief A játék figuráit/korongjait megvalósító osztály
 */
class Figura {
private:
    Szin szin; /**< a figura színe */

public:
    /**
     * A figura konstruktora, beállítja a színét.
     * \param fSzin: a figura leendő színe
     */
    Figura(const Szin& fSzin)
        : szin(fSzin) {
    }

    /**
     * \brief Getter a figura színéhez
     * \return a figura színe
     */
    Szin getSzin() const {
        return szin;
    }

    /**
    * Egy 'V'/'S' karakterrel jeleníti meg a figurát.
    * \param os: az output stream, ahova kiírja a karaktert
    */
    void rajzol(std::ostream& os);
};

/**
 * Egyenlőség operátor, ami egy figura színének egyezőségét vizsgálja egy adott színnel.
 * \param fig: a figura
 * \param sz: a szín, amihez hasonlítjuk a figura színét
 * \return Egyezés esetén true, különben false
 */
inline bool operator==(const Figura& fig, const Szin& sz) { return sz == fig.getSzin(); }

/**
 * Egyenlőség operátor, ami egy figura színének
 *        különbözőségét vizsgálja egy adott színnel.
 * \param fig: a figura
 * \param sz: a szín, amihez hasonlítjuk a figura színét
 * \return Egyezés esetén false, különbözőség esetén true.
 */
inline bool operator!=(const Figura& fig, const Szin& sz) { return sz != fig.getSzin(); }

/**
 * \brief A malom játéktábláját megvalósító osztály
 */
class Tabla {
private:
    static int elek[32][2];             /**< A tábla élei rendezett éllistaként, 2D tömbben tárolva */
    static int malmok[48][2];           /**< A tábla malmai; minden i. mezőhöz a 2*i. és a 2*i+1. eleme tárolja a
                                         azoknak a malomhelyeknek a másik két elemét, amely malmokban az szerepel. */
    static String jelek;                /**< A mezők kódjai. 0-23-ig A-X. A 24. a '-' kötőjel, nem-ütéskor használjuk. */
    static const char rajz[25][7+1];    /**< A tábla karakteres rajza, a mezők helye választja el egymástól a részeit. */

    std::ostream& kimenet;              /**< Output stream, ahova a táblát rajzoljuk. */
    Figura* mezok[24];                  /**< 0-tól 23-ig indexelve tárolja a táblán levő figurák pointereit,
                                             üres mező esetén NULL szerepel. */

public:
    /**
     * Egy mezőindexhez megadja a karakterkódját, a -1-hez pedig a '-' kötőjelet rendeli.
     *        Túlindexelés esetén std::out_of_range kivételt dob.
     * \param i: az index (vagy -1)
     * \return a karakter (A-tól X-ig terjed), vagy kötőjel
     */
    static char pozToJel(int i);

    /**
     * Egy mezőkódot indexre konvertál, a '-' jelhez pedig a -1-et rendeli.
     *        Túlindexelés esetén std::out_of_range kivételt dob.
     * \param ch: a karakteres mezőkód (vagy '-')
     * \return a mező indexe (0-tól 23-ig terjed), vagy -1
     */
    static int jelToPoz(char ch);

    /**
     * Megadja a mezőkódok sztringjét.
     * \return konstans referencia a kódok sztringjére
     */
    static const String& getJelek();

    /**
     * Az osztályszintű malmok tömböt konstans módon indexelő függvény.
     * \param malomIdx: a malom sorszáma
     * \param mezoIdx: a mező sorszáma a malmon belül
     * \return a malomIdx-edik malom mezoIdx-edik mezője
     */
    static int malmokIndex(int malomIdx, int mezoIdx);

    /**
     * Megvizsgálja, hogy két, pozíciójukkal adott mező szomszédos-e a jétáktáblán.
     *        Túlindexeléskor std::out_of_range kivételt dob.
     * \param poz1: az egyik mező indexe
     * \param poz2: a másik mező indexe
     * \return Szomszédos mezők esetén true, különben false
     */
    static bool szomszedosak(int poz1, int poz2);

    /**
     * Jelmagyarázat; kirajolja a játéktáblát a mezők kódjaival a <kimenet> adatfolyamra.
     * \param os: szabványos output stream, ahova rajzol
     */
    static void rajzolJelek(std::ostream& os);

    /**
     * Kirajzolja a táblát a "kimenet"-re.
     * \param tabla: konstans referencia a kirajzolandó táblára
     */
    static void rajzolAllas(const Tabla& tabla);

    /**
     * A tábla konstruktora. Beállítja a rajzolás helyét (outputStream), és NULL-okkal tölti fel a mezők tömbjét.
     */
    Tabla(std::ostream& outputStream);

    /**
     * Másoló konstruktor
     * \param eredeti: a másik tábla, amelyről a másolat készül
     */
    Tabla(const Tabla& eredeti);

    /**
     * Értékadó operátor
     * \param eredeti: a másik tábla, amelyről a másolat készül
     * \return Referencia a felülírt táblára, hogy láncolható legyen az értékadás.
     */
    Tabla& operator=(const Tabla& eredeti);

    /**
     * Destruktor, felszabadítja a tábla nem NULL mezőit.
     */
    ~Tabla();

    /**
     * A tábla index operátorai, a mezőket teszik elérhetővé.
     */
    Figura*& operator[](int poz);   // A játékmester majd a private részében tárolja a játéktáblát, így a játékosok
    const Figura* operator[](int poz) const;      // csak a konstans index operátorral férhetnek hozzá a figurákhoz.

    /**
     * Megmadja, hogy az adott pozíciójú mezőn szerepel-e olyan figura, amely éppen malomban van.
     * \param poz: a mező indexe
     * \return Igaz/hamis attól függően, hogy malom része-e a mező.
     */
    bool malomResze(int poz) const;

    /**
     * Megvizsgálja, hogy a megadott színű játékos korongjai között van-e olyan, amelyik épp nincs malomban.
     * \param aktSzin: a játékos figurái színe
     * \return Pontosan akkor igaz, ha mindegyik figura valamely malom részét alkotja.
     */
    bool mindMalombanVan(const Szin& aktSzin) const;

    /**
     * Megvizsgálja, hogy a megadott pozíción levő  figura (ha van)
     * tud-e szomszédos üres mezőre lépni.
     * \param poz: a pozíció/mezőindex
     * \return Pontosan akkor igaz, ha van ott figura, és tud szomszédos mezőre lépni.
     */
    bool vanUresSzomszedja(int poz) const;

    /**
     * Megvizsgálja, hogy a megadott színű játékos tud-e szabályos tolást végezni
     * (szomszédos mezőre) valamely korongjával.
     * \param aktSzin: a játékos színe
     * \return Pontosan akkor igaz, ha nincs figura, amelyikkel tudna szomszédos mezőre lépni.
     *         Ha nincs adott színű figura, akkor is false-szal tér vissza.
     */
    bool beszorult(const Szin& aktSzin) const;
};

#endif // TABLA_H_INCLUDED
