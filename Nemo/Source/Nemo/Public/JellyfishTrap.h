#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "JellyfishTrap.generated.h"

UENUM(BlueprintType)
enum class EJellyfishTrapState : uint8
{
    Patrol,   // ¼øÂû
    Chase,    // ÃßÀû
    Return    // º¹±Í
};

UCLASS()
class NEMO_API AJellyfishTrap : public ABaseEnemy
{
    GENERATED_BODY()

public:
    AJellyfishTrap();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCapsuleComponent* CapsuleComponent;

    UFUNCTION()
    void OnJellyfishTrapOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfishTrap")
    int32 TouchDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfishTrap")
    float DetectionRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfishTrap")
    float ChaseRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfishTrap")
    float PatrolRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfishTrap|Combat")
    float KnockbackStrength;        // ³Ë¹é °­µµ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfishTrap|Combat")
    float HitFlashDuration;         // ±ôºýÀÓ Áö¼Ó ½Ã°£

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfishTrap|Combat")
    UParticleSystem* DeathParticle; // »ç¸Á ÆÄÆ¼Å¬
    
    FVector SpawnLocation;

    EJellyfishTrapState CurrentState;

    void SetState(EJellyfishTrapState NewState);

protected:
    virtual void OnDeath() override;
    virtual void OnDamaged(float Amount, AActor* Causer) override;

private:
    FTimerHandle HitFlashTimerHandle;

    void StartHitFlash();
    void EndHitFlash();
};