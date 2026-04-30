// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorSpawnRow.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EActorType : uint8
{
	Item      UMETA(DisplayName = "Item"),    
	Creature   UMETA(DisplayName = "Creature"), 
	Target     UMETA(DisplayName = "Target"),   
	
};

USTRUCT(BlueprintType)
struct FActorSpawnRow : public FTableRowBase
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActorName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnWeight;


};