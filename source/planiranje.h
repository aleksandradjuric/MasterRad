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
    Planiranje(Domen _domen, Scena* _scena);
    bool generisi_plan();
private:
    Domen domen;
    Scena* scena;
};

#endif // PLANIRANJE_H
