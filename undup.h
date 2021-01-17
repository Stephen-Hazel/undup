#ifndef UNDUP_H
#define UNDUP_H

#include "../stv/ui.h"
#include <QApplication>
#include <QMainWindow>
#include "ui_undup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UnDup; }
QT_END_NAMESPACE


class UnDup: public QMainWindow {
   Q_OBJECT   
private:
   Ui::UnDup *ui;

public:
   UnDup (QWidget *par = nullptr)
   : QMainWindow (par), ui (new Ui::UnDup)
   {  ui->setupUi (this);  }

  ~UnDup ()  {delete ui;}

   void Open (), Shut ();

public slots:
   void Dir ();
   void Go ();
};

#endif // UNDUP_H
