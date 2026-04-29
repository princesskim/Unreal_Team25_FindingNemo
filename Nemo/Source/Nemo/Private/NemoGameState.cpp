#include "NemoGameState.h"
#include "NemoGameInstance.h"
#include "MarinController.h"
#include "Kismet/GamePlayStatics.h"
#include "SpawnVolume.h"
//#include "Components/TextBlock.h"
//#include "Blueprint/UserWidget.h"

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
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMarinController* MarinController = Cast<AMarinController>(PlayerController))
		{
			MarinController->SetPause(true);
		}
	}
}

void ANemoGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMarinController* MarinController = Cast<AMarinController>(PlayerController))
		{
			//MarinController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UNemoGameInstance* NemoGameInstance = Cast<UNemoGameInstance>(GameInstance);
		if (NemoGameInstance)
		{
			CurrentLevelIndex = NemoGameInstance->CurrentLevelIndex;
		}
	}

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				//AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				//if (SpawnedActor && SpawnedActor->IsA(~~::StaticClass()))
				{
					//~~Count++;
				}
			}
		}
	}
	// 시간 제한을 둘때 사용 안하면 제거 예정
	/*
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);*/
}

void ANemoGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ANemoGameState::EndLevel()
{
	//GetWorldTimerManager().ClearTimer(LevelTimerHandle);


	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UNemoGameInstance* NemoGameInstance = Cast<UNemoGameInstance>(GameInstance);
		if (NemoGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			NemoGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ANemoGameState::UpdateHUD()
{
	if (APlayerController* PlayerController =
		GetWorld()->GetFirstPlayerController())
	{
		if (AMarinController* MarinController =
			Cast<AMarinController>(PlayerController))
		{
			// UI 구현과 키값에 따라 일부 변경 예정
			/*
			if (UUserWidget* HUDWidget = MarinController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>
					(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>
					(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UNemoGameInstance* NemoGameInstance = Cast<UNemoGameInstance>(GameInstance);
						ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), NemoGameInstance->TotalScore)));
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>
					(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString
					(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}
			}
			*/
		}
	}
}
