#include "NemoGameState.h"
#include "NemoGameInstance.h"
#include "MarinController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "BabyFish.h"
#include "BombItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ANemoGameState::ANemoGameState()
{
    Score = 0;
    LevelDuration = 300.f;
    CurrentLevelIndex = 0;
    MaxLevels = 3;
    TargetToSpawn = 0;
    ItemToSpawn = 0;
    CreatureToSpawn = 0;
    CurrentWave = 0;
    MaxWaves = 3;
    BabyFishCount = 0;
    MinBabyFishCount = 0;
    MaxBabyFishCount = 0;
    BossSpawnLocation = FVector::ZeroVector;
}

void ANemoGameState::BeginPlay()
{
    Super::BeginPlay();

    StartLevel();

    UpdateHUD();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ANemoGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 ANemoGameState::GetScore() const
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
            MarinController->ShowLevelClear();
            MarinController->SetPause(true);
        }
    }
}

void ANemoGameState::RandomSpawn(EActorType SpawnType, int32 SpawnNum, TArray<AActor*> FoundVolumes)
{
    if (FoundVolumes.Num() == 0) return;

    for (int32 i = 0; i < SpawnNum; i++)
    {
        // 랜덤 볼륨에서 스폰
        int32 RandomIndex = FMath::RandRange(0, FoundVolumes.Num() - 1);
        ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[RandomIndex]);
        if (SpawnVolume)
        {
            SpawnVolume->SpawnRandomItem(SpawnType);
        }
    }
}

void ANemoGameState::StartLevel()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UNemoGameInstance* NemoGameInstance = Cast<UNemoGameInstance>(GameInstance);
        if (NemoGameInstance)
        {
            CurrentLevelIndex = NemoGameInstance->CurrentLevelIndex;
        }
    }

    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (AMarinController* MarinController = Cast<AMarinController>(PlayerController))
        {
            MarinController->ShowGameHUD();
        }
    }

    UpdateHUD();

    StartWave(1);
}

void ANemoGameState::StartWave(int32 WaveIndex)
{
    // 이전 웨이브 액터 정리
    if (WaveIndex > 1)
    {
        ClearWaveActors();
    }

    CurrentWave = WaveIndex;

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
        FString::Printf(TEXT("Wave %d Start!"), CurrentWave));

    if (CurrentWave == 1)
    {
        RandomSpawn(EActorType::Target, TargetToSpawn, FoundVolumes);

        TArray<AActor*> SpawnedFish;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABabyFish::StaticClass(), SpawnedFish);
        BabyFishCount = SpawnedFish.Num();
        MaxBabyFishCount = SpawnedFish.Num();

        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            FString::Printf(TEXT("BabyFish Count: %d"), BabyFishCount));
    }
    else if (CurrentWave == 2)
    {

        // 스테이지 클리어 UI + 다음 스테이지 소개 
        AMarinController* MarinController = Cast<AMarinController>(
            UGameplayStatics::GetPlayerController(GetWorld(), 0)
        );

        if (MarinController)
        {
            MarinController->ShowNarrationPanel();
        }

        RandomSpawn(EActorType::Target, TargetToSpawn, FoundVolumes);
        RandomSpawn(EActorType::Item, ItemToSpawn, FoundVolumes);
        
        TArray<AActor*> SpawnedFish;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABabyFish::StaticClass(), SpawnedFish);
        BabyFishCount = SpawnedFish.Num();

        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            FString::Printf(TEXT("BabyFish Count: %d"), BabyFishCount));
    }
    else if (CurrentWave == 3)
    {

        // 스테이지 클리어 UI + 다음 스테이지 소개 
        AMarinController* MarinController = Cast<AMarinController>(
            UGameplayStatics::GetPlayerController(GetWorld(), 0)
        );

        if (MarinController)
        {
            MarinController->ShowNarrationPanel();
        }

        if (BossSharkClass)
        {
            RandomSpawn(EActorType::Target, TargetToSpawn, FoundVolumes);
            RandomSpawn(EActorType::Item, ItemToSpawn, FoundVolumes);
            RandomSpawn(EActorType::Creature, CreatureToSpawn, FoundVolumes);

            TArray<AActor*> SpawnedFish;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABabyFish::StaticClass(), SpawnedFish);
            BabyFishCount = SpawnedFish.Num();

            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                FString::Printf(TEXT("BabyFish Count: %d"), BabyFishCount));

            /*
            FActorSpawnParameters SpawnParams;
            GetWorld()->SpawnActor<AActor>(
                BossSharkClass,
                BossSpawnLocation,
                FRotator::ZeroRotator,
                SpawnParams
            );
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Boss Spawned!"));
            */
        }
    }
}

void ANemoGameState::OnBabyFishCollected()
{
    BabyFishCount--;
    MinBabyFishCount++;

    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
        FString::Printf(TEXT("BabyFish Remaining: %d"), BabyFishCount));

    if (BabyFishCount <= 0)
    {
        OnWaveClear();
    }
}

void ANemoGameState::OnBossDefeated()
{
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Boss Defeated!"));
    OnWaveClear();
}

void ANemoGameState::OnWaveClear()
{
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
        FString::Printf(TEXT("Wave %d Clear!"), CurrentWave));

    if (CurrentWave >= MaxWaves)
    {
        EndLevel();
        return;
    }

    MinBabyFishCount = 0;
    StartWave(CurrentWave + 1);
}

void ANemoGameState::ClearWaveActors()
{
    // 남은 베이비피쉬 정리
    TArray<AActor*> RemainingFish;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABabyFish::StaticClass(), RemainingFish);
    for (AActor* Actor : RemainingFish)
    {
        Actor->Destroy();
    }

    // 남은 폭탄 정리
    TArray<AActor*> RemainingBombs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABombItem::StaticClass(), RemainingBombs);
    for (AActor* Actor : RemainingBombs)
    {
        Actor->Destroy();
    }
}

void ANemoGameState::OnLevelTimeUp()
{
    EndLevel();
}

void ANemoGameState::EndLevel()
{
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

// ================ UI ================

void ANemoGameState::UpdateHUD()
{
   
	if (APlayerController* PlayerController =
		GetWorld()->GetFirstPlayerController())
	{
		if (AMarinController* MarinController =
			Cast<AMarinController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MarinController->GetHUDWidget())
			{
                
                if (UTextBlock* TimeText = Cast<UTextBlock>
                    (HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingTime)));
                }
				if (UTextBlock* ScoreText = Cast<UTextBlock>
					(HUDWidget->GetWidgetFromName(TEXT("ScoreText"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UNemoGameInstance* NemoGameInstance = Cast<UNemoGameInstance>(GameInstance);
                        ScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), NemoGameInstance->TotalScore)));
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>
					(HUDWidget->GetWidgetFromName(TEXT("Stage"))))
				{
					LevelIndexText->SetText(FText::FromString
					(FString::Printf(TEXT("%d"), CurrentWave + 1)));
				}

                if (UTextBlock* LevelIndexText = Cast<UTextBlock>
                    (HUDWidget->GetWidgetFromName(TEXT("MaxFish"))))
                {
                    LevelIndexText->SetText(FText::FromString
                    (FString::Printf(TEXT("%d"), MaxBabyFishCount)));
                }

                if (UTextBlock* LevelIndexText = Cast<UTextBlock>
                    (HUDWidget->GetWidgetFromName(TEXT("CollectedFish"))))
                {
                    LevelIndexText->SetText(FText::FromString
                    (FString::Printf(TEXT("%d"), MinBabyFishCount)));
                }
			}
		}
	}
}
