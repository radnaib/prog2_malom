#ifndef MYSTRING_H
#define MYSTRING_H

/**
 * \file: mystring.h
 * A String osztály interfészét tartalmazza.
 * Az 5. heti laboron elkészített osztály átalakított változata.
 */

#include <iostream>

/**
 * String osztály nullterminált dinamikus karaktertömbök tárolására.
 */
class String {
    static size_t kapacNov;  /**< a kapacitás növekménye */


    char* pData;        /**< pointer az adatra */
    size_t len;         /**< hossz lezáró nulla nélkül */
    size_t kapac;       /**< maximális kapacitás lezáró nulla nélkül */

public:
    /// Paraméter nélküli konstruktor:
    String();

    /// Konstruktor egy karakterből
    /// @param ch - karakter, amiből az új String-et létrehozza
    String(const char ch);

    /// Konstruktor nullterminált karaktertömbből
    /// @param ch - nullterminált karaktertömb, amiből az új String-et létrehozza
    String(const char* str);

    /// Másoló konstruktor
    /// @param konstans referencia a másolandó String-re
    String(const String& theOther);

    /// Destruktor
    ~String() { delete[] pData; }

    /// operator=
    /// @param konstans referencia a jobb oldali String-re
    /// @return referencia az új értéket kapott String-re
    String& operator=(const String& rhs);

    /// Sztring hosszát adja vissza.
    /// @return sztring tényleges hossza (lezáró nulla nélkül).
    size_t size() const { return len; }

    /// Sztring kapacitását adja vissza.
    /// @return sztring maximális kapacitása (lezáró nulla nélkül).
    size_t capacity() const { return kapac; }

    /// Indexoperátor
    /// Túlindexelés esetén std::out_of_range kivételt dob.
    /// @param idx - a nemnegatív index
    /// @return referencia az index által elért karakterre
    char& operator[](size_t idx);

    /// Konstans indexoperátor
    /// Túlindexelés esetén std::out_of_range kivételt dob.
    /// @param idx - a nemnegatív index
    /// @return referencia az index által elért karakterre
    const char& operator[](size_t idx) const;

    /// String-hez jobbról karaktert fűz
    /// @param rhs - a meglévő String-hez jobbról hozzáfűzendő karakter
    /// @return az új, összefűzött String
    String& append(const char ch);

    /// C-sztringet ad vissza
    /// @return pointer a tárolt, vagy azzal azonos tartalmú nullával lezárt sztring-re.
    const char* c_str() const { return pData; }
};

/// Inserter operátor String-ek kiírásához
/// @param os - std::ostream típusú objektum, ahová a String-et írja
/// @param rhs - konstans referencia a kiírandó String-re
/// @return referencia az std::ostream objektumra a láncolhatóságért
std::ostream& operator<<(std::ostream& os, const String& rhs);

/// Extractor operátor String-ek beolvasásához
/// @param is - std::istream típusú objektum, ahonnan a String-et olvassa
/// @param rhs - referencia a beolvasandó String-re
/// @return referencia az std::istream objektumra a láncolhatóságért
std::istream& operator>>(std::istream& is, String& rhs);

/// Teljes sort String-be beolvasó függvény.
/// @param is - std::istream típusú objektum, ahonnan a String-et olvassa
/// @param rhs - referencia a String-re, ahova olvassuk a sort
void getline(std::istream& is, String& str);

#endif
