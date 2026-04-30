// BaseEnemyAI.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAI.generated.h"

/*
 * [BaseEnemyAI 역할]
 *	1. AI 소유 및 제어
 *	2. Tick 기반 판단 (타겟 감지, 공격 사이클 실행)
 *	3. 상태 관리
 *
 *
 * [사용 방법]
	BP_Lionfish 열기
	→ AI Controller Class : BaseEnemyAI
	→ 레벨에 배치된 BP_Lionfish 선택
		→ 디테일 패널 → AI 카테고리
			→ PlayerDetectionRadius : 1200
			→ AttackCooldownTime    : 2.5
 */


class ABaseEnemy;

UCLASS()
class NEMO_API ABaseEnemyAI : public AAIController
{
	GENERATED_BODY()
	
public:																		// 게임 시스템이 간접적으로 쓰는 객체이기 때문에
	ABaseEnemyAI();															// 외부에서 호출할 일 거의 없음
																			// public은 엔진 훅들로 구성하기
	
	virtual void OnPossess(APawn* InPawn) override;							// AI 컨트롤러가 적 폰에 빙의할 때 자동 호출
	virtual void OnUnPossess() override;									// 빙의 해제될 때 호출 (타이머 해제 등 정리 작업)
	virtual void Tick(float DeltaSeconds) override;
	
protected:																	// 자식에서 값 덮어써야 하는 것들을 protected
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float PlayerDetectionRadius;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float AttackCooldownTime;
	
	TObjectPtr<ABaseEnemy> PossessedEnemy;									// 조종 중인 적 캐싱 - 자식에서 행동로직 관리
																			// AI 컨트롤러 하나가 적 하나를 담당
	
private:
	bool bIsAttackBlocked;													// 중복 공격 방지 (Idle이어도 쿨다운 중일 수 있음)
	
	FTimerHandle AttackCooldownTimerHandle;
	
	float GetDistanceToPlayer() const;										// 타겟 감지를 위한 함수
	void OnCooldownFinished();												// 공격 사이클 진행을 위한 함수
};
