#ifndef POMOC_H
#define POMOC_H

#include <QWidget>

namespace Ui {
class Pomoc;
}

class Pomoc : public QWidget
{
    Q_OBJECT

public:
    explicit Pomoc(QWidget *parent = 0);
    ~Pomoc();
private:
    Ui::Pomoc *ui;
};

#endif // POMOC_H
