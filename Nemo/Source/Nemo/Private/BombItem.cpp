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
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("[BombItem] Activate / Timer Start"));

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
    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            ABaseCreature* Creature = Cast<ABaseCreature>(Actor);
            if (Creature)
            {
                Creature->ApplyDamage(ExplosionDamage, this);
            }
        }
    }
    DestroyItem();
}


