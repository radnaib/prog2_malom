/**
 * \file mystring.cpp
 * A String osztály nem inline tagfüggvényeinek a definícióit tartalmazza.
 */

#ifdef _MSC_VER
// MSC ne adjon figyelmeztető üzenetet a C sztringkezelő függvényeire
  #define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstring>
#include <cctype>

#include "memtrace.h"
#include "mystring.h"

size_t String::kapacNov = 8;

String::String() {
    len = 0;
    kapac = String::kapacNov;
    pData = new char[kapac + 1];
    pData[0] = '\0';

}

String::String(const char ch) {
    len = 1;
    kapac = String::kapacNov;
    pData = new char[kapac + 1];
    pData[0] = ch;
    pData[1] = '\0';

}

String::String(const char* str) {
    len = strlen(str);
    kapac = len + String::kapacNov;
    pData = new char[kapac + 1];
    strcpy(pData, str);
}

String::String(const String& theOther) {
    len = theOther.len;
    pData = new char[len+1];
    strcpy(pData, theOther.pData);
}

String& String::operator=(const String& rhs) {
    if (this != &rhs) {
        if (kapac < rhs.len) {
            delete[] pData;
            kapac = rhs.len + String::kapacNov;
            pData = new char[kapac + 1];
        }
        len = rhs.len;
        strcpy(pData, rhs.pData);
    }
    return *this;
}

char& String::operator[](size_t idx) {
    if (idx >= len)
        throw std::out_of_range("(String) Tulindexeles!");
    return pData[idx];
}

const char& String::operator[](size_t idx) const {
    if (idx >= len)
        throw std::out_of_range("(String) Tulindexeles!");
    return pData[idx];
}

String& String::append(char ch) {
    if (kapac < len + 1) {
        kapac = len + 1 + String::kapacNov;
        char* uj = new char[kapac + 1];
        strcpy(uj, pData);
        delete[] pData;
        pData = uj;
    }
    pData[len] = ch;
    pData[len + 1] = '\0';
    len += 1;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const String& rhs) {
    os << rhs.c_str();
    return os;
}

std::istream& operator>>(std::istream& is, String& rhs) {
    String inp;
    char c;
    while (is.get(c) && !isspace(c))
        inp.append(c);
    rhs = inp;
    return is;
}

void getline(std::istream& is, String& str) {
    String inp;
    char c;
    is >> std::noskipws;
    while (is >> c && c != '\n')
        inp.append(c);
    is >> std::skipws;
    str = inp;
}
