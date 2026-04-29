// BossShark.cpp

#include "BossShark.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "MarinPlayer.h"

ABossShark::ABossShark()
{
	// ============= 파라미터 초기화 =============
	MaxHP = 300.f;
	
	TelegraphDecalRadius = 300.f;
	TelegraphDuration = 3.f;
	
	ChargeDuration = 0.5f;
	ChargeDamage = 20.f;
	NormalSpeed = 250.f;
	ChargeSpeed = 1400.f;
	
	StunDuration = 1.0f;
	
	ChargeRadius         = 300.f;
}

void ABossShark::EnterChargingState()
{
	Super::EnterChargingState();

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;
	
	FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator TargetRot = Dir.Rotation();
	SetActorRotation(TargetRot);
	
	if (UFloatingPawnMovement* Mov =
			FindComponentByClass<UFloatingPawnMovement>())
	{
		Mov->MaxSpeed = ChargeSpeed;
		Mov->Velocity = Dir * ChargeSpeed;
	}
	
	Super::EnterChargingState();							// Charge 타이머와 충돌 감지 반복 타이머 시작!
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
			MarinPlayer->ApplyDamage(ChargeDamage, this);
			GetWorldTimerManager().ClearTimer(ChargeTimerHandle); // 타이머 끄기
			ExitChargingState();
		}
	}
}
