// BossShark.cpp

#include "BossShark.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "MarinPlayer.h"

ABossShark::ABossShark()
{
	// ============= 파라미터 초기화 =============
	MaxHP = 300.f;
	
	TelegraphDecalRadius = 350.f;
	TelegraphDuration = 10.f;
	
	ChargeDuration = 1.2f;
	ChargeDamage = 25.f;
	NormalSpeed = 220.f;
	ChargeSpeed = 4000.f;
	
	StunDuration = 1.5f;
	
	RotationInterpSpeed = 2.0f;
	
	ChargeRadius = 300.f;
}
