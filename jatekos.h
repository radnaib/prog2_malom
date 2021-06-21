#ifndef JATEKOS_H_INCLUDED
#define JATEKOS_H_INCLUDED

/**
 * \file: jatekos.h
 * A malom játékostípusainak (játékos <- felhasználó / bot) deklarációi
 */

#include <iostream>

#include "tabla.h"
#include "mystring.h"

/**
 * \brief Felsorolt típus az egyes lépések elkülönítésére.
 *        A játék első fázisában LETESZ-, a másodikban TOLAS_HONNAN- és TOLAS_HOVA-,
 *        a harmadikban pedig UGRAS_HONNAN- és UGRAS_HOVA lepesek következhetnek.
 *        Bármelyik lépés után ELVESZ lépés is jön, ha malom keletkezett.
 */
enum LepesTipus {LETESZ, TOLAS_HONNAN, TOLAS_HOVA, UGRAS_HONNAN, UGRAS_HOVA, ELVESZ};

/**
 * \brief Absztrakt ősosztály a malom játékosaihoz
 */
class Jatekos {
protected:
    String nev; /**< a játékos neve, tartalmazhat szóközöket is */

public:
    /**
     * Az ősosztály konstruktora
     * \param jNev: String, a játékos leendő nevét tartalmazza
     */
    Jatekos(const String& jNev)
        : nev(jNev) {
    }

    /**
     * Getter a játékos nevéhez
     * \return a játékos neve, String
     */
    String getNev() const {
        return nev;
    }

    /**
     * Virtuális függvény, ami a játékosok lépés függvényeit határozza meg.
     * \param os: output adatfolyam, ahol a játékmester ír a felhasználóknak
     * \param tabla: konstans referencia a táblára, amin lépni kell
     * \param aktSzin: az aktuális körben lépő játékos színe
     * \param tipus: a lépés típusa
     * \param honnan: az előző lépésben megjelölt pozíció (honnan-hova párok esetén használjuk)
     * \return A mező kódja, amit a játékos a lépésében megjelölt (ELVESZ esetén -1 is lehet).
     */
    virtual int lepes(std::ostream& os, const Tabla& tabla,
                      const Szin& aktSzin, const LepesTipus& tipus, int honnan) const = 0;

    /**
     * Virtuális destruktor
     */
    virtual ~Jatekos() {
    }
};

/**
 * \brief Felhasználó, azaz élő ember játékos, a Jatekos osztály leszármazottja.
 *        Feladata a játékos nevét tárolni, és konzolon bekérni a lépéseit.
 */
class Felhasznalo : public Jatekos {
private:
    std::istream& bemenet; /**< szabványos input adafolyam, ahova a felhasználó a lépéseit írja */

public:
    /**
     * Konstruktor, beállítja az input adatfolyamot, és a felhasználó nevét.
     */
    Felhasznalo(const String& fNev, std::istream& inputStream)
        : Jatekos(fNev), bemenet(inputStream) {
    }

    /**
     * A felhasználó lépését bekérő függvény.
     * Konzolablakból várja a karakterkódot, amit ellenőriz, és int-té alakít.
     * \param os: output adatfolyam, ahol a játékmester ír a felhasználóknak
     * \param tabla: konstans referencia a táblára, amin lépni kell
     * \param aktSzin: az aktuális körben lépő játékos színe
     * \param tipus: a lépés típusa
     * \param honnan: az előző lépésben megjelölt pozíció (honnan-hova párok esetén használjuk)
     * \return A mező kódja, amit a játékos a lépésében megjelölt (ELVESZ esetén -1 is lehet).
     */
    int lepes(std::ostream& os, const Tabla& tabla,
              const Szin& aktSzin, const LepesTipus& tipus, int honnan) const;
};

/**
 * \brief Robotot/számítógépet megvalósító Jatekos-leszármazott.
 *        Algoritmikusan számítja a lépések helyét.
 */
class Bot : public Jatekos {
public:
    /**
     * Konstruktor, beállítja a bot nevét.
     */
    Bot(const String& bNev)
        : Jatekos(bNev) {
    }
    /**
     * A bot lépését számító függvény. Az egyes lépéstípusoknál különböző feltételekkel,
     * de alapvetően hasonlóan számol. A lehetséges pozícióknak prioritásértéket ad,
     * majd a maximális prioritású opciók közül véletlenszerűen választ.
     * Odafigyel a malmok kihasználására, az ellenfél malmainak megakadályozására,
     * illetve a lehetőségei csökkentésére; ennél több stratégiája nincsen.
     *
     * \param os: output adatfolyam, ahol a játékmester ír a felhasználóknak
     * \param tabla: konstans referencia a táblára, amin lépni kell
     * \param aktSzin: az aktuális körben lépő játékos színe
     * \param tipus: a lépés típusa
     * \param honnan: az előző lépésben megjelölt pozíció (honnan-hova párok esetén használjuk)
     * \return A mező kódja, amit a játékos a lépésében megjelölt (ELVESZ esetén -1 is lehet).
     */
    int lepes(std::ostream& os, const Tabla& tabla,
              const Szin& aktSzin, const LepesTipus& tipus, int honnan) const;
};

#endif // JATEKOS_H_INCLUDED
