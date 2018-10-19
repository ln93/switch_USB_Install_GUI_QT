﻿
#include "main0.h"



#define MY_VID 0x057E
#define MY_PID 0x3000


#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN 0x81
#define EP_OUT 0x01

// Device of bytes to transfer.
#define BUF_SIZE 64

#define HID_REPORT_SET 0x09
#define TIMEOUT 5000
#define USB_OUT_TIME 1000

#define CMD_ID_EXIT 0
#define CMD_ID_FILE_RANGE 1
#define CMD_TYPE_RESPONSE 1

#define CMD_READ_SIZE 32
using namespace std;


usb_dev_handle* main0::open_dev(void)
{
	struct usb_bus *bus;
	struct usb_device *dev;

	for (bus = usb_get_busses(); bus; bus = bus->next)
	{
		for (dev = bus->devices; dev; dev = dev->next)
		{
			
			if (dev->descriptor.idVendor == MY_VID
				&& dev->descriptor.idProduct == MY_PID)
			{			
				return usb_open(dev);
			}
		}
	}
	return NULL;
}

void  getFiles(string path,vector<string>& files)
{
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string path_ = path;
	hFile = _findfirst(path_.append("\\*.nsp").c_str(), &fileinfo);
	if (hFile == -1)
	{
		return;
	}
	do
	{
		if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
			string t = path+"\\";
			t.append(fileinfo.name);
			t.append("\n");
			files.push_back(t);
		}		
	} while (_findnext(hFile, &fileinfo) == 0);
	_findclose(hFile);
}
void  main0::printww() {
    Send_COUT(QString::fromLocal8Bit(
        "使用方法:\n1.先将switch进入Tinfol的USB安装界面,显示USB is ready\n2.将游戏目录粘贴进软件里面按回车,在switch里面选择要安装的nsp\n3.等待安装完成。"
        ));
	//
}

usb_dev_handle* main0::Connect_switch() {
	usb_dev_handle *dev = NULL; /* the device handle */
	int ret;
	void* async_read_context = NULL;
	void* async_write_context = NULL;
    Send_COUT(QString::fromLocal8Bit("请打开switch,进入tinfoil的usb安装界面。\n并使用type-C数据线将其连接到你的电脑上。"));
	do {
		usb_init();
		usb_find_busses();
		usb_find_devices();
		if ((dev = open_dev()))
			break;
		cout<<".";
		Sleep(5000);
	} while (true);
	cout << endl;
    Send_COUT(QString::fromLocal8Bit("success: switch设备连接成功！"));
	usb_set_configuration(dev, 1);
	usb_claim_interface(dev, MY_INTF);
	return dev;
}

void  main0::send_filelist(usb_dev_handle *dev,string path_) {
	int ret;
	unsigned char tul0[4] = { 84,85,76,48 };
	unsigned char len[4] = { 0 };
	unsigned char padding[8] = { 0 };
	char title[100];
    //string path_;
    //cout << "输入游戏目录" << endl;
    //cin >> path_;
	vector<string> files;
	getFiles(path_, files);
	long l = 0;
	for (int i = 0; i < files.size(); i++)
		l += files[i].length();
	memcpy(len, &l, 4);
	ret = usb_write(dev, tul0, 4);
	ret = usb_write(dev, len, 4);
	ret = usb_write(dev, padding, 8);
	for (int i = 0; i < files.size(); i++) {
		string f = files[i];
		f.copy(title, f.length(), 0);
		title[f.length()] = '\0';
		usb_write(dev, title, f.length());
	}
	if (ret == 1)
        Send_COUT(QString::fromLocal8Bit("请在switch界面选择要安装的nsp文件！"));
	poll_commands(dev);
}
void  main0::send_singlefile(usb_dev_handle *dev,string path_) {
    int ret;
    unsigned char tul0[4] = { 84,85,76,48 };
    unsigned char len[4] = { 0 };
    unsigned char padding[8] = { 0 };
    char title[100];
    //string path_;
    //cout << "输入游戏目录" << endl;
    //cin >> path_;
    //vector<string> files;
    //getFiles(path_, files);
    long l = path_.length();
    memcpy(len, &l, 4);
    ret = usb_write(dev, tul0, 4);
    ret = usb_write(dev, len, 4);
    ret = usb_write(dev, padding, 8);
        string f = path_;
        f.copy(title, f.length(), 0);
        title[f.length()] = '\0';
        usb_write(dev, title, f.length());
    if (ret == 1)
        Send_COUT(QString::fromLocal8Bit("请在switch界面选择要安装的nsp文件！"));
    poll_commands(dev);
}
void  main0::poll_commands(usb_dev_handle *dev) {
	unsigned char cmd[32];
	unsigned char magic[5];
	unsigned char cmd_type[4];
	unsigned char cmd_id[4];
	unsigned char cmd_size[10];
	
	char magic_tmp[5];
	while (true) {
		int ret = usb_read(dev,  cmd);
		if (ret < 0) {
			cout << usb_strerror() << endl;
			break;
		}
		submit_char_arr(cmd, magic,0,4);
		magic[4] = '\0';
		unchar2char(magic,magic_tmp,5);

		if (strcmp(magic_tmp,"TUC0")!=0)
			continue;
		submit_char_arr(cmd, cmd_type, 4, 8);
		submit_char_arr(cmd, cmd_id, 8, 12);
		submit_char_arr(cmd, cmd_size, 12, 20);
		if (cmd_id[0] == CMD_ID_EXIT) {
            Send_COUT(QString::fromLocal8Bit("NSP安装完成。"));
			break;
		}
		if (cmd_id[0] == CMD_ID_FILE_RANGE)
			file_range_cmd(dev, cmd_size);
	}
}
void  main0::uchar2long(unsigned char *c, uint64_t *tmp) {
    memcpy(tmp, c, 8);
}
void  main0::file_range_cmd(usb_dev_handle *dev, unsigned char *cmd_size) {
	unsigned char cmd[100];

	unsigned char range_size[8] = {0};
	unsigned char range_offset[8] = {0};
	unsigned char nsp_name_len[8] = { 0 };

	unsigned char cmd_id[4] = {1,0,0,0};

	uint64_t nspname_len_int;
	uint64_t nsp_size_int;
	uint64_t offset;

	int ret = usb_read(dev,cmd);	
	submit_char_arr(cmd, range_size, 0, 8);
	submit_char_arr(cmd, range_offset, 8, 16);
	submit_char_arr(cmd, nsp_name_len,16, 24);

	uchar2long(range_size, &nsp_size_int);
	uchar2long(nsp_name_len, &nspname_len_int);
	uchar2long(range_offset, &offset);
	char *nsp_name = new char[nspname_len_int];
	ret = usb_read(dev, nsp_name, nspname_len_int);
	nsp_name[nspname_len_int] = '\0';
	send_response_header(dev,cmd_id,range_size); 
    //cout << "当前安装文件:" << nsp_name<<",文件大小:"<<nsp_size_int<<endl;
	FILE *f;
	fopen_s(&f,nsp_name, "rb+");
	if (f == NULL)
        Send_COUT(QString::fromLocal8Bit("文件打开失败"));
	uint64_t curr_off = 0x0;
	uint64_t end_off = nsp_size_int;
	uint64_t read_size = 0x1000000;
	uint64_t a= _fseeki64(f, offset,SEEK_SET );
	double avg;
	while(curr_off<end_off) {
		if (curr_off + read_size >= end_off)
			read_size = end_off - curr_off;	
		unsigned char *nsp_value = new unsigned char[read_size];
		fread(nsp_value, sizeof(unsigned char), read_size, f);
		avg = curr_off / end_off;
        //Send_COUT(QString::fromLocal8Bit("·"));
        Send_ProgressBar(100*curr_off/nsp_size_int);
		usb_write(dev, nsp_value, read_size);
		curr_off += read_size;	
		delete[]nsp_value;
	}
    Send_COUT(QString::fromLocal8Bit(">安装完成"));
	fclose(f);	
}
void  main0::send_response_header(usb_dev_handle *dev, unsigned char *cmd_id, unsigned char *data_size) { //send header to switch
	int ret;
	unsigned char tuc0[4] = { 84,85,67,48 };
	unsigned char cmd_type_response[1] = {1};
	unsigned char padding3[3] = { 0 };
	unsigned char padding12[12] = { 0 };
	usb_write(dev, tuc0, 4);
	usb_write(dev, cmd_type_response, 1);
	usb_write(dev, padding3, 3);
	usb_write(dev, cmd_id, 4);
	usb_write(dev, data_size, 8);
	usb_write(dev, padding12, 12);
} 
void main0::submit_char_arr(unsigned char *tmp, unsigned char *str,int start,int end) {
	for (int i = start,j=0; i < end; i++,j++) {
		str[j] = tmp[i];
	}
}
void  main0::print_char_arr(char *tmp, int len,char *Suffix) {
	for (int i = 0; i < len; i++) {
		printf("%d  ", tmp[i]);
	}
	printf(" ===>%s\n",Suffix);
}
void  main0::print_char_arr(unsigned char *tmp, int len, char *Suffix) {
	for (int i = 0; i < len; i++) {
		printf("%d  ", tmp[i]);
	}
	printf(" ===>%s\n", Suffix);
}
void  main0::char2unchar(char *c,unsigned char *uc,int length) {
	for (int i = 0; i < length; i++) {
		uc[i] = c[i];
	}
}
char  convert(unsigned char in)
{
	if (in<128)
		return in;
	else
		return((char)in - 256);
}
void  main0::unchar2char(unsigned char *uc, char *c, int length) {
	for (int i = 0; i < length; i++) {
		c[i] = convert(uc[i]);
	}
}

int  main0::usb_write(usb_dev_handle *dev, unsigned char *tmp, int len) {
	char *cmd = new char[len];
	unchar2char(tmp, cmd, len);
	int ret = usb_bulk_write(dev, EP_OUT, cmd, len, USB_OUT_TIME);
	delete[] cmd;
	if (ret < 0) {
		cout << usb_strerror() << endl;
		return 0;
	}
	return 1;
}
int  main0::usb_write(usb_dev_handle *dev, char *tmp, int len) {

	int ret = usb_bulk_write(dev, EP_OUT, tmp, len, USB_OUT_TIME);
	if (ret < 0) {
		cout << usb_strerror() << endl;
		return 0;
	}
	return 1;
}
int  main0::usb_read(usb_dev_handle *dev, unsigned char *tmp) {
	char cmd[32] = { 0 };
	int ret = usb_bulk_read(dev, EP_IN, cmd, 32, 0);
	if (ret < 0)
		cout << usb_strerror() << endl;
	char2unchar(cmd, tmp, 32);
	return ret;

}
int  main0::usb_read(usb_dev_handle *dev, unsigned char *tmp, int len) {
	char *cmd = new char[len];
	int ret = usb_bulk_read(dev, EP_IN, cmd, len, 0);
	if (ret < 0)
		cout << usb_strerror() << endl;
	char2unchar(cmd, tmp, len);
	return ret;
}
int main0::usb_read(usb_dev_handle *dev, char *tmp, int len) {
	int ret = usb_bulk_read(dev, EP_IN, tmp, len, 0);
	if (ret < 0)
		cout << usb_strerror() << endl;
	return ret;
}
 main0::main0() {

    GoInstall=0;
    stopped=0;

}
 main0::~main0() {

stopped=1;

}
void main0::run() {

    usb_dev_handle *dev = NULL;
    //char boo;
    //printww();
    while(!stopped){
        if(GoInstall)
        {
            for(int i=0;i<nspnames.size();i++)
            {
                dev= Connect_switch();
                send_singlefile(dev,nspnames.at(i).toStdString());
            }

        }
        GoInstall=0;
        msleep(100);
    }
;
}
 void main0::installall(QStringList path)
 {
        nspnames=path;
        GoInstall=1;

 }
 void main0::installsingle(QString path)
 {
     usb_dev_handle *dev = NULL;
     //char boo;
     printww();
     dev= Connect_switch();
   //  do {
         send_singlefile(dev,path.toStdString());
        // cout << "是否继续安装? y:继续安装" << endl;
       //  cin >> boo;
      //  }
   //  while(boo=='y');

 }
