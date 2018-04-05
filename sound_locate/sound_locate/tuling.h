/*
Created by Jinhua Zhao, 2017.09.28.
Contact: 3210513029@qq.com.
*/
#include <winhttp.h> 

#pragma comment(lib,"winhttp.lib")
#define TULING_URL  L"http://www.tuling123.com/openapi/api?key=7d4f7c0c97bb428aabfb3b4377b1553d&info=%s"

char *UnicodeToANSI(const wchar_t *str)
{
	static char result[1024];
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, len, NULL, NULL);
	result[len] = '\0';
	return result;
}
wchar_t *UTF8ToUnicode(const char *str)
{
	static wchar_t result[1024];
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, result, len);
	result[len] = L'\0';
	return result;
}
wchar_t *ANSIToUnicode(const char* str)
{
	int textlen;
	static wchar_t result[1024];
	textlen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	memset(result, 0, sizeof(char) * (textlen + 1));
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)result, textlen);
	return result;
}

std::string tuling(std::string input_str)
{
	static wchar_t String[1024];
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer = NULL;
	static HINTERNET hSession = WinHttpOpen(L" ", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	static HINTERNET hConnect = NULL, hRequest = NULL;
	BOOL  bResults = FALSE;
	std::string name;
	char uin[1024];
	int i;

	for (i = 0; i<input_str.length(); i++)
		uin[i] = input_str[i];
	uin[i] = '\0';

	wsprintf(String, TULING_URL, ANSIToUnicode(uin));

	//Establish an HTTP connection session.It can be the host name or the IP address. No HTTP prefix.
	if (hSession)
	{
		if (!hConnect)
		{
			hConnect = WinHttpConnect(hSession, L"www.tuling123.com", INTERNET_DEFAULT_HTTP_PORT, 0);
		}
	}

	//Create an HTTP request handle.
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"POST", String, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH);
	//Send request data.
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	//End the request, receive the data.
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	//If there is something wrong, that you can use the 'getlasterror()' to get the error information.
	else
		printf("Error %d has occurred.\n", GetLastError());
	
	// An internal loop is used to ensure that all data is received.
	if (bResults)
	{
		do
		{
			//Check if there are datas that to be received.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}

			if (!dwSize)  break;

			//Allocate memory for buffering and read.
			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				break;
			}

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
			{
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			}
			else
			{
				//Turing api returns the contents of the UTF-8 encoding, we need to convert it back to ANSI in the console display.
				//Because I did not use the JSON library, so I broke this string of violence.
				pszOutBuffer[strlen(pszOutBuffer) - 2] = '\0';
				//printf("Ð¡Áé:%s\n\n", UnicodeToANSI(UTF8ToUnicode(pszOutBuffer)) + 23);
				name = UnicodeToANSI(UTF8ToUnicode(pszOutBuffer)) + 23;
				return name;
			}

			delete[] pszOutBuffer;
			if (!dwDownloaded) break;
		} while (dwSize > 0);
	}

	//Close the closed handle
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	return 0;
}