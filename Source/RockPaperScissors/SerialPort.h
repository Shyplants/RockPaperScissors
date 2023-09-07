#pragma once

#include "CoreMinimal.h"
#include "CoreTypes.h"

/**
 *
 */
class ROCKPAPERSCISSORS_API SerialPort
{
public:
	SerialPort();
	~SerialPort();

	bool OpenPort(const TCHAR* sPort, int32 nBaud = 9600);
	void ClosePort();

	int ReadLine(ANSICHAR* Buffer);

	static const int BUFFER_SIZE = 128;
private:

	void* PortHandle;
};