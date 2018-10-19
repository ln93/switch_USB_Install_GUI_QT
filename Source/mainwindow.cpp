#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QFileDialog>
#include<QDir>
#include <QFile>
#include <main0.h>
QStringList nsppath;
main0 threadinstall;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this,SIGNAL(emitpath(QStringList)),&threadinstall,SLOT(installall(QStringList)));
    connect(&threadinstall,SIGNAL(Send_COUT(QString)),this,SLOT(GetCout(QString)));
    threadinstall.start();
}

MainWindow::~MainWindow()
{

    threadinstall.~main0();
        delete ui;
}

void MainWindow::on_fopenpushButton_clicked()
{
    nsppath=QFileDialog::getOpenFileNames(this,tr("打开NSP文件"),"./",tr("switch安装文件 (*.nsp)"));
    QString tmp;
    for(int i=0;i<nsppath.size();i++)
        tmp+=nsppath.at(i)+"\n";
    tmp.chop(1);
    ui->label->setText(tmp);

}

void MainWindow::on_RuninstallpushButton_clicked()
{
    emit emitpath(nsppath);
}
 void MainWindow::GetCout(QString COUT)
 {
     ui->CoutLabel->setText(COUT);
 }
