// BaseEnemy.cpp

#include "BaseEnemy.h"
#include "MarinPlayer.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "BaseEnemyAI.h"
#include "Kismet/GameplayStatics.h"

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// ========= AI 컨트롤러 자동 연결 =========                                               // 둘 다 APawn에 이미 선언된 멤버 변수임
	AIControllerClass = ABaseEnemyAI::StaticClass();										// 이 Pawn이 생성되면 엔진이 자동으로 ABaseEnemyAI 생성
																							// 그리고 Pawn을 Possess
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;									// AI Controller가 언제 자동으로 붙을지 (옵션 4 선택)
																								// 옵션 1.Disabled : 직접 Possess 해야 함
																								// 옵션 2.PlacedInWorld : 레벨에 “배치된 경우만” AI 생성
																								// 옵션 3.Spawned : 런타임 Spawn된 경우만 AI 생성
																								// 옵션 4.PlacedInWorldOrSpawned : 레벨에 있든, 스폰되든 무조건 AI 붙이기
	/*bUseControllerRotationPitch = false;	
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;*/					// Pawn, MovementComponent, Controller(특히 AIController)가 서로 경쟁적으로 회전을 갱신
															// AI가 자체적으로 회전할 때, bUseControllerRotation~은 false로 해야함.
															// 여기서는 틱마다 SetActorRotation, RInterpTo, 커스텀 회전 로직을 사용함
	
	
	// ============== 상태 초기화 ==============
	CurrentState = EEnemyState::Idle;
	
	// ============ 파라미터 초기화 ============
	TelegraphDecalRadius = 100.f;
	TelegraphDuration = 1.5f;
	
	ChargeDuration = 0.4f;
	ChargeDamage = 10.f;
	NormalSpeed = 300.f;
	ChargeSpeed = 1000.f;
	
	StunDuration = 1.0f;
	StunMultiplier = 1.0f;
	
	bIsStunned = false;
	
	TelegraphStartTime = 0.f;
	
	RotationInterpSpeed = 10.f;
	
	// =============== Decal 생성 ===============
	TelegraphDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("TelegraphDecal"));
	TelegraphDecal->SetupAttachment(RootComponent);
	
	TelegraphDecal->SetVisibility(false);													// 기본 숨김
	TelegraphDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));		// 바닥 투영
	
	TelegraphMaterial = nullptr;
	TelegraphMID      = nullptr;
	
	// ========== 컴포넌트 이동 관련 초기화 ==========
	MovementComp->MaxSpeed = NormalSpeed;
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
	if (!IsAlive()) return;										// 텔레그래프 완료 시점 플레이어 방향 고정
																// 이후 플레이어가 피해도 고정 방향으로 돌진
	
	//UE_LOG(LogTemp, Warning, TEXT("EnterChargingState 호출"));
	
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;
	
	FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator TargetRot = Dir.Rotation();
	SetActorRotation(TargetRot);
	
	MovementComp->MaxSpeed = ChargeSpeed;
	MovementComp->Velocity = Dir * ChargeSpeed;
	//UE_LOG(LogTemp, Warning, TEXT("Velocity 주입 완료"));
	
	
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
			GetWorldTimerManager().ClearTimer(ChargeOverlapTimerHandle); // 반복 타이머 우선적으로 끄기
			GetWorldTimerManager().ClearTimer(ChargeTimerHandle); // 타이머 끄기
			
			Player->ApplyDamage(ChargeDamage, this);
			// 충돌하면 ChargeDuration 남았어도, 즉시 경직으로 전환
			ExitChargingState();
			return;
		}
	}
}

void ABaseEnemy::ExitChargingState()
{
	GetWorldTimerManager().ClearTimer(ChargeOverlapTimerHandle); // 반복 타이머 끄기
	
	MovementComp->Velocity = FVector::ZeroVector;
	MovementComp->MaxSpeed = NormalSpeed;
	
	if (!IsAlive()) return;
	
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
	
	if (CurrentState == EEnemyState::Telegraph) UpdateTelegraphDecal();		// 데칼 업데이트
	
	
	// Enemy가 플레이어를 바라보도록
				// Idle        → 플레이어 방향 추적 + 회전 (위치 고정)
				// Telegraph   → 플레이어 방향 빠른 추적 + 회전 (위치 고정)
				// Charging    → EnterChargingState 시점 방향 고정 (단, 약한 추적) + 직진
				// Stunned     → 회전·이동 없음 (얼음)
				// Dead        → 아무것도 안 함
	
	// 즉, Idle, Telegraph일 때만 메쉬 회전
	// Charging 중엔 약하게 추적
	
	if (CurrentState != EEnemyState::Idle && 
		CurrentState != EEnemyState::Telegraph &&
		CurrentState != EEnemyState::Charging)
		return;
	
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;
	
	const FVector TargetDirection = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const FRotator TargetRotator = TargetDirection.Rotation();
	const FRotator CurrentRotation = GetActorRotation();
	
	float EffectiveRotationSpeed ; // 공격 예고 단계에서 플레이어를 더 빠르게 추적
	
	if (CurrentState == EEnemyState::Telegraph)
	{
		EffectiveRotationSpeed = RotationInterpSpeed * 1.5f;
	}
	else if (CurrentState == EEnemyState::Charging)
	{
		EffectiveRotationSpeed = 1.0f; // 아주 약하게 추적
	}
	else
	{
		EffectiveRotationSpeed = RotationInterpSpeed;
	}
	
	const FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation, 
		TargetRotator, 
		DeltaTime, 
		EffectiveRotationSpeed);
	
	if (FVector::DotProduct(GetActorForwardVector(), TargetDirection) > 0.98f)
	{																			// 거의 목표 방향에 도달하면 보간을 끊고 즉시 정렬
		SetActorRotation(TargetRotator);										// RInterpTo 특성상 정확히 도달하지 않는데, 저속 회전에서 눈에 띔
	}
	else
	{
		SetActorRotation(NewRotation);
	}
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