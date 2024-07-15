// Fill out your copyright notice in the Description page of Project Settings.


#include "KilledEmAllGameMode.h"
#include "EngineUtils.h"
#include "ShooterAIController.h"

void AKilledEmAllGameMode::PawnKilled(APawn* DeadPawn)
{
	Super::PawnKilled(DeadPawn);

	APlayerController* PlayerController = Cast<APlayerController>(DeadPawn->GetController());

	if (PlayerController)
	{
		EndGame(false);
		return;
	}
	for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
	{
		if (!Controller->IsDead())
		{
			return;
		}
	}
	EndGame(true);
}

void AKilledEmAllGameMode::EndGame(bool bIsPlayerWinner)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		/*if (Controller->IsPlayerController() && bIsPlayerWinner)
		{
			Controller->GameHasEnded(nullptr, true);
		}
		else if (!Controller->IsPlayerController() && !bIsPlayerWinner)
		{
			Controller->GameHasEnded(nullptr, true);
		}
		else
		{
			Controller->GameHasEnded(nullptr, false);
		}*/
		bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}