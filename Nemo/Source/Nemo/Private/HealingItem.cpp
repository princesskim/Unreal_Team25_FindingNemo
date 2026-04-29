// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"

AHealingItem::AHealingItem()
{
	ItemType = "Healing";
	HealAmount = 20.0f;
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player Gained %d HP"),HealAmount));
		

		//if (Player)
		//{
		//	Player->AddHealth(HealAmount);
		//}
		DestroyItem();
	}
}
