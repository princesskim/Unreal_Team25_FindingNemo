#include "JellyfishTrap.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PawnMovementComponent.h"

AJellyfishTrap::AJellyfishTrap()
{
    TouchDamage = 15;
    DetectionRange = 600.0f;
    ChaseRange = 1200.0f;
    PatrolRadius = 400.0f;
    CurrentState = EJellyfishTrapState::Patrol;

    KnockbackStrength = 500.0f;
    HitFlashDuration = 0.15f;
    DeathParticle = nullptr;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    SetRootComponent(CapsuleComponent);

    CapsuleComponent->OnComponentBeginOverlap.AddDynamic(
        this, &AJellyfishTrap::OnJellyfishTrapOverlap);
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
    // �׽�Ʈ �α�
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
        FString::Printf(TEXT("JellyfishTrap Hit! Damage: %.1f"), Amount));
    // ���� ������
    StartHitFlash();

    // �˹� : ������ ���⿡�� �з���
    if (Causer)
    {
        FVector KnockbackDir = (GetActorLocation() - Causer->GetActorLocation()).GetSafeNormal();
        // APawn�� LaunchPawn ��� ���� velocity ����
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
        // ���͸��� �۾� �ʿ�
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

// �ǰ� ���� �����·� ������
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