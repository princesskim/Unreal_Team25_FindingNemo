#include "SpikeTrapItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ASpikeTrapItem::ASpikeTrapItem()
{
	SpikeDamage = 10.0f;

	ItemType = "Spkie";
}

void ASpikeTrapItem::ActivateItem(AActor* Activator)
{
	TArray<AActor*> OverlappingActors;
	SpikeCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				SpikeDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}
}