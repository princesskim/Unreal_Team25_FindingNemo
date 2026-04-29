// MarinPlayer.cpp

#include "MarinPlayer.h"

AMarinPlayer::AMarinPlayer()
{
}

void AMarinPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool AMarinPlayer::IsDamageImmune() const
{
	return Super::IsDamageImmune();
}

void AMarinPlayer::BeginPlay()
{
	Super::BeginPlay();
}

bool AMarinPlayer::IsOnDashCooldown() const
{
	return bIsDashOnCooldown;
}

float AMarinPlayer::GetDashCooldownRatio() const
{
	return 0.f; // 수정 예정
}

bool AMarinPlayer::IsSpeedBoostActive() const
{
	return bSpeedBoostActive;
}

float AMarinPlayer::GetSpeedBoostRemainingTime() const
{
	return 0.f; // 수정 예정
}

void AMarinPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMarinPlayer::OnDeath()
{
	Super::OnDeath();
}

void AMarinPlayer::OnDamaged(float Amount, AActor* Causer)
{
	Super::OnDamaged(Amount, Causer);
}

void AMarinPlayer::Move(const FInputActionValue& value)
{
}

void AMarinPlayer::Look(const FInputActionValue& value)
{
}

void AMarinPlayer::Dash(const FInputActionValue& value)
{
}

void AMarinPlayer::StartDash()
{
}

void AMarinPlayer::EndDash()
{
}

void AMarinPlayer::EndDashImmune()
{
}

void AMarinPlayer::ResetDashCooldown()
{
}

void AMarinPlayer::StartSpeedBoost()
{
}

void AMarinPlayer::EndSpeedBoost()
{
}

void AMarinPlayer::OnPickupOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
}


