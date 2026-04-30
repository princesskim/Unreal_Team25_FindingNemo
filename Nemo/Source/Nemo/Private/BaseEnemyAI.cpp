// BaseEnemyAI.cpp

#include "BaseEnemyAI.h"
#include "BaseEnemy.h"
#include "Kismet/GameplayStatics.h"

ABaseEnemyAI::ABaseEnemyAI()
{
	PrimaryActorTick.bCanEverTick = true;
	
	PlayerDetectionRadius = 1200.f;
	AttackCooldownTime = 2.5f;
	bIsAttackBlocked = false;
	PossessedEnemy = nullptr;
	
	bSetControlRotationFromPawnOrientation = false;				// true: Pawn → Controller 회전 동기화
																// false: Controller → Pawn 방향 제어 유지
}

void ABaseEnemyAI::OnPossess(APawn* InPawn)						// 언리얼이 넘겨주는 Pawn
{
	Super::OnPossess(InPawn);									// 부모클래스 OnPossess 먼저 실행
																// 넘겨받은 Pawn을 그대로 부모에게 줘서 초기화
	
	PossessedEnemy = Cast<ABaseEnemy>(InPawn);					// Tick마다 Cast하지 않고, 여기서 1회 진행
}

void ABaseEnemyAI::OnUnPossess()
{
	GetWorldTimerManager().ClearTimer(AttackCooldownTimerHandle);
	bIsAttackBlocked = false;
	PossessedEnemy = nullptr;
	
	Super::OnUnPossess();										// 우리꺼 정리 먼저 하고, 부모함수 호출
}

void ABaseEnemyAI::Tick(float DeltaSeconds)						// 새 공격 조건 : 대기(Idle) 상태 + 감지 영역 내 + 쿨다운 시간 벗어남
{
	Super::Tick(DeltaSeconds);
	
	if (!PossessedEnemy) return;
	if (bIsAttackBlocked) return;
	if (PossessedEnemy->GetEnemyState() != EEnemyState::Idle) return;	// 이미 사이클 중
	if (GetDistanceToPlayer() > PlayerDetectionRadius) return;			// 감지 영역 밖
	
	// 새 공격을 위한 조건을 모두 만족한다면 텔레그래프 시작
	PossessedEnemy->EnterTelegraphState();								// 새 공격 사이클 시작
	bIsAttackBlocked = true;											// 중복 공격 막는 플래그
	
	const float TotalDelay =	PossessedEnemy->GetTelegraphDuration() +
								PossessedEnemy->GetChargeDuration() + 
								PossessedEnemy->GetStunDuration() + 
								AttackCooldownTime;
	
	GetWorldTimerManager().SetTimer(
		AttackCooldownTimerHandle, this,
		&ABaseEnemyAI::OnCooldownFinished, 
		TotalDelay, false);
}

void ABaseEnemyAI::OnCooldownFinished()
{
	bIsAttackBlocked = false;
}

float ABaseEnemyAI::GetDistanceToPlayer() const
{
	if (!PossessedEnemy) return TNumericLimits<float>::Max();
	
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return TNumericLimits<float>::Max();
	
	return FVector::Dist(
		Player->GetActorLocation(), 
		PossessedEnemy->GetActorLocation());
}
