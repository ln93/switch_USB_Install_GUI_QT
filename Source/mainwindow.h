#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void GetCout(QString COUT);
    void GetProgressBar(int progress);
private slots:
    void on_fopenpushButton_clicked();

    void on_RuninstallpushButton_clicked();


signals:
    void emitpath(QStringList path);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
