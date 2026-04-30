// MarinPlayer.cpp

#include "MarinPlayer.h"
#include "BaseEnemy.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MarinController.h"
#include "EnhancedInputComponent.h"
#include "NemoGameState.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

AMarinPlayer::AMarinPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MaxHP = 100.f;
	
	// =========== 이동 파라미터 초기화 ===========
	SwimSpeed = 550.f;
	
	// =========== 회전 파라미터 초기화 ===========
	MaxPitchAngle = 60.f;
	LookSensitivity = 0.8f;
	RotationInterpSpeed = 3.f;
	
	// =========== Dash 파라미터 초기화 ===========
	DashSpeed = 2000.f;
	DashDuration = 0.3f;
	DashCooldownDuration = 1.2f;
	DamageImmuneDuration = 0.15f;
	
	// ======= SpeedBoost 파라미터 초기화 =======
	SpeedBoostMultiplier = 1.7f;
	
	// ============== 상태 초기화 ==============
	bIsDashing = false;
	bIsDashOnCooldown = false;
	bIsDamageImmune = false;
	
	bSpeedBoostActive = false;
	SpeedBoostDuration = 2.5f;
	
	CurrentMoveDirection = FVector::ZeroVector;
	
	
	// ============= 컴포넌트 생성 =============
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->MaxSpeed = SwimSpeed;
	MovementComp->Acceleration = 1200.f;
	MovementComp->Deceleration = 800.f;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength        = 400.f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritRoll            = false;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	
	// ============= 위젯 붙이기 =============
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
}

void AMarinPlayer::BeginPlay()
{
	Super::BeginPlay();
}



// =============================================================
// 입력 바인딩
// =============================================================

void AMarinPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMarinController* PlayerController = Cast<AMarinController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMarinPlayer::Move
				);
			}
			if (PlayerController->VirticalMoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->VirticalMoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMarinPlayer::VirticalMove
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMarinPlayer::Look
				);
			}
			if (PlayerController->DashAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DashAction,
					ETriggerEvent::Triggered,
					this,
					&AMarinPlayer::Dash
				);
			}
		}
	}
}

void AMarinPlayer::UpdateOverheadHP()
{
	if (!OverheadWidget) return;
	
	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	
	if (!OverheadWidgetInstance) return;
	
	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverheadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), GetCurrentHP(), GetMaxHP())));
		
	}
}


// =============================================================
// 이동 및 메시 회전
// =============================================================

void AMarinPlayer::Move(const FInputActionValue& value)
{
	if (!Controller) return; 
	
	const FVector2D MoveInput = value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero()) return;
	
	const FRotator ControlRot = GetControlRotation();
	
	FVector ForwardVector = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
	
	if (!FMath::IsNearlyZero(MoveInput.X)) 
	{
		AddMovementInput(ForwardVector, MoveInput.X); 
	}
	if (!FMath::IsNearlyZero(MoveInput.Y)) 
	{
		AddMovementInput(RightVector, MoveInput.Y);
	}
}

void AMarinPlayer::VirticalMove(const FInputActionValue& value)
{
	AddMovementInput(FVector::UpVector, value.Get<float>());
}

void AMarinPlayer::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X * LookSensitivity);
	AddControllerPitchInput(LookInput.Y * LookSensitivity);
}

void AMarinPlayer::Dash(const FInputActionValue& value)
{
	if (!IsAlive() || bIsDashing || bIsDashOnCooldown) return;
	StartDash();
}

void AMarinPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	const FVector Velocity = GetVelocity();
	
	FVector HorizontalVelocity = Velocity;
	HorizontalVelocity.Z = 0.f;												// 수평 속도만 추출
	
	bool bHasHorizontalMovement = !HorizontalVelocity.IsNearlyZero();
	
	if (bHasHorizontalMovement)
	{
		FRotator TargetRotation  = Velocity.Rotation();						// 수평 이동 중 — 실제 "이동 방향(Pitch 포함)"으로 메시 회전
		
		if (TargetRotation.Pitch > 180) TargetRotation.Pitch -= 360.f;		// 언리얼은 Pitch를 0~360으로 표현 (270도 -> -90도로 변경)
		if (TargetRotation.Pitch > MaxPitchAngle) TargetRotation.Pitch = MaxPitchAngle;
		if (TargetRotation.Pitch < -MaxPitchAngle) TargetRotation.Pitch = -MaxPitchAngle;
		
		FRotator NewRotation = FMath::RInterpTo(
			GetActorRotation(),	
			TargetRotation,
			DeltaSeconds,	
			RotationInterpSpeed	
		);
		
		SetActorRotation(NewRotation);
	}	
	else																	// 순수 상하 이동(Q/E) 또는 정지 중
	{																		// Pitch는 점진적으로 수평(0도)으로 복귀, Yaw는 유지
		FRotator CurrentRotation = GetActorRotation();						// 현재 "액터가 바라보는 방향"
		
		if (CurrentRotation.Pitch > 180) CurrentRotation.Pitch -= 360.f;
		if (CurrentRotation.Pitch > MaxPitchAngle) CurrentRotation.Pitch = MaxPitchAngle;
		if (CurrentRotation.Pitch < -MaxPitchAngle) CurrentRotation.Pitch = -MaxPitchAngle;
		
		FRotator TargetRotation = FRotator(0.f, CurrentRotation.Yaw, 0.f);
		
		FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,
			TargetRotation,	
			DeltaSeconds,	
			RotationInterpSpeed	
		);
		SetActorRotation(NewRotation);
	}
	
	CurrentMoveDirection = Velocity.GetSafeNormal();
}



// =============================================================
// Dash
// =============================================================

void AMarinPlayer::StartDash()
{
	bIsDashing = true;
	bIsDashOnCooldown = true;
	bIsDamageImmune = true;
	
	DashCooldownStartTime = GetWorld()->GetTimeSeconds();
	
	const FVector Dir = CurrentMoveDirection.IsNearlyZero() ? GetActorForwardVector() : CurrentMoveDirection;

	MovementComp->Velocity = Dir * DashSpeed;								// 물리 무시하고 즉시 대시 속도로 점프
	MovementComp->MaxSpeed = DashSpeed;										// 이동 컴포넌트가 허용하는 최대 속도 제한을 변경
	
	GetWorldTimerManager().SetTimer(
		DashHandle, this,
		&AMarinPlayer::EndDash,
		DashDuration, false);
	
	GetWorldTimerManager().SetTimer(
		DamageImmuneHandle, this,
		&AMarinPlayer::EndDashImmune,
		DamageImmuneDuration, false);
	
	// 주변 경직된 적 처치 판정
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ABaseEnemy::StaticClass());
	
	for (AActor* Actor : OverlappingActors)
	{
		if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(Actor))
		{
			if (Enemy->GetEnemyState() == EEnemyState::Stunned)
			{
				Enemy->KillInstantly();
			}
		}
	}
}

void AMarinPlayer::EndDash()
{
	bIsDashing = false;
	
	MovementComp->MaxSpeed = bSpeedBoostActive ? SwimSpeed * SpeedBoostMultiplier : SwimSpeed;
																			// Dash가 우선순위 1순위, SpeedBoost가 2순위
	
	GetWorldTimerManager().SetTimer(										// Dash 끝나면 쿨타임 카운트 시작
		DashCooldownHandle, this,
		&AMarinPlayer::ResetDashCooldown,
		DashCooldownDuration, false);
}

void AMarinPlayer::EndDashImmune()
{
	bIsDamageImmune = false;
}

void AMarinPlayer::ResetDashCooldown()
{
	bIsDashOnCooldown = false;
}


bool AMarinPlayer::IsDamageImmune() const
{
	return bIsDamageImmune;
}

bool AMarinPlayer::IsOnDashCooldown() const
{
	return bIsDashOnCooldown;
}

float AMarinPlayer::GetDashCooldownRatio() const
{
	if (!bIsDashOnCooldown) return 0.f;
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float TimePassed = CurrentTime - DashCooldownStartTime;
	return FMath::Clamp(TimePassed/DashCooldownDuration, 0.f, 1.f);
}



// =============================================================
// SpeedBoost
// =============================================================

void AMarinPlayer::StartSpeedBoost()
{
	bSpeedBoostActive = true;
	
	if (!bIsDashing) MovementComp->MaxSpeed = SwimSpeed * SpeedBoostMultiplier;
	
	GetWorldTimerManager().SetTimer(
		SpeedBoostTimerHandle, this,
		&AMarinPlayer::EndSpeedBoost,
		SpeedBoostDuration, false);
}

void AMarinPlayer::EndSpeedBoost()
{
	bSpeedBoostActive = false;
	if (!bIsDashing) MovementComp->MaxSpeed = SwimSpeed;
}

bool AMarinPlayer::IsSpeedBoostActive() const
{
	return bSpeedBoostActive;
}

float AMarinPlayer::GetSpeedBoostRemainingRatio() const
{
	if (!bSpeedBoostActive) return 0.f;
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float TimePassed = CurrentTime - SpeedBoostStartTime;
	return FMath::Clamp(1.f - TimePassed/SpeedBoostDuration, 0.f, 1.f);
}



// =============================================================
// 피격 · 사망 오버라이드
// =============================================================

void AMarinPlayer::OnDamaged(float Amount, AActor* Causer)
{
	if (AMarinController* PlayerController = Cast<AMarinController>(GetController()))
	{
		// @서희 : 위젯 BP에서 Tick마다 GetHealthPercent() 호출해서 HP바 업데이트
		// @서희 : 피격 연출
	}
}

void AMarinPlayer::OnDeath()
{
	ANemoGameState* NemoGameState = GetWorld() ? GetWorld()->GetGameState<ANemoGameState>() : nullptr;
	if (NemoGameState)
	{
		NemoGameState->OnGameOver();
	}
}
