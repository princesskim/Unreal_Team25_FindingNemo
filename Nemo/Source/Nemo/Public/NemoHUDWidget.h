// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NemoHUDWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class NEMO_API UNemoHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHPBar(float CurrentHP, float MaxHP);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;
	
};
