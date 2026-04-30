#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SpikeTrapItem.generated.h"

UCLASS()
class NEMO_API ASpikeTrapItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ASpikeTrapItem();

	USphereComponent* SpikeCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 SpikeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DamageInterval;  // 초당 데미지

	FTimerHandle SpikeDamageTimerHandle;

	AActor* CurrentActivator;

	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	void ApplySpikeDamage();

	virtual void ActivateItem(AActor* Activator) override;
};
