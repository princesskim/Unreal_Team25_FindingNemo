// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"
#include "MarinPlayer.h"

AHealingItem::AHealingItem()
{
	ItemType = "Healing";
	HealAmount = 20.0f;
}

void AHealingItem::BeginPlay()
{
	Super::BeginPlay();
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player Gained %.1f HP"),HealAmount));
		
		if (AMarinPlayer* MarinPlayer = Cast<AMarinPlayer>(Activator))
		{
			MarinPlayer->ApplyHeal(HealAmount,this);
			UE_LOG(LogTemp, Warning, TEXT("Player Gained %.1f HP, CurrentHP : %.1f"), HealAmount, MarinPlayer->GetCurrentHP());
		}
		DestroyItem();
	}
}
