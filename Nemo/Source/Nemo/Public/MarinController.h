// MarinController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MarinController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UNemoHUDWidget;

UCLASS()
class NEMO_API AMarinController : public APlayerController
{
	GENERATED_BODY()
public:
	AMarinController();
	
	// ============== IMC & IA ==============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> VirticalMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> DashAction;
	
	
	// ============== 메인 메뉴 위젯 ==============
																					// 게임 시작 전 표시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MainMenu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MainMenu")
	UUserWidget* MainMenuWidgetInstance;
	
	UFUNCTION(BlueprintCallable, Category="MainMenu")
	void ShowMainMenu(bool bIsRestart);
	
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();

	
	// ============== HUD 위젯 ==============
																					// 인게임 중 항상 떠있는 UI (HP바, 비늘 카운터 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
	TSubclassOf<UNemoHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HUD")
	UNemoHUDWidget* HUDWidgetInstance;

	UFUNCTION(BlueprintCallable, Category="HUD")
	void ShowGameHUD();

	UFUNCTION(BlueprintPure, Category="HUD")
	UNemoHUDWidget* GetHUDWidget() const;




	
	FTimerHandle NarrationTimerHandle;

	void ShowNarrationPanel();
	void HideNarrationPanel();
	
	void UpdateHUDHP(float CurrentHP, float MaxHP);
	void SetNarrationTextByStage(int32 Stage);
	
	// ============== 레벨 클리어 위젯 ==============
																					// 웨이브 클리어 시 표시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LevelClear")
	TSubclassOf<UUserWidget> LevelClearWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LevelClear")
	UUserWidget* LevelClearWidgetInstance;

	UFUNCTION(BlueprintCallable, Category="LevelClear")
	void ShowLevelClear();

	
	// ============== 게임오버 위젯 ==============
																					// 플레이어 사망 시 표시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameOver")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GameOver")
	UUserWidget* GameOverWidgetInstance;

	UFUNCTION(BlueprintCallable, Category="GameOver")
	void ShowGameOver();

	
protected:
	virtual void BeginPlay() override;
	
private:
	void ShutAllWidgets();
};
