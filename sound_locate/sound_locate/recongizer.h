#include <iostream>
#include <windows.h>
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

using namespace std;
#define BUFFER_LAYER 1024

#pragma comment(lib,"winmm.lib")

HANDLE wait;
HWAVEIN hWaveIn;//输入设备
WAVEFORMATEX waveform;//采集的音频的结构体
BYTE* pBuffer;//采集音频的数据缓存
WAVEHDR wHder[BUFFER_LAYER] = {};;//采集音频时包含数据缓存的结构体

std::string recong()
{
	int				aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;		//音频状态
	int				ep_stat = MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int				rec_stat = MSP_REC_STATUS_SUCCESS;			//识别状态
	int				errcode = MSP_SUCCESS;
	int				errcode_QISRAudioWrite = MSP_SUCCESS;
	string        resultresult = "";

	long            len = 6400;
	char			hints[100] = { NULL };
	const char* session_id = QISRSessionBegin(NULL, "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312,vad_eos=200", &errcode);

	waveform.wFormatTag = WAVE_FORMAT_PCM;
	waveform.nSamplesPerSec = 16000;//采样率
	waveform.wBitsPerSample = 16;
	waveform.nChannels = 1;
	waveform.nAvgBytesPerSec = 32000;
	waveform.nBlockAlign = 2;
	waveform.cbSize = 0;
	wait = CreateEvent(NULL, 0, 0, NULL);
	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);


	for (int i = 0; i < BUFFER_LAYER; i++)
	{
		pBuffer = new BYTE[32000];
		wHder[i].lpData = (LPSTR)pBuffer;
		wHder[i].dwBufferLength = 32000;
		wHder[i].dwBytesRecorded = 0;
		wHder[i].dwUser = 0;
		wHder[i].dwFlags = 0;
		wHder[i].dwLoops = 1;

		waveInPrepareHeader(hWaveIn, &wHder[i], sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn, &wHder[i], sizeof(WAVEHDR));
		waveInStart(hWaveIn);
		Sleep(200);
		//cout << "<";
		aud_stat = MSP_AUDIO_SAMPLE_FIRST;
		errcode_QISRAudioWrite = QISRAudioWrite(session_id, pBuffer, len, aud_stat, &ep_stat, &rec_stat);
		if (errcode != MSP_SUCCESS)
		{
			delete pBuffer;
			return "";
		}
		//cout << "ep_stat:" << ep_stat << endl;
		//cout << "rec_stat:" << rec_stat << endl;

		if (MSP_EP_AFTER_SPEECH == ep_stat)
		{
			delete[] pBuffer;
			waveInReset(hWaveIn);
			break;
		}
		waveInReset(hWaveIn);
		delete[] pBuffer;
	}

	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	//cout << "*errcode:" << errcode << endl;
	if (errcode != MSP_SUCCESS)
	{
		delete pBuffer;
		return "";
	}
	//cout << "**************" << endl;
	string  str;
	while (MSP_REC_STATUS_COMPLETE != rec_stat)
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (NULL != rslt)
		{
			str = rslt;
			resultresult += str.c_str();
			//cout << "resultresult:" << resultresult.c_str() << endl;
		}
		Sleep(150); //防止频繁占用CPU
	}



	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	std::cout << std::endl << std::endl;
	std::cout <<" o_O:" <<resultresult.c_str() << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);


	QISRSessionEnd(session_id, hints);
	//delete pBuffer;
	waveInClose(hWaveIn);
	return  resultresult;
}



std::string recong1()
{
	int				aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;		//音频状态
	int				ep_stat = MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int				rec_stat = MSP_REC_STATUS_SUCCESS;			//识别状态
	int				errcode = MSP_SUCCESS;
	int				errcode_QISRAudioWrite = MSP_SUCCESS;
	string        resultresult = "";

	long            len = 6400;
	char			hints[100] = { NULL };
	const char* session_id = QISRSessionBegin(NULL, "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312,vad_eos=2000", &errcode);

	waveform.wFormatTag = WAVE_FORMAT_PCM;
	waveform.nSamplesPerSec = 16000;//采样率
	waveform.wBitsPerSample = 16;
	waveform.nChannels = 1;
	waveform.nAvgBytesPerSec = 32000;
	waveform.nBlockAlign = 2;
	waveform.cbSize = 0;

	wait = CreateEvent(NULL, 0, 0, NULL);
	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	for (int i = 0; i < BUFFER_LAYER; i++)
	{
		pBuffer = new BYTE[32000];

		wHder[i].lpData = (LPSTR)pBuffer;
		wHder[i].dwBufferLength = 32000;
		wHder[i].dwBytesRecorded = 0;
		wHder[i].dwUser = 0;
		wHder[i].dwFlags = 0;
		wHder[i].dwLoops = 1;

		waveInPrepareHeader(hWaveIn, &wHder[i], sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn, &wHder[i], sizeof(WAVEHDR));
		waveInStart(hWaveIn);
		Sleep(200);

		aud_stat = MSP_AUDIO_SAMPLE_FIRST;
		errcode_QISRAudioWrite = QISRAudioWrite(session_id, pBuffer, len, aud_stat, &ep_stat, &rec_stat);
		if (errcode != MSP_SUCCESS)
		{
			delete pBuffer;
			return "";
		}
		//cout << "ep_stat:" << ep_stat << endl;
		//cout << "rec_stat:" << rec_stat << endl;

		if (MSP_EP_AFTER_SPEECH == ep_stat)
		{
			waveInReset(hWaveIn);
			break;
		}
		waveInReset(hWaveIn);
		delete pBuffer;
	}

	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	//cout << "*errcode:" << errcode << endl;
	if (errcode != MSP_SUCCESS)
	{
		delete pBuffer;
		return "";
	}

	string  str;
	while (MSP_REC_STATUS_COMPLETE != rec_stat)
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (NULL != rslt)
		{
			str = rslt;
			resultresult += str.c_str();
			//cout << "resultresult:" << resultresult.c_str() << endl;
		}
		Sleep(150); //防止频繁占用CPU
	}

	//std::cout << resultresult.c_str() << std::endl;
	QISRSessionEnd(session_id, hints);
	//delete pBuffer;
	waveInClose(hWaveIn);
	return  resultresult;
}
