// Fill out your copyright notice in the Description page of Project Settings.


#include "NemoHUDWidget.h"
#include "Components/ProgressBar.h"


//HP�� ������Ʈ
void UNemoHUDWidget::UpdateHPBar(float HPPercent)
{
	if (HPBar == nullptr || HPPercent <= 0.0f)
	{
		return;
	}

	HPBar->SetPercent(HPPercent);

}
