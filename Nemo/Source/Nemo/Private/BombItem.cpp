#include "BombItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCreature.h"

ABombItem::ABombItem()
{
	ExplosionDelay = 3.0f;
	ExplosionRadius = 700.0f;
	ExplosionDamage = 20;

	ItemType = "Bomb";

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->SetupAttachment(Scene);
    ExplosionCollision->SetSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ABombItem::ActivateItem(AActor* Activator)
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("[BombItem] 활성화 / 폭발 타이머 시작"));

	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&ABombItem::Explode,
		ExplosionDelay,
		false
	);
}

void ABombItem::Explode()
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[BombItem] 폭발!"));

    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            // UGameplayStatics::ApplyDamage 대신
            ABaseCreature* Creature = Cast<ABaseCreature>(Actor);
            if (Creature)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("[BombItem] 플레이어 피격 / 데미지: %d"), ExplosionDamage));
                Creature->ApplyDamage(ExplosionDamage, this);
            }
        }
    }
    DestroyItem();
}


