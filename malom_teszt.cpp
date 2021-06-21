/**
 * \file: malom_teszt.cpp
 * Egyszerű tesztmodul a malom játékhoz.
 * Ki lehet próbálni benne a játékot, és egy próbajátékban is tesztelhető a működés.
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

#include "memtrace.h"
#include "mester.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main() {
    srand(time(0));

    try {
        char jatekValasztas = 'n';
        cout << "Ird be, hogy magat a jatekot (I)" << endl
             << "vagy pedig a tesztjatekot (N)" << endl
             << "szeretned inditani (I/N): ";
        cin >> jatekValasztas;
        cout << endl;

        switch(jatekValasztas) {
            case 'n':
            case 'N':
                {
                    cout << "Indul a tesztjatek." << endl
                         << "A jatekmester a malom.txt fajlban naplozza az esemenyeket." << endl << endl;
                    std::ofstream log;
                    log.open("malom.txt", std::ofstream::out);
                    Mester mesterLog(log);
                    mesterLog.jatek();
                    log.close();
                    break;
                }

            default:
                Mester mester(std::cout);
                mester.jatek();
                break;
        }
    }
    catch(std::exception& err) {
        cerr << err.what() << endl;
    }
    catch(...) {
        cerr << "Ismeretlen kivetel erkezett!" << endl;
    }

    return 0;
}
