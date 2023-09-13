/*
 * Author: Jinu Chae
 * Date: 2023-09-07
 * Description: 장갑형 VR 컨틀로러부터 손가락 구부린 정도를 입력받아 손의 형태(가위, 바위, 보)를 판단하는 코드입니다.
 */

#include "Referee.h"
#include "SerialPort.h"

AReferee::AReferee()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AReferee::BeginPlay()
{
	Super::BeginPlay();
	
	MySerialPort = new SerialPort();
	if (MySerialPort->OpenPort(_T("COM4")))
	{
		UE_LOG(LogTemp, Log, TEXT("연결 성공"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("연결 실패"));
	}

	FingerValues.SetNumZeroed(static_cast<uint32>(EFingerState::Max));
	IsExtendFinger.SetNumZeroed(static_cast<uint32>(EFingerState::Max));

	HandState = EHandState::None;
}

void AReferee::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (MySerialPort)
	{
		MySerialPort->ClosePort();
		delete MySerialPort;
		MySerialPort = nullptr;
	}
}

void AReferee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ANSICHAR Buffer[SerialPort::BUFFER_SIZE] = { 0 };
	int32 BufferReadBytes = MySerialPort->ReadLine(Buffer);

	if (BufferReadBytes > 0)
	{
		TCHAR* TBuffer = ANSI_TO_TCHAR(Buffer);
		// UE_LOG(LogTemp, Log, TEXT("input : %s"), TBuffer);

		// Parse and Fill finger values
		FString BufferString(TBuffer);
		if ((_T('0') <= BufferString[0]) && (BufferString[0] <= _T('9')))
		{
			TArray<FString> ParsedDatas;
			BufferString.ParseIntoArray(ParsedDatas, TEXT(","));

			check(FingerValues.Num() >= ParsedDatas.Num());
			for (uint8 i = 0; i < ParsedDatas.Num(); ++i)
			{
				FingerValues[i] = FCString::Atoi(*ParsedDatas[i]);
				IsExtendFinger[i] = (FingerValues[i] >= 8);
			}
		}

		// Determine current hand shape 
		DetermineHandShape(HandState);

		// Log current hand shape
		LogEHandState(HandState);
	}
}

void AReferee::DetermineHandShape(EHandState& OutHandState)
{
	uint8 ExtendedCount = 0;
	for (uint8 i = 0; i < IsExtendFinger.Num(); ++i)
	{
		ExtendedCount += IsExtendFinger[i];  // IsExtendFinger[i] : True or False
	}

	if (ExtendedCount == 0)
	{
		OutHandState = EHandState::Rock;
	}
	else if (ExtendedCount == 2)
	{
		OutHandState = EHandState::Scissors;
	}
	else if (ExtendedCount == 5)
	{
		OutHandState = EHandState::Paper;
	}
	else
	{
		OutHandState = EHandState::None;
	}

	// UE_LOG(LogTemp, Log, TEXT("ExtendedCount : %d"), ExtendedCount);
}

void AReferee::LogEHandState(EHandState InHandState)
{
	switch (InHandState)
	{
	case EHandState::None:
		UE_LOG(LogTemp, Log, TEXT("Hand State : None"));
		break;
	case EHandState::Rock:
		UE_LOG(LogTemp, Log, TEXT("Hand State : Rock"));
		break;
	case EHandState::Paper:
		UE_LOG(LogTemp, Log, TEXT("Hand State : Paper"));
		break;
	case EHandState::Scissors:
		UE_LOG(LogTemp, Log, TEXT("Hand State : Scissors"));
		break;
	}
}