#ifndef MESTER_H_INCLUDED
#define MESTER_H_INCLUDED

/**
 * \file: mester.h
 * A malom játékmesterének deklarációja
 */

#include <iostream>

#include "jatekos.h"
#include "tabla.h"

/**
 * \brief A játékmenetek állapotai. Csak a JATEK állapotban folytatódik a játék.
 */
enum Allapot {JATEK, BESZORULT, ELFOGYOTT, DONTETLEN};

/**
 * \brief Játékmester osztály.
 *
 * Felelős a játékosok és a tábla kezeléséért (dinamikusan foglalja őket,
 * megszűnésekor őket is megszünteti), és a malom szabályai szerinti játék lebonyolításáért.
 */
class Mester {
private:
    static std::istream& bemenet;       /**< Bemeneti adatfolyam, ahonnan a felhasználók lépéseit várja. */
    static std::ostream& kimenet;       /**< Kimeneti adatfolyam, oda rajzol(tat)ja ki a táblát,
                                             és oda küldi az üzeneteit a játékosoknak a lépéseikről. */
    std::ostream& naploStream;          /**< Kimeneti adatfolyam a lépések naplózásához. */

    Mester(const Mester&);              /**< Mester objektumok másolása nem illik a modellbe, ezért van letiltva. */
    Mester& operator=(const Mester&);   /**< Hasonan az értékadás is. */

    Jatekos* pJatekos1;     /**< A fő játékos, aki csak felhasználó lehet. Ő dönt a játék beállításairól. */
    Jatekos* pJatekos2;     /**< A másik játékos, lehet ember is, robot is. */
    bool j1Kezd;            /**< Pontosan akkor igaz, ha az első játékos kezdi a játékot. */
    Tabla* pTabla;          /**< A játéktábla */

    Szin aktSzin;           /**< Az aktuálisan soron következő/lépő játékos színe. Világos kezdi a játékot. */
    int figuraDbKezben;     /**< A figurák száma a játékosoknál külön-külön (kezdetben 9) */
    int vilagosDbTablan;    /**< Világos táblán levő figuráinak a száma */
    int sotetDbTablan;      /**< A táblán levő sötét figurák száma */
    int utolsoUtes;         /**< A legutolsó ütés óta megtett egyéni lépések száma (nem lépéspárok) */
    Allapot allapot;        /**< A játék mindenkori állapota */

    /**
     * A játék(menet) beállításait elvégző függvény.
     * \param in: a bemeneti stream
     * \param out: a kimeneti stream
     */
    void jatekInit(std::istream& in, std::ostream& out);

    /**
     * A jatekLepes() függvény egyik segédfüggvénye. A tolásokat és az ugrásokat fogja össze.
     * \param pjAkt: pointer az aktuálisan lépő játékosra
     * \param tipusHonnan: TOLAS_HONNAN vagy UGRAS_HONNAN lehet.
     * \param tipusHonnan: TOLAS_HOVA vagy UGRAS_HOVA lehet, megfelelve tipusHonnan-nak
     * \param honnan: a mező indexe, amelyen levő figurával lépünk
     * \param hova: a mező indexe, ahova a figurával lépünk
     */
    void tolasVagyUgras(Jatekos const* const& pjAkt, const LepesTipus& tipusHonnan,
                        const LepesTipus& tipusHova, int& honnan, int& hova);

    /**
     * Megvalósítja egy felhasználó körét, az opcionális ütéssel/figura elvétellel együtt.
     */
    void jatekLepes();

    /**
     * A játék állapotát frissíti egy lépés utáni színcserét követően.
     * (Elfogytak-e a játékos bábui, beszorult-e, döntetlen-e az állás ütések hiányában,
     *  vagy pedig folytatódhat a játék).
     */
    void allapotFrissites();

public:
    /**
     * A játékmester default konstruktora.
     */
    Mester(std::ostream& naplo) : naploStream(naplo) {
    }

    /**
     * Destruktor. A - dinamiusan foglalt - játékosokat és táblát szabadítja fel.
     */
    ~Mester();

    /**
     * A játékmester legfontosabb függvénye, magát a játékot valósítja meg.
     */
    void jatek();
};

#endif // MESTER_H_INCLUDED
