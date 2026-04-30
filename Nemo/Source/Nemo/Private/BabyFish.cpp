// Fill out your copyright notice in the Description page of Project Settings.


#include "BabyFish.h"
#include "NemoGameState.h"

ABabyFish::ABabyFish()
{
	ItemType = "Fish";
	PointValue = 1;
}

void ABabyFish::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player Gained %f Points"),PointValue));
		if (UWorld* World = GetWorld())
		{
			if (ANemoGameState* GameState = World->GetGameState<ANemoGameState>())
			{
				GameState->AddScore(PointValue);
			}
		}
		DestroyItem();
	}
}
