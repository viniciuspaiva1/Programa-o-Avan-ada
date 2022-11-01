#include "mainlojaqt.h"
#include "ui_mainlojaqt.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

MainLojaQT::mainlojaQT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainLojaQT)
{
    ui->setupUi(this);
    ui->label_livro->setStyleSheet("background-color: lightgray");
}

MainLojaQT::~MainLojaQT()
{
    delete ui;
}


/*void MainLojaQT::on_tabela_livros_cellDoubleClicked(int row, int column)
{

}


void MainLojaQT::on_tabela_cds_cellDoubleClicked(int row, int column)
{

}


void MainLojaQT::on_tabela_dvds_cellDoubleClicked(int row, int column)
{

}*/


void MainLojaQT::on_actionSair_triggered()
{
   QCoreApplication::quit();
}

/*
void MainLojaQT::on_actionLer_triggered()
{

    QString fileName = QFileDialog::
        getOpenFileName(this,
                        "Ler Estoque",
                        QString(),
                        "Text Files (*.txt);;C++ Files (*.cpp *.h)");

    if (!fileName.isEmpty()) {
      QFile file(fileName);
      if (!file.open(QIODevice::ReadOnly)) {
        // error message
        QMessageBox::critical(this, tr("Error"), tr("Could not open file to read"));
        return;
      }
      QTextStream in(&file);
      //ui->textEdit->setText(in.readAll());
      file.close();
    }
}*/

