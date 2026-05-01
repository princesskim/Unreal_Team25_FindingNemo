// MarinPlayer.h

#pragma once

#include "CoreMinimal.h"
#include "BaseCreature.h"
#include "MarinPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
struct FInputActionValue;
class UWidgetComponent;

UCLASS()
class NEMO_API AMarinPlayer : public ABaseCreature
{
	GENERATED_BODY()
	
public:
	AMarinPlayer();
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual bool IsDamageImmune() const override;
	
	virtual void BeginPlay() override;

	virtual void OnDamaged(float Amount, AActor* Causer) override;


	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;
	
	
	// ============= UI용 상태 조회 =============
	UFUNCTION(BlueprintPure, Category="Dash")
	bool IsSpeedBoostActive() const;
	
	UFUNCTION(BlueprintPure, Category="Dash")
	bool IsOnDashCooldown() const;
	
	UFUNCTION(BlueprintPure, Category="Dash")
	float GetDashCooldownRatio() const;								// @서희
																	// 0.0 : 대시 시작
																	// 1.0 : 대시 끝
																	// 대시는 지속시간이 짧기 때문에 bar 대신 아이콘 정도가 가능함
	
	UFUNCTION(BlueprintPure, Category="SpeedBoost")
	float GetSpeedBoostRemainingRatio() const;						// @서희
																	// 1.0 : 부스트 시작	[██████████]
																	// 중간				[██████    ]
																	// 0.0 : 부스트 끝	[          ]
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void UpdateOverheadHP();
	
	virtual void OnDeath() override;	
	
	
	
	// ================ 컴포넌트 ================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCameraComponent> CameraComp;
	
	// ============== 이동 파라미터 ==============
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float SwimSpeed;
	
	// ============== 회전 파라미터 ==============
	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxPitchAngle;											// 위아래 시야 최대 각도 제한
	
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float LookSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float RotationInterpSpeed;										// 메시 회전 보간 속도
	
	
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
	
	UPROPERTY(EditDefaultsOnly, Category="SpeedBoost")
	float SpeedBoostDuration;
	
private:
	// =============== 입력 바인딩 ===============
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void VirticalMove(const FInputActionValue& value);
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
	float DashCooldownStartTime;
	
	// ============= SpeedBoost 상태 =============
	bool  bSpeedBoostActive;
	FTimerHandle SpeedBoostTimerHandle;
	float SpeedBoostStartTime;
	
	// =============== Dash 관련 함수 ===============
	void StartDash();
	void EndDash();
	void EndDashImmune();
	void ResetDashCooldown();
	
	// ============ SpeedBoost 관련 함수 ============
	void StartSpeedBoost();
	void EndSpeedBoost();
};
