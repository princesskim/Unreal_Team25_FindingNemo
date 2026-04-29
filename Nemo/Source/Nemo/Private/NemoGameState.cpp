#include "NemoGameState.h"
#include "NemoGameInstance.h"

ANemoGameState::ANemoGameState()
{
	Score = 0;
	LevelDuration = 300.f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void ANemoGameState::BeginPlay()
{
	Super::BeginPlay();
}

int32 ANemoGameState::GetScoure() const
{
	return Score;
}

void ANemoGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UNemoGameInstance* NemoGameInstance = Cast<UNemoGameInstance>(GameInstance);
		if (NemoGameInstance)
		{
			NemoGameInstance->AddToScore(Amount);
		}
	}
}

void ANemoGameState::OnGameOver()
{
}

void ANemoGameState::StartLevel()
{
}

void ANemoGameState::OnLevelTimeUp()
{
}

void ANemoGameState::EndLevel()
{
}

void ANemoGameState::UpdateHUD()
{
}
