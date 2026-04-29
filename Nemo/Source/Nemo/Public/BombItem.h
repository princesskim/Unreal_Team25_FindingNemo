#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BombItem.generated.h"

UCLASS()
class NEMO_API ABombItem : public ABaseItem
{
	GENERATED_BODY()
public:
	ABombItem();

	USphereComponent* ExplosionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamage;

	FTimerHandle ExplosionTimerHandle;

	virtual void ActivateItem(AActor* Activator) override;
	void Explode();
};
