#include "JellyfishTrapAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

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

    UNavigationSystemV1* NavSystem =
        UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem) return;

    FNavLocation RandomLocation;
    NavSystem->GetRandomReachablePointInRadius(
        JellyfishTrapOwner->SpawnLocation,
        JellyfishTrapOwner->PatrolRadius,
        RandomLocation
    );

    MoveToLocation(RandomLocation.Location);
}

void AJellyfishTrapAIController::DoChase(AActor* Player)
{
    MoveToActor(Player);
}

void AJellyfishTrapAIController::DoReturn()
{
    MoveToLocation(JellyfishTrapOwner->SpawnLocation);
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