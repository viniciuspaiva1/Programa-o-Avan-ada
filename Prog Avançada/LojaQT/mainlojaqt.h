#ifndef MAINLOJAQT_H
#define MAINLOJAQT_H

#include <QMainWindow>
#include "loja.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainLojaQT; }
QT_END_NAMESPACE

class MainLojaQT : public QMainWindow
{
    Q_OBJECT

public:
    MainLojaQT(QWidget *parent = nullptr);
    ~MainLojaQT();

/*private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tabela_livros_cellDoubleClicked(int row, int column);

    void on_tabela_cds_cellDoubleClicked(int row, int column);

    void on_tabela_dvds_cellDoubleClicked(int row, int column);*/

private slots:
    void on_actionSair_triggered();

    void on_actionLer_triggered();

private:
    Ui::MainLojaQT *ui;
    Loja store;
};
#endif // MAINLOJAQT_H
