// Fill out your copyright notice in the Description page of Project Settings.


#include "NemoHUDWidget.h"
#include "Components/ProgressBar.h"


//HP바 업데이트
void UNemoHUDWidget::UpdateHPBar(float CurrentHP, float MaxHP)
{
	if (HPBar == nullptr || MaxHP <= 0.0f)
	{
		return;
	}

	float Percent = CurrentHP / MaxHP;

	Percent = FMath::Clamp(Percent, 0.0f, 1.0f);

	HPBar->SetPercent(Percent);

}
