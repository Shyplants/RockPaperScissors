#include "SerialPort.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/MinWindows.h"
#include "Windows/HideWindowsPlatformTypes.h"

SerialPort::SerialPort()
{
	PortHandle = NULL;
}

SerialPort::~SerialPort()
{
	ClosePort();
}

bool SerialPort::OpenPort(const TCHAR* sPort, int32 nBaud)
{
	FString szPort = sPort;
	// DCB dcb;

	PortHandle = CreateFile(*szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (PortHandle == NULL)
	{
		uint32 dwError = GetLastError();
		UE_LOG(LogTemp, Error, TEXT("Failed to open port %s. Error: %08X"), *szPort, dwError);
		return false;
	}

	if (PortHandle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			UE_LOG(LogTemp, Error, TEXT("Error: Handle was not attached"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error"));
		}
	}


	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 500;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 10;
	SetCommTimeouts(PortHandle, &CommTimeOuts);

	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	GetCommState(PortHandle, &dcb);
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;

	if (!SetCommState(PortHandle, &dcb) || !SetupComm(PortHandle, BUFFER_SIZE, BUFFER_SIZE))
	{
		uint32 dwError = GetLastError();
		CloseHandle(PortHandle);
		PortHandle = NULL;
		return false;
	}

	PurgeComm(PortHandle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	return true;
}

void SerialPort::ClosePort()
{
	if (!PortHandle)
		return;
	CloseHandle(PortHandle);
	PortHandle = NULL;
}

int32 SerialPort::ReadLine(ANSICHAR* Buffer)
{
	DWORD dwBytesRead = 0;
	ANSICHAR buffer;
	int32 len = 0;

	while (true) {
		if (!ReadFile(PortHandle, &buffer, 1, &dwBytesRead, NULL))
		{
			UE_LOG(LogTemp, Log, TEXT("error : readLine"));
			return -1;
		}

		if (dwBytesRead > 0)
		{
			if (buffer == '\n')
			{
				Buffer[len] = '\n';
				break;
			}

			else if (buffer != '\r')
			{
				Buffer[len++] = buffer;
			}
		}
	}

	return len;
}