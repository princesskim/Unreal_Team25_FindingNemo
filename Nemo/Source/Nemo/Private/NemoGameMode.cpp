// Fill out your copyright notice in the Description page of Project Settings.


#include "NemoGameMode.h"
#include "MarinPlayer.h"

ANemoGameMode::ANemoGameMode()
{
	DefaultPawnClass = AMarinPlayer::StaticClass();
}
