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
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	void SetNewTarget();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Point")
	float PointValue;


	FVector StartLocation;
	FVector TargetLocation;

	float MoveSpeed = 100.f;
	float MoveRadius = 500.f;

	float ChangeTargetTime = 2.0f;
	float CurrentTime = 0.f;
	
};
