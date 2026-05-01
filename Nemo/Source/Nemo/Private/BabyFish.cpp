// Fill out your copyright notice in the Description page of Project Settings.


#include "BabyFish.h"
#include "NemoGameState.h"

ABabyFish::ABabyFish()
{
	ItemType = "Fish";
	PointValue = 132;
	PrimaryActorTick.bCanEverTick = true;
}

void ABabyFish::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player Gained %f Points"),PointValue));
		if (UWorld* World = GetWorld())
		{
			if (ANemoGameState* GameState = World->GetGameState<ANemoGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnBabyFishCollected();
			}
		}
		DestroyItem();
	}
}

void ABabyFish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += DeltaTime;


	if (CurrentTime >= ChangeTargetTime)
	{
		SetNewTarget();
		CurrentTime = 0.f;
	}


	FVector CurrentLocation = GetActorLocation();


	FVector NewLocation = FMath::VInterpTo(
		CurrentLocation,
		TargetLocation,
		DeltaTime,
		1.5f 
	);

	SetActorLocation(NewLocation);

	
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

	if (!Direction.IsNearlyZero())
	{
		FRotator TargetRotation = Direction.Rotation();

		FRotator NewRotation = FMath::RInterpTo(
			GetActorRotation(),
			TargetRotation,
			DeltaTime,
			2.0f
		);

		SetActorRotation(NewRotation);
	}
}

void ABabyFish::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	SetNewTarget();
}

void ABabyFish::SetNewTarget()
{
	FVector RandomOffset = FVector(
		FMath::RandRange(-MoveRadius, MoveRadius),
		FMath::RandRange(-MoveRadius, MoveRadius),
		FMath::RandRange(-50.f, 50.f) 
	);

	TargetLocation = StartLocation + RandomOffset;
}

