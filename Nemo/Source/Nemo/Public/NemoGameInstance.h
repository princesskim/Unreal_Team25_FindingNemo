#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NemoGameInstance.generated.h"

UCLASS()
class NEMO_API UNemoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UNemoGameInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;

	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
	
};
