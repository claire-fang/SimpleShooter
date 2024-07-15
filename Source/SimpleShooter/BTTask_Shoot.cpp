// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Shoot.h"
#include "AIController.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = "Shoot";
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!OwnerComp.GetAIOwner())
	{
		return EBTNodeResult::Failed;
	}

	AShooterCharacter* AI = Cast<AShooterCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (!AI || !Player || Player->IsDead()) // AI stops shooting after the player is dead
	{
		return EBTNodeResult::Failed;
	}

	AI->Shoot();

	return EBTNodeResult::Succeeded;
}