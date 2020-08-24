#ifndef PLANIRANJE_H
#define PLANIRANJE_H

#include "domen.h"
#include "scena.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <QFile>
#include <QTextStream>
#include <memory>
#include <z3++.h>

class Planiranje
{
public:
    Planiranje(std::shared_ptr<Domen> _domen, std::shared_ptr<Scena> _scena, int _maksimalna_duzinja_putanje);
    bool generisi_plan();
private:
    std::shared_ptr<Domen> domen;
    std::shared_ptr<Scena> scena;
    int maksimalna_duzinja_putanje;
};

#endif // PLANIRANJE_H
