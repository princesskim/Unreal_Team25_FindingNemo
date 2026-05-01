#include "JellyfishTrapAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/PawnMovementComponent.h"

AJellyfishTrapAIController::AJellyfishTrapAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AJellyfishTrapAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    JellyfishTrapOwner = Cast<AJellyfishTrap>(InPawn);

    GetWorld()->GetTimerManager().SetTimer(
        PatrolTimerHandle,
        this,
        &AJellyfishTrapAIController::DoPatrol,
        2.0f,
        true
    );
}

void AJellyfishTrapAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!JellyfishTrapOwner) return;
    if (!JellyfishTrapOwner->IsAlive()) return;

    AActor* Player = FindPlayer();

    switch (JellyfishTrapOwner->CurrentState)
    {
    case EJellyfishTrapState::Patrol:
        if (Player)
        {
            GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);
            JellyfishTrapOwner->SetState(EJellyfishTrapState::Chase);
            DoChase(Player);
        }
        else
        {
            FVector Dir = (PatrolTargetLocation - JellyfishTrapOwner->GetActorLocation()).GetSafeNormal();
            float Dist = FVector::Dist(PatrolTargetLocation, JellyfishTrapOwner->GetActorLocation());

            if (Dist > 50.f)
            {
                JellyfishTrapOwner->GetMovementComponent()->Velocity = Dir * 300.f;
            }
            else
            {
                DoPatrol();
            }
        }
        break;

    case EJellyfishTrapState::Chase:
        if (Player)
        {
            DoChase(Player);
        }
        else
        {
            JellyfishTrapOwner->SetState(EJellyfishTrapState::Return);
        }
        break;

    case EJellyfishTrapState::Return:
        DoReturn();
        if (FVector::Dist(JellyfishTrapOwner->GetActorLocation(),
            JellyfishTrapOwner->SpawnLocation) < 100.0f)
        {
            JellyfishTrapOwner->SetState(EJellyfishTrapState::Patrol);
            GetWorld()->GetTimerManager().SetTimer(
                PatrolTimerHandle,
                this,
                &AJellyfishTrapAIController::DoPatrol,
                2.0f,
                true
            );
        }
        break;
    }
}

void AJellyfishTrapAIController::DoPatrol()
{
    if (!JellyfishTrapOwner) return;

    FVector RandomOffset = FVector(
        FMath::RandRange(-JellyfishTrapOwner->PatrolRadius, JellyfishTrapOwner->PatrolRadius),
        FMath::RandRange(-JellyfishTrapOwner->PatrolRadius, JellyfishTrapOwner->PatrolRadius),
        FMath::RandRange(-400.f, 400.f)
    );

    PatrolTargetLocation = JellyfishTrapOwner->SpawnLocation + RandomOffset;
}

void AJellyfishTrapAIController::DoChase(AActor* Player)
{
    FVector Dir = (Player->GetActorLocation() - JellyfishTrapOwner->GetActorLocation()).GetSafeNormal();
    JellyfishTrapOwner->GetMovementComponent()->Velocity = Dir * 800.f;
}

void AJellyfishTrapAIController::DoReturn()
{
    FVector Dir = (JellyfishTrapOwner->SpawnLocation - JellyfishTrapOwner->GetActorLocation()).GetSafeNormal();
    JellyfishTrapOwner->GetMovementComponent()->Velocity = Dir * 500.f;
}

AActor* AJellyfishTrapAIController::FindPlayer()
{
    if (!JellyfishTrapOwner) return nullptr;

    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return nullptr;

    float Distance = FVector::Dist(
        JellyfishTrapOwner->GetActorLocation(),
        Player->GetActorLocation()
    );

    float RangeToUse = (JellyfishTrapOwner->CurrentState == EJellyfishTrapState::Chase)
        ? JellyfishTrapOwner->ChaseRange
        : JellyfishTrapOwner->DetectionRange;

    return Distance <= RangeToUse ? Player : nullptr;
}