#include "BombItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ABombItem::ABombItem()
{
	ExplosionDelay = 3.0f;
	ExplosionRadius = 700.0f;
	ExplosionDamage = 20.0f;

	ItemType = "Bomb";
}

void ABombItem::ActivateItem(AActor* Activator)
{
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
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}

	DestroyItem();
}


