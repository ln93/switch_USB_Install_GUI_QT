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
    connect(&threadinstall,SIGNAL(Send_ProgressBar(int)),this,SLOT(GetProgressBar(int)));
    ui->InstallprogressBar->setVisible(0);
    ui->label_2->setVisible(0);
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
    if(nsppath.size()==0)
    {
        GetCout(QString::fromUtf8("尚未选择nsp文件!"));
        return;
     }
    emit emitpath(nsppath);
     ui->InstallprogressBar->setVisible(1);
     ui->label_2->setVisible(1);
}
 void MainWindow::GetCout(QString COUT)
 {
     ui->CoutLabel->setText(COUT);
 }
 void MainWindow::GetProgressBar(int progress)
 {
     ui->InstallprogressBar->setValue(progress);
 }
