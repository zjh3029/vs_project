#include <conio.h>
#include "XAudio2.h"
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include <iostream>
#pragma comment(lib,"xaudio2.lib")

void broadcast(LPCWSTR wave_name)
{
	PlaySound(wave_name, NULL, SND_FILENAME);
}

int text_to_speech(const char* src_text,const char* session_begin_params)
{
	int ret = -1;
	int  synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	unsigned int audio_len = 0;
	unsigned int speech_len = 0;

	const char*sessionID = QTTSSessionBegin(session_begin_params, &ret);
	const void* audio_data = NULL;
	char* synth_speech = new char[2 * 1024 * 1024];

	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);

	IXAudio2 *pEngine = NULL;
	IXAudio2MasteringVoice *pMasterVoice = NULL;
	IXAudio2SourceVoice *pSourceVoice = NULL;

	WAVEFORMATEX waveFormat = { 1,1,16000,32000,2,16,0 };//获取文件格式

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))exit(0);
	if (FAILED(XAudio2Create(&pEngine)))exit(0);
	if (FAILED(pEngine->CreateMasteringVoice(&pMasterVoice)))exit(0);
	if (FAILED(pEngine->CreateSourceVoice(&pSourceVoice, &waveFormat)))exit(0);

	while (MSP_TTS_FLAG_DATA_END != synth_status)
	{
		audio_data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (ret) break;
		if (NULL != audio_data && 0 != audio_len)
		{
			memcpy(synth_speech + speech_len, audio_data, audio_len);
			speech_len += audio_len;
		}
	}
	ret = QTTSSessionEnd(sessionID, "Normal");
	BYTE *pData = new BYTE[speech_len];//申请内存空间，用于保存数据
	pData = (BYTE*)synth_speech;

	XAUDIO2_BUFFER buffer = { 0 };//将读取的文件数据，赋值XAUDIO2_BUFFER
	buffer.AudioBytes = speech_len;
	buffer.pAudioData = pData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	pSourceVoice->SubmitSourceBuffer(&buffer);//提交内存数据
	pSourceVoice->Start(0);//启动源声音
	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);//获取状态
	while (state.BuffersQueued)
	{
		pSourceVoice->GetState(&state);
	}
	pMasterVoice->DestroyVoice();//释放资源
	pSourceVoice->DestroyVoice();//释放资源
	pEngine->Release();//释放资源
	CoUninitialize();//释放资源
	std::cout << pData;
	delete[] pData;//释放资源
	pData = NULL;
	return 0;
}


void warningall()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!网络状态不太好!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
}


int tts(const char* text)
{
	int  ret = MSP_SUCCESS;
	const char* session_begin_params = "voice_name = xiaowanzi, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 100, pitch = 50, rdn = 0";
	//ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取,此时不应该再登陆
	if (*text == '\0')  return 0;
	text_to_speech(text, session_begin_params);
	//MSPLogout(); //退出登录
	return 0;
}
