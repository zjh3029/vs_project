#include "tts.h"
#include "recongizer.h"
#include "tuling.h"
#include <iostream>
#include "hidapi.h"
#include <string>
#include <thread>  


#pragma comment (lib,"hidapi.lib")
using namespace std;

int move_angle = 0;
unsigned int angle_now = 0;
unsigned int angle_before = 0;

bool findstr_or(std::string str_org, std::initializer_list<string> lst)
{

	for (auto i = lst.begin(); i != lst.end(); i++)
	{
		if (str_org.find(*i) != str_org.npos)
		{
			return true;
		}
	}
	return false;
}

unsigned int get_angle()
{
	int res;
	unsigned char buf[65];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;
	unsigned int angle=0;

	res = hid_init();// Initialize the hidapi library

					 // Open the device using the VID, PID,and optionally the Serial number.
	handle = hid_open(0x2752, 0x1C, NULL);//��ʾ��USB�豸���ĸ����̵������豸��ȥ�ο��ĵ�����
	if (!handle)
	{
		wprintf(L"Sorry, Please check if the USB is connected!", wstr);
		exit(-1);
	}
										  // Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	//wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	//wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	//wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	//wprintf(L"Indexed String 1: %s\n", wstr);

	// �˴�����д���ֽڣ�
	//buf[0] = 0x0;
	//buf[1] = 0x80;
	//res = hid_write(handle, buf, 65);

	/*���������ֽ����ݣ�
	---buf[0]��0x06
	---buf[1]��0x36
	---buf[2]��1��ʾ�������źţ�0��ʾû�У���������
	---buf[3]������Ǹ��ֽڣ�Ϊ1����0
	---buf[4]������ǵ��ֽ�
	---buf[5]�������źŵ��﷽��
	*/
	while (1)// Read requested state
	{
		res = hid_read(handle, buf, 65);
		if (buf[0] == 6 && buf[2] == 1)
		{
			//wprintf(L"Have detected!!!\n");
			//for (i = 0; i < 6; i++)// Print out the returned buffer.
				//printf("buf[%d]: %d\n", i, buf[i]);
			angle = ((unsigned int)(buf[3] << 8)) | buf[4];
			wprintf(L"The angle is:%u\n", angle);
			break;
		}
	}
	// Finalize the hidapi library
	res = hid_exit();
	hid_close(handle);
	return angle;
}

int  compare_angle(unsigned int angle_now, unsigned int angle_before)
{
	int d_value = 0;
	d_value = angle_now - angle_before;
	if (abs(d_value) <= 180)
	{
		std::cout << "����ת��" << std::endl;
		std::cout << "��ת�ĽǶȴ�С�ǣ�" << abs(d_value) << std::endl;
		return abs(d_value);
	}
	else
	{
		std::cout << "����ת��" << std::endl;
		std::cout << "��ת�ĽǶȴ�С�ǣ�" << 360-abs(d_value) << std::endl;
		return abs(d_value)-360;
	}
}

void move()
{
	angle_now = get_angle();
	move_angle=compare_angle(angle_now, angle_before);
	angle_before = angle_now;
	return;
}


int main()
{
	const char* login_params = "appid = 59c066c4, work_dir = .";
	MSPLogout(); //�˳���¼
				 //��һ���������û������ڶ������������룬����NULL���ɣ������������ǵ�¼����
	if (MSP_SUCCESS != MSPLogin(NULL, NULL, login_params))
	{
		std::cout << "========================" << std::endl;
		std::cout << "Login failed!" << std::endl;
		std::cout << "========================" << std::endl;
	}
	std::string str;
	tts("��ã��ܸ�����ʶ����");

	string angle_str;
	do
	{
		//angle_str=to_string(angle_now);
		std::thread t1(move);//��ʱ����һ���̣߳���Ӱ��recong()������ִ��
		t1.detach();
		str = recong();
		if (str.empty())
		{
			continue;
		}
		if (findstr_or(str, { "���" }))
		{
			tts("��ã�");
		}
		if (findstr_or(str, { "�˳�","ֹͣ","����" }))
		{
			tts("�õģ������Ǹ������ģ�");
			break;
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		string answer = tuling(str);
		std::cout << " O_O:" << answer<< std::endl;
		tts(answer.c_str());
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		//tts(("���������ڵķ����ǣ�"+ angle_str).c_str());
	} while (1);
	return 0;
}


