// Lionfish.cpp

#include "Lionfish.h"
#include "MarinPlayer.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"

ALionfish::ALionfish()
{
	// ============= 파라미터 초기화 =============
	MaxHP = 30.f;
	
	TelegraphDecalRadius = 100.f;
	TelegraphDuration = 1.5f;
	
	ChargeDuration = 0.35f;
	ChargeDamage = 10.f;
	NormalSpeed = 400.f;
	ChargeSpeed = 1800.f;
	
	StunDuration = 1.0f;
}

void ALionfish::EnterChargingState()						// 텔레그래프 완료 시점 플레이어 방향 고정
{															// 이후 플레이어가 피해도 고정 방향으로 돌진
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;									// 현재 월드에서 0번 플레이어의 Pawn을 가져오는 함수
	
	FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator TargetRot = Dir.Rotation();
	SetActorRotation(TargetRot);							// 돌진할 방향을 바라보도록
	
	if (UFloatingPawnMovement* Mov =
			FindComponentByClass<UFloatingPawnMovement>())	// FindComponentByClass<T>()
	{														// 현재 Actor에 붙어 있는 컴포넌트 중 타입 T를 찾아서 반환
		
		Mov->MaxSpeed = ChargeSpeed;						// 스칼라
		Mov->Velocity = Dir * ChargeSpeed;					// 벡터
	}
	
	Super::EnterChargingState();							// Charge 타이머와 충돌 감지 반복 타이머 시작!
}
