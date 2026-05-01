#include "JellyfishTrap.h"
#include "Kismet/GameplayStatics.h"
#include "JellyfishTrapAIController.h"
#include "GameFramework/PawnMovementComponent.h"

AJellyfishTrap::AJellyfishTrap()
{
    TouchDamage = 15;
    DetectionRange = 1500.0f;
    ChaseRange = 3000.0f;
    PatrolRadius = 800.0f;
    CurrentState = EJellyfishTrapState::Patrol;

    KnockbackStrength = 500.0f;
    HitFlashDuration = 0.15f;
    DeathParticle = nullptr;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    SetRootComponent(CapsuleComponent);

    MeshComp->AttachToComponent(CapsuleComponent,
        FAttachmentTransformRules::KeepRelativeTransform);

    CapsuleComponent->OnComponentBeginOverlap.AddDynamic(
        this, &AJellyfishTrap::OnJellyfishTrapOverlap);

    AIControllerClass = AJellyfishTrapAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AJellyfishTrap::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();
}

void AJellyfishTrap::OnJellyfishTrapOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        ABaseCreature* Player = Cast<ABaseCreature>(OtherActor);
        if (Player)
        {
            Player->ApplyDamage(TouchDamage, this);
        }
    }
}

void AJellyfishTrap::OnDeath()
{
    if (DeathParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            DeathParticle,
            GetActorLocation(),
            GetActorRotation()
        );
    }

    Destroy();
}

void AJellyfishTrap::OnDamaged(float Amount, AActor* Causer)
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
        FString::Printf(TEXT("JellyfishTrap Hit! Damage: %.1f"), Amount));

    StartHitFlash();

    if (Causer)
    {
        FVector KnockbackDir = (GetActorLocation() - Causer->GetActorLocation()).GetSafeNormal();
       
        if (UPawnMovementComponent* MoveComp = GetMovementComponent())
        {
            MoveComp->Velocity += KnockbackDir * KnockbackStrength;
        }
    }
}

void AJellyfishTrap::StartHitFlash()
{
    if (MeshComp)
    {
        
        MeshComp->SetScalarParameterValueOnMaterials(TEXT("HitFlash"), 1.0f);
    }

    GetWorld()->GetTimerManager().SetTimer(
        HitFlashTimerHandle,
        this,
        &AJellyfishTrap::EndHitFlash,
        HitFlashDuration,
        false
    );
}


void AJellyfishTrap::EndHitFlash()
{
    if (MeshComp)
    {
        MeshComp->SetScalarParameterValueOnMaterials(TEXT("HitFlash"), 0.0f);
    }
}

void AJellyfishTrap::SetState(EJellyfishTrapState NewState)
{
    CurrentState = NewState;
}