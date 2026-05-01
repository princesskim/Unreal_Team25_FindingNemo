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

	TelegraphMaterial = nullptr;
	TelegraphMID      = nullptr;
	
	// ========== 컴포넌트 이동 관련 초기화 ==========
	MovementComp->MaxSpeed = NormalSpeed;
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->OnComponentHit.AddDynamic(this, &ABaseEnemy::CheckChargeOverlap);
																					// AddDynamic : Hit 하면 기다렸다가 원하는 함수를 실행
																					// UFUNCTION() 되어 있어야 함
																					// 함수 시그니처 필요
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
	
	// Emissive true Test
	/*
	TelegraphMID = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
	if (TelegraphMID)
	{
		TelegraphMID->SetVectorParameterValue(TEXT("Color"), FLinearColor::Red);
		TelegraphMID->SetScalarParameterValue(TEXT("Progress"), 1.0f);
	}
	*/
	GetWorldTimerManager().SetTimer(
		TelegraphTimerHandle,this,
		&ABaseEnemy::ExitTelegraphState,
		TelegraphDuration,false);
}

void ABaseEnemy::ExitTelegraphState()
{
	if (!IsAlive()) return;
	
	// Emissive false Test
	/*
	if (TelegraphMID)
	{
		TelegraphMID->SetScalarParameterValue(TEXT("Progress"), 0.0f);
	}
	*/

	SetState(EEnemyState::Charging);
	
	EnterChargingState();														// Charge 상태로 바로 전환
}

void ABaseEnemy::EnterChargingState()											// ChargeDuration 만큼 시간이 흐르는 중에, 돌진하며 플레이어와 충돌 감지
{
	if (!IsAlive()) return;														// 텔레그래프 완료 시점 플레이어 방향 고정
																				// 이후 플레이어가 피해도 고정 방향으로 돌진
	
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;
	
	FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator TargetRot = Dir.Rotation();
	SetActorRotation(TargetRot);
	
	MovementComp->MaxSpeed = ChargeSpeed;
	MovementComp->Velocity = Dir * ChargeSpeed;
	
	
	GetWorldTimerManager().SetTimer(
		ChargeTimerHandle, this,
		&ABaseEnemy::ExitChargingState,
		ChargeDuration, false);
	
																				// 이제 자동 감지라서 '반복 감지 타이머' 없어도 됨
}

void ABaseEnemy::CheckChargeOverlap(UPrimitiveComponent* HitComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	FVector NormalImpulse, 
	const FHitResult& Hit)
{
																				// 부딪힐 때 비벼지니까 
																					// 가능한 방법 1. 데미지를 진짜 조금씩 줘서 HP 부드럽게 깎이게 (채택!!!)
																					// 가능한 방법 2. 타이머로 제약 걸어서 이 함수가 몇 초에 한번씩만 불리게 하거나
																					// 아예 Hit 말고 콜리전 추가해서 하는 방법이 있지만, 빈공간 때문에 어색할 수도.
																				
	//if (CurrentState != EEnemyState::Charging) return;
	
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Hit!"));
	
	if (AMarinPlayer* Player = Cast<AMarinPlayer>(OtherActor))
	{
		GetWorldTimerManager().ClearTimer(ChargeTimerHandle);
			
		Player->ApplyDamage(ChargeDamage, this);
																				// 충돌하면 ChargeDuration 남았어도, 즉시 경직으로 전환
		ExitChargingState();
		return;
	}
}

void ABaseEnemy::ExitChargingState()
{
	GetWorldTimerManager().ClearTimer(ChargeOverlapTimerHandle);				// 반복 타이머 끄기
	
	MovementComp->Velocity = FVector::ZeroVector;
	MovementComp->MaxSpeed = NormalSpeed;
	
	if (!IsAlive()) return;
	
	SetState(EEnemyState::Stunned);												// Stunned 상태로 바로 전환
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
	
	// Telegraph 마테리얼 끄기
	
	SetState(NewState);
}

// =============================================================
// 텔레그래프 데칼
// =============================================================

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentState == EEnemyState::Telegraph) UpdateTelegraphDecal();
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
	Super::OnDeath();
}