// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BabyFish.generated.h"

/**
 * 
 */
UCLASS()
class NEMO_API ABabyFish : public ABaseItem
{
	GENERATED_BODY()

public:
	ABabyFish();

	virtual void ActivateItem(AActor* Activator) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Point")
	float PointValue;
	
};
