#include "pch.h"
#include "util.h"

UTIL_START

void DisplayError()
{
	int error_code = WSAGetLastError();

	LPVOID lpMsgBuf{};

	// 오류 코드에 대한 메시지 포맷
	DWORD dw = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	if (dw) {
		// 오류 메시지 출력
		std::wcout << L"오류: " << (wchar_t*)lpMsgBuf << std::endl;

		// 메모리 해제
		LocalFree(lpMsgBuf);
	}
	else {
		std::wcout << L"오류 메시지 포맷 실패, 오류 코드: " << error_code << std::endl;
	}
}

void DisplayQuitError()
{
	DisplayError();
	exit(-1);
}

UTIL_END