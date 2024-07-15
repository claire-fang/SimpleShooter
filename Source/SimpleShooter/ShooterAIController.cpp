// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ShooterCharacter.h"

void AShooterAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
}

//void AShooterAIController::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
//
//	if (LineOfSightTo(Player))
//	{
//		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());
//		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownLocation"), Player->GetActorLocation());
//	}
//	else
//	{
//		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
//	}
//
//
//}

bool AShooterAIController::IsDead() const
{
	AShooterCharacter* ControlledCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ControlledCharacter)
	{
		return ControlledCharacter->IsDead();
	}
	return true;
}