#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Referee.generated.h"

UENUM(BlueprintType)
enum class EFingerState : uint8
{
	Thumb    UMETA(DisplayName = "Thumb"),
	Index    UMETA(DisplayName = "Index"),
	Middle   UMETA(DisplayName = "Middle"),
	Ring     UMETA(DisplayName = "Ring"),
	Pinky    UMETA(DisplayName = "Pinky"),

	Max      UMETA(Hidden)  // Sentinel Value
};

UENUM(BlueprintType)
enum class EHandState : uint8
{
	None     UMETA(DisplayName = "None"),
	Rock     UMETA(DisplayName = "Rock"),
	Paper    UMETA(DisplayName = "Paper"),
	Scissors UMETA(DisplayName = "Scissors")
};

UCLASS()
class ROCKPAPERSCISSORS_API AReferee : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReferee();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void DetermineHandShape(EHandState& OutHandState);
	void LogEHandState(EHandState InHandState);

private:
	class SerialPort* MySerialPort = nullptr;
	TArray<uint8> FingerValues;
	TArray<uint8> IsExtendFinger;
	EHandState HandState;
};
