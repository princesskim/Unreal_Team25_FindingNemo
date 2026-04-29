// MarinController.cpp

#include "MarinController.h"
#include "EnhancedInputSubsystems.h"

AMarinController::AMarinController()
	: MoveAction(nullptr), LookAction(nullptr), DashAction(nullptr)
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
}
