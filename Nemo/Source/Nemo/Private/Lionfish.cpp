// Lionfish.cpp

#include "Lionfish.h"
#include "MarinPlayer.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"

ALionfish::ALionfish()
{
	// ============= 파라미터 초기화 =============
	MaxHP = 30.f;
	
	TelegraphDecalRadius = 120.f;
	TelegraphDuration = 3.f;
	
	ChargeDuration = 0.8f;
	ChargeDamage = 10.f;
	NormalSpeed = 350.f;
	ChargeSpeed = 2000.f;
	
	StunDuration = 0.6f;
	
	RotationInterpSpeed = 4.0f;
	
	
}