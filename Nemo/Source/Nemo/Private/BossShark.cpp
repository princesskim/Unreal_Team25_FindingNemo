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



void ABossShark::CheckChargeOverlap()
{
	if (GetEnemyState() != EEnemyState::Charging) return;
	
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;
	
	const float Dist = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
	if (Dist <= ChargeRadius)								// 플레이어가 반경 내 거리 안에 있으면 감지
	{
		if (AMarinPlayer* MarinPlayer = Cast<AMarinPlayer>(Player))
		{
			GetWorldTimerManager().ClearTimer(ChargeOverlapTimerHandle); // 반복 타이머 우선적으로 끄기
			GetWorldTimerManager().ClearTimer(ChargeTimerHandle); // 타이머 끄기
			
			MarinPlayer->ApplyDamage(ChargeDamage, this);
			ExitChargingState();
		}
	}
}