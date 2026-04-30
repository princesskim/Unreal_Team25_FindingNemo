#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAI.h"
#include "JellyfishTrap.h"
#include "JellyfishTrapAIController.generated.h"

UCLASS()
class NEMO_API AJellyfishTrapAIController : public ABaseEnemyAI
{
    GENERATED_BODY()

public:
    AJellyfishTrapAIController();

    virtual void Tick(float DeltaTime) override;
    virtual void OnPossess(APawn* InPawn) override;

private:
    AJellyfishTrap* JellyfishTrapOwner;

    FTimerHandle PatrolTimerHandle;

    // 순찰 - SpawnLocation 기준 랜덤 위치로 이동
    void DoPatrol();
    // 추적 - 플레이어 방향으로 이동
    void DoChase(AActor* Player);
    // 복귀 - SpawnLocation으로 이동
    void DoReturn();

    // 플레이어 탐색
    AActor* FindPlayer();
};