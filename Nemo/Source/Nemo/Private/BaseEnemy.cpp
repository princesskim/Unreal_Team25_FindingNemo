// BaseEnemy.cpp

#include "BaseEnemy.h"
#include "MarinPlayer.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABaseEnemy::ABaseEnemy()
{
	// ============== 상태 초기화 ==============
	CurrentState = EEnemyState::Idle;
	
	// ============= 파라미터 초기화 =============
	TelegraphDecalRadius = 100.f;
	TelegraphDuration = 1.5f;
	
	ChargeDuration = 0.4f;
	ChargeDamage = 10.f;
	
	StunDuration = 1.0f;
	StunMultiplier = 1.0f;
	
	bIsStunned = false;
	
	TelegraphStartTime = 0.f;
	
	// =============== Decal 생성 ===============
	TelegraphDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("TelegraphDecal"));
	TelegraphDecal->SetupAttachment(RootComponent);
	
	TelegraphDecal->SetVisibility(false);													// 기본 숨김
	TelegraphDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));		// 바닥 투영
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	TelegraphDecal->DecalSize = FVector(TelegraphDecalRadius);
	
	if (TelegraphMaterial)
	{
		TelegraphMID = TelegraphDecal->CreateDynamicMaterialInstance();						// 머테리얼 인스턴스 한 번만 생성
	}
}

// =============================================================
// 상태 체크
// =============================================================

void ABaseEnemy::SetState(EEnemyState NewState)
{
	if (CurrentState == NewState) return;
	CurrentState = NewState;
}

EEnemyState ABaseEnemy::GetEnemyState() const
{
	return CurrentState;
}


float ABaseEnemy::GetTelegraphRatio() const
{
	if (CurrentState != EEnemyState::Telegraph) return 0.f;
	const float TimePassed = GetWorld()->GetTimeSeconds() - TelegraphStartTime;
	return FMath::Clamp(TimePassed/TelegraphDuration, 0.f, 1.f);
}


// =============================================================
// 스테이트머신 흐름
// =============================================================

void ABaseEnemy::EnterTelegraphState()
{
	if (!IsAlive() || CurrentState != EEnemyState::Idle) return;
	
	SetState(EEnemyState::Telegraph);
	TelegraphStartTime = GetWorld()->GetTimeSeconds();
		
	TelegraphDecal->SetVisibility(true);
	
	GetWorldTimerManager().SetTimer(
		TelegraphTimerHandle,this,
		&ABaseEnemy::ExitTelegraphState,
		TelegraphDuration,false);
}

void ABaseEnemy::ExitTelegraphState()
{
	if (!IsAlive()) return;
	
	TelegraphDecal->SetVisibility(false);
	SetState(EEnemyState::Charging);
	
	EnterChargingState();										// Charge 상태로 바로 전환
}

void ABaseEnemy::EnterChargingState()							// ChargeDuration 만큼 시간이 흐르는 중에, 돌진하며 플레이어와 충돌 감지
{
	if (!IsAlive()) return;
	
	GetWorldTimerManager().SetTimer(
		ChargeTimerHandle, this,
		&ABaseEnemy::ExitChargingState,
		ChargeDuration, false);
	
	GetWorldTimerManager().SetTimer(
		ChargeOverlapTimerHandle, this,
		&ABaseEnemy::CheckChargeOverlap,
		0.05f, true);							// 0.05s 간격으로 플레이어 충돌 반복 감지
															// 자식에서 Super::ExecuteCharge() 호출하면
															// 충돌감지 타이머 시작
}

void ABaseEnemy::CheckChargeOverlap()
{
	if (CurrentState != EEnemyState::Charging) return;
	
	//돌진 중 플레이어와 오버랩하면 데이미를 줌
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AMarinPlayer::StaticClass());			// 현재 Enemy와 겹친 Actor를 수집
																						// 두 번째 인자로 클래스 필터 적용 가능
	
	for (AActor* Actor : OverlappingActors)
	{
		if (AMarinPlayer* Player = Cast<AMarinPlayer>(Actor))
		{
			Player->ApplyDamage(ChargeDamage, this);
															// 충돌하면 ChargeDuration 남았어도, 즉시 경직으로 전환
			GetWorldTimerManager().ClearTimer(ChargeTimerHandle); // 타이머 끄기
			ExitChargingState();
			return;
		}
	}
}

void ABaseEnemy::ExitChargingState()
{
	if (!IsAlive()) return;
	
	GetWorldTimerManager().ClearTimer(ChargeOverlapTimerHandle); // 반복 타이머 끄기
	
	SetState(EEnemyState::Stunned);							// Stunned 상태로 바로 전환
	EnterStunnedState(StunDuration * StunMultiplier);
}

void ABaseEnemy::EnterStunnedState(float InStunDuration)
{
	if (!IsAlive()) return;
	
	bIsStunned = true;
	
	GetWorldTimerManager().SetTimer(
		StunTimerHandle, [this]()
		{
			if (IsAlive())
			{
				SetState(EEnemyState::Idle);
				ExitStunnedState();
			}
		},InStunDuration, false);					// StunnedDuration 후 Idle 복귀
																// 특정 함수를 부르는 것이 아니라면, 람다 사용 가능
																// 이 경우, 2번째 인자로 this 넣지 않고, 캡처에 this 넣기
}

void ABaseEnemy::ExitStunnedState()
{
	bIsStunned = false;
}

void ABaseEnemy::ResetEnemyState(EEnemyState NewState)
{
	GetWorldTimerManager().ClearTimer(TelegraphTimerHandle);
	GetWorldTimerManager().ClearTimer(ChargeTimerHandle);
	GetWorldTimerManager().ClearTimer(ChargeOverlapTimerHandle);
	GetWorldTimerManager().ClearTimer(StunTimerHandle);
	
	TelegraphDecal->SetVisibility(false);
	
	SetState(NewState);
}

// =============================================================
// 텔레그래프 데칼
// =============================================================

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentState == EEnemyState::Telegraph)
		UpdateTelegraphDecal();
}

void ABaseEnemy::UpdateTelegraphDecal()
{
	if (!TelegraphMID) return;
	
	// 70% 기준으로 두 단계
	// 0.0 ~ 0.7 : 주황색 (경고 단계)
	// 0.7 ~ 1.0 : 주황 → 빨강으로 부드럽게 변화
	// LerpUsingHSV(ColorA, ColorB, Alpha)
		
	const float Ratio = GetTelegraphRatio();
	const FLinearColor Color = (Ratio < 0.7f) 
		? FLinearColor(1.f, 0.4f, 0.f, 0.6f)
		: FLinearColor::LerpUsingHSV(
		FLinearColor(1.f, 0.4f, 0.f, 0.6f),
		FLinearColor(1.f, 0.0f, 0.f, 0.9f),
		(Ratio - 0.7f) / 0.3f);										// 0.7 ~ 1.0을 0~1로 정규화
		
	TelegraphMID->SetVectorParameterValue(TEXT("Color"),    Color);
	TelegraphMID->SetScalarParameterValue(TEXT("Progress"), Ratio);
}


// =============================================================
// 피격 판정 : 경직 중 + 대시 중인 플레이어 충돌 → 처치
// =============================================================

void ABaseEnemy::OnDamaged(float Amount, AActor* Causer)
{
	/*
	MarinPlayer->StartDash()
			└── Enemy->KillInstantly()
					└── ABaseCreature::ProcessDeath()
							└── OnDeath() 자동 호출
									└── ResetEnemyState(Dead)
	 */
}

void ABaseEnemy::OnDeath()
{
	ResetEnemyState(EEnemyState::Dead);
}