// Fill out your copyright notice in the Description page of Project Settings.


#include "KilledEmAllGameMode.h"

void AKilledEmAllGameMode::PawnKilled(APawn* DeadPawn)
{
	Super::PawnKilled(DeadPawn);

	APlayerController* PlayerController = Cast<APlayerController>(DeadPawn->GetController());

	if (PlayerController)
	{
		PlayerController->GameHasEnded(nullptr, false);
	}
}