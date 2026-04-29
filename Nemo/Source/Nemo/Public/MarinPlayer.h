// MarinPlayer.h

#pragma once

#include "CoreMinimal.h"
#include "BaseCreature.h"
#include "MarinPlayer.generated.h"

class UFloatingPawnMovement;	
class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
struct FInputActionValue;

UCLASS()
class NEMO_API AMarinPlayer : public ABaseCreature
{
	GENERATED_BODY()
	
public:
	AMarinPlayer();
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual bool IsDamageImmune() const override;
	
	virtual void BeginPlay() override;
	
	// ============= UI용 상태 조회 =============
	UFUNCTION(BlueprintPure, Category="Dash")
	bool IsOnDashCooldown() const;
	
	UFUNCTION(BlueprintPure, Category="Dash")
	float GetDashCooldownRatio() const;								// @서희
																	// 0.0 : 대시 시작
																	// 1.0 : 대시 끝
	
	
	UFUNCTION(BlueprintPure, Category="Dash")
	bool IsSpeedBoostActive() const;
	
	UFUNCTION(BlueprintPure, Category="SpeedBoost")
	float GetSpeedBoostRemainingTime() const;						// @서희 <- 부스트 남은 시간
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	virtual void OnDeath() override;	
	
	virtual void OnDamaged(float Amount, AActor* Causer) override;
	
	// ================ 컴포넌트 ================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UFloatingPawnMovement> MovementComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCameraComponent> CameraComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> PickupSphereComp;
	
	// ============== 이동 파라미터 ==============
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float SwimSpeed;
	
	// ============== Dash 파라미터 ==============
	UPROPERTY(EditDefaultsOnly, Category="Dash")
	float DashSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category="Dash")
	float DashDuration;

	UPROPERTY(EditDefaultsOnly, Category="Dash")
	float DashCooldownDuration;

	UPROPERTY(EditDefaultsOnly, Category="Dash")
	float DamageImmuneDuration;
	
	// =========== SpeedBoost 파라미터 ===========
	UPROPERTY(EditDefaultsOnly, Category="SpeedBoost")
	float SpeedBoostMultiplier;
	
private:
	// =============== 입력 바인딩 ===============
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()	
	void Look(const FInputActionValue& value);	
	UFUNCTION()
	void Dash(const FInputActionValue& value);
	
	// =============== 이동 방향 ===============
	FVector CurrentMoveDirection;
	
	// ================ Dash 상태 ================
	bool bIsDashing;
	bool bIsDashOnCooldown;
	bool bIsDamageImmune;
	FTimerHandle DashHandle;
	FTimerHandle DashCooldownHandle;
	FTimerHandle DamageImmuneHandle;
	
	// ============= SpeedBoost 상태 =============
	bool  bSpeedBoostActive;
	float SpeedBoostDuration;
	FTimerHandle SpeedBoostTimerHandle;
	
	// =============== Dash 관련 함수 ===============
	void StartDash();
	void EndDash();
	void EndDashImmune();
	void ResetDashCooldown();
	
	// ============ SpeedBoost 관련 함수 ============
	void StartSpeedBoost();
	void EndSpeedBoost();
	
	// =============== 픽업 오버랩 ===============
	UFUNCTION()
	void OnPickupOverlap(UPrimitiveComponent* OverlappedComp,
						 AActor*              OtherActor,
						 UPrimitiveComponent* OtherComp,
						 int32                OtherBodyIndex,
						 bool                 bFromSweep,
						 const FHitResult&    SweepResult);				// 언리얼 OnComponentBeginOverlap 콜백 시그니처
};
