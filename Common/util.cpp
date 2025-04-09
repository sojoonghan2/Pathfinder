#include "pch.h"
#include "util.h"

UTIL_START

void DisplayError()
{
	int error_code = WSAGetLastError();

	LPVOID lpMsgBuf{};

	// ���� �ڵ忡 ���� �޽��� ����
	DWORD dw = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	if (dw) {
		// ���� �޽��� ���
		std::wcout << L"����: " << (wchar_t*)lpMsgBuf << std::endl;

		// �޸� ����
		LocalFree(lpMsgBuf);
	}
	else {
		std::wcout << L"���� �޽��� ���� ����, ���� �ڵ�: " << error_code << std::endl;
	}
}

void DisplayQuitError()
{
	DisplayError();
	exit(-1);
}

UTIL_END