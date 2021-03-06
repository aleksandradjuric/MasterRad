#ifndef PRIKAZ_PLANA_H
#define PRIKAZ_PLANA_H

#include <QWidget>
#include <memory>

namespace Ui {
class Prikaz_plana;
}

class Prikaz_plana : public QWidget
{
    Q_OBJECT

public:
    explicit Prikaz_plana(QWidget *parent = 0);
    ~Prikaz_plana();
    void postavi_tekst_plana(QString tekst);
private:
    std::shared_ptr<Ui::Prikaz_plana> ui;
};

#endif // PRIKAZ_PLANA_H
