// MarinController.cpp

#include "MarinController.h"
#include "MarinPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "NemoHUDWidget.h"
#include "NemoGameState.h"
#include "NemoGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

AMarinController::AMarinController()
      : InputMappingContext(nullptr),
		MoveAction(nullptr), 
		LookAction(nullptr), 
		DashAction(nullptr),
		MainMenuWidgetClass(nullptr), 
		MainMenuWidgetInstance(nullptr),
		HUDWidgetClass(nullptr), 
		HUDWidgetInstance(nullptr), 
		LevelClearWidgetClass(nullptr), 
		LevelClearWidgetInstance(nullptr), 
		GameOverWidgetClass(nullptr), 
		GameOverWidgetInstance(nullptr)
{
}


void AMarinController::BeginPlay()
{
	Super::BeginPlay();
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem 
			= LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MainMenuLevel"))
	{
		if (UNemoGameInstance* GI = Cast<UNemoGameInstance>(GetGameInstance()))
		{
			GI->ResetGameData();
		}

		ShowMainMenu(false);
	}
}

void AMarinController::ShutAllWidgets()
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	// 이미 클리어가 떠 있으면 제거
	if (LevelClearWidgetInstance)
	{
		LevelClearWidgetInstance->RemoveFromParent();
		LevelClearWidgetInstance = nullptr;
	}

	// 이미 게임오버가 켜져있다면 제거
	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->RemoveFromParent();
		GameOverWidgetInstance = nullptr;
	}

}

void AMarinController::ShowMainMenu(bool bIsRestart)
{

	ShutAllWidgets();

	// 메뉴 UI 생성
	if (MainMenuWidgetClass)
	{		
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();
			
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void AMarinController::StartGame()
{
	if (UNemoGameInstance* NemoGameInstance = Cast<UNemoGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		NemoGameInstance->CurrentLevelIndex = 0;
		NemoGameInstance->TotalScore = 0;
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), FName("Level_Design_Demo_Map"));
}

void AMarinController::ShowGameHUD()
{
	ShutAllWidgets();

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
			
			ANemoGameState* NemoGameState = GetWorld() ? GetWorld()->GetGameState<ANemoGameState>() : nullptr;
			if (NemoGameState)
			{
				NemoGameState->UpdateHUD();
			}
			
			ShowNarrationPanel();
		}
	}
}

UUserWidget* AMarinController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AMarinController::UpdateHUDWidget()
{
	if (UNemoHUDWidget* NemoHUDWidget = Cast<UNemoHUDWidget>(HUDWidgetInstance))
	{
		if (AMarinPlayer* TempPlayer = Cast<AMarinPlayer>(GetPawn()))
		{
			float CurrentHPPercent = TempPlayer->GetHealthPercent();
			NemoHUDWidget->UpdateHPBar(CurrentHPPercent);
		}
	}

}

void AMarinController::ShowNarrationPanel()
{
	if (!HUDWidgetInstance) return;
	
	UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("PlayNarrationAnimation"));
	if (PlayAnimFunc)
	{
		HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
	}

	GetWorld()->GetTimerManager().SetTimer(
		NarrationTimerHandle,
		this,
		&AMarinController::HideNarrationPanel,
		5.0f,
		false
	);
}


void AMarinController::HideNarrationPanel()
{
	if (!HUDWidgetInstance) return;

	if (UWidget* Panel = HUDWidgetInstance->GetWidgetFromName(TEXT("NarrationPanel")))
	{
		Panel->SetRenderOpacity(0.0f);
	}
}

void AMarinController::ShowLevelClear()
{
	ShutAllWidgets();

	if (LevelClearWidgetClass)
	{
		LevelClearWidgetInstance = CreateWidget<UUserWidget>(this, LevelClearWidgetClass);
		if (LevelClearWidgetInstance)
		{
			LevelClearWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void AMarinController::ShowGameOver()
{

	ShutAllWidgets();

	if (GameOverWidgetClass)
	{
		GameOverWidgetInstance = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		if (GameOverWidgetInstance)
		{
			GameOverWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}
