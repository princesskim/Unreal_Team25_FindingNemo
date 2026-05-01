#include "NemoGameInstance.h"

UNemoGameInstance::UNemoGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UNemoGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}

void UNemoGameInstance::ResetGameData()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}