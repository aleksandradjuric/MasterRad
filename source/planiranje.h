#ifndef PLANIRANJE_H
#define PLANIRANJE_H

#include "domen.h"
#include "scena.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <QFile>
#include <QTextStream>

class Planiranje
{
public:
    Planiranje(Domen _domen, Scena* _scena, int _maksimalna_duzinja_putanje);
    bool generisi_plan();
private:
    Domen domen;
    Scena* scena;
    int maksimalna_duzinja_putanje;
};

#endif // PLANIRANJE_H
