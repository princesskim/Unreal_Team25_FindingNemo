// MarinController.cpp

#include "MarinController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "NemoGameState.h"
#include "NemoGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

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
			SetNarrationTextByStage(1);
			ShowNarrationPanel();
		}
	}
}

UUserWidget* AMarinController::GetHUDWidget() const
{
	return HUDWidgetInstance;
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

void AMarinController::SetNarrationTextByStage(int32 Stage)
{
	if (!HUDWidgetInstance) return;

	if (UTextBlock* NarrationTextBlock = Cast<UTextBlock>(
		HUDWidgetInstance->GetWidgetFromName(TEXT("NarrationTextBlock"))))
	{
	switch (Stage)
	{
	case 1:
		NarrationTextBlock->SetText(FText::FromString(TEXT("Stage 1\n아이들을 찾아가보자")));
			break;

	case 2:
		NarrationTextBlock->SetText(FText::FromString(TEXT("Stage 2\n지뢰들을 조심하며 나아가자")));
			break;

	case 3:
		NarrationTextBlock->SetText(FText::FromString(TEXT("Stage 3\n상어를 피해 아이들과 집으로 돌아가자")));
		break;

	default:
		NarrationTextBlock->SetText(FText::FromString(TEXT("Stage 1\n아이들을 찾아가보자")));
		break;
	}
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
