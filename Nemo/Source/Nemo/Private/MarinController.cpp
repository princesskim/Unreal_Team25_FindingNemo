// MarinController.cpp

#include "MarinController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "NemoGameState.h"
#include "NemoGameInstance.h"
#include "Kismet/GameplayStatics.h"
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
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

void AMarinController::ShowMainMenu(bool bIsRestart)
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
		
		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
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
	
	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
}

void AMarinController::ShowGameHUD()
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
		}
	}
}

UUserWidget* AMarinController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AMarinController::ShowLevelClear()
{
}

void AMarinController::ShowGameOver()
{
}
