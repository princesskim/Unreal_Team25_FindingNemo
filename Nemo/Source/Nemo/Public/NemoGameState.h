#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ActorSpawnRow.h"
#include "NemoGameState.generated.h"

UCLASS()
class NEMO_API ANemoGameState : public AGameState
{
    GENERATED_BODY()
public:
    ANemoGameState();
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Score")
    int32 Score;
    UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Level")
    float LevelDuration;
    float ElapsedTime;
    UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Level")
    int32 CurrentLevelIndex;
    UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Level")
    int32 MaxLevels;
    UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Level")
    TArray<FName> LevelMapNames;
    UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Spawn")
    int32 ItemToSpawn;
    UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Spawn")
    int32 TargetToSpawn;
    UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Spawn")
    int32 CreatureToSpawn;

    UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Spawn")
    int32 BossToSpawn;


    //================ Time ====================
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "TIme")
    float TimeDamage;


    // ================ Wave ================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWave;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 MaxWaves;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 BabyFishCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 MinBabyFishCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 MaxBabyFishCount;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    TSubclassOf<AActor> BossSharkClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    FVector BossSpawnLocation;

    FTimerHandle LevelTimerHandle;
    FTimerHandle HUDUpdateTimerHandle;

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);
    UFUNCTION(BlueprintCallable, Category = "Level")
    void OnGameOver();
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void OnBabyFishCollected();
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void OnBossDefeated();

    void RandomSpawn(EActorType SpawnType, int32 SpawnNum, TArray<AActor*> FoundVolumes);
    void StartLevel();
    void StartWave(int32 WaveIndex);
    void ApplyTimeDamge();
    void OnWaveClear();
    void ClearWaveActors();
    void OnLevelTimeUp();
    void EndLevel();
    void UpdateHUD();
    
};
