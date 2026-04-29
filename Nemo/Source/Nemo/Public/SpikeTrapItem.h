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

	virtual void ActivateItem(AActor* Activator) override;
	void Spike();
	
};
