#ifndef MAIN0_H
#define MAIN0_H
#include "QThread"
#include "QObject"

#include<stdio.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include "lusb0_usb.h"
#include <io.h>
#include <vector>
#include <stdint.h>

using namespace std;
class main0 : public QThread
{
    Q_OBJECT
public:
    main0();
    ~main0();
    QStringList nspnames;
    bool GoInstall;
    bool stopped;
signals:
    void Send_COUT(QString COUT);
    void Send_ProgressBar(int progress);



public slots:
    void installall(QStringList path);
    void installsingle(QString path);
protected:
    void run();

private:
    int usb_write(usb_dev_handle *dev, char *tmp, int len);
    int usb_write(usb_dev_handle *dev, unsigned char *tmp, int len);
    int usb_read(usb_dev_handle *dev, unsigned char *tmp);
    int usb_read(usb_dev_handle *dev, unsigned char *tmp, int len);
    int usb_read(usb_dev_handle *dev, char *tmp, int len);
    void poll_commands(usb_dev_handle *dev);


    void file_range_cmd(usb_dev_handle *dev, unsigned char *data_size);
    void send_response_header(usb_dev_handle *dev, unsigned char *cmd_id, unsigned char *data_size);

    void submit_char_arr(unsigned char *tmp, unsigned char *str, int be, int len);


    void print_char_arr(char *tmp, int len, char *Suffix);
    void print_char_arr(unsigned char *tmp, int len, char *Suffix);

    void char2unchar(char *c, unsigned char *uc, int length);
    void unchar2char(unsigned char *uc, char *c, int length);

    void uchar2long(unsigned char *c, uint64_t *tmp);
    void  printww();
    usb_dev_handle* Connect_switch();
    usb_dev_handle* open_dev(void);
    void send_filelist(usb_dev_handle *dev,string path_);
    void  send_singlefile(usb_dev_handle *dev,string path_);
};


#endif // MAIN0_H
