// BaseCreature.cpp

#include "BaseCreature.h"

ABaseCreature::ABaseCreature()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCreature::ApplyDamage(float Amount, AActor* Causer)
{
}

void ABaseCreature::ApplyHeal(float Amount)
{
}

bool ABaseCreature::IsAlive() const
{
	return !bIsDead;
}

bool ABaseCreature::IsDamageImmune() const
{
	return false;
}

void ABaseCreature::KillInstantly()
{
}

float ABaseCreature::GetCurrentHP() const
{
	return CurrentHP;
}

float ABaseCreature::GetMaxHP() const
{
	return MaxHP;
}

float ABaseCreature::GetHealthPercent() const
{
	return (MaxHP > 0.f) ? FMath::Clamp(CurrentHP / MaxHP * 100, 0.f, 100.f) : 0;
}

void ABaseCreature::ProcessDeath(AActor* Causer)
{
}

void ABaseCreature::OnDeath()
{
}

void ABaseCreature::OnDamaged(float Amount, AActor* Causer)
{
}

void ABaseCreature::BeginPlay()
{
	Super::BeginPlay();
	
}