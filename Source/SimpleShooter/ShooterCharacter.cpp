// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"
#include "Components/PostProcessComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	TPVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPV Camera"));
	TPVCamera->SetupAttachment(SpringArm);

	FPVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPV Camera"));
	FPVCamera->SetupAttachment(GetMesh(), TEXT("head"));

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Post Process Component"));
	PostProcessComponent->SetupAttachment(FPVCamera);
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None); // hide the default gun (bone)

	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);

	Health = MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsFPV)
	{
		SetDOF();
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);

	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("TogglePerspective"), EInputEvent::IE_Pressed, this, &AShooterCharacter::TogglePerspective);
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Shoot()
{
	Gun->PullTrigger();
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;
	// UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	if (IsDead())
	{
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if (GameMode)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageApplied;
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

void AShooterCharacter::SetDOF()
{
	FHitResult HitResult;
	FVector StartLocation = FPVCamera->GetComponentLocation();
	FVector EndLocation = StartLocation + HitDistance * FPVCamera->GetForwardVector();

	FCollisionQueryParams IgnoredParams;
	IgnoredParams.AddIgnoredActor(this);
	IgnoredParams.AddIgnoredActor(Gun);

	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, IgnoredParams);

	if (Hit)
	{
		FocalDistance = FVector::Dist(HitResult.Location, StartLocation);
	}
	else
	{
		FocalDistance = 100000;
	}
	// PostProcessComponent->Settings.DepthOfFieldFocalDistance = FocalDistance;
	// UE_LOG(LogTemp, Display, TEXT("Focal Distance: %f"), FocalDistance);
	FPVCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	FPVCamera->PostProcessSettings.DepthOfFieldFocalDistance = FocalDistance;

	FPVCamera->PostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
	FPVCamera->PostProcessSettings.DepthOfFieldFocalRegion = 10;
}

void AShooterCharacter::TogglePerspective()
{
	if (TPVCamera->IsActive())
	{
		TPVCamera->SetActive(false);
		FPVCamera->SetActive(true);
		bUseControllerRotationYaw = true;
		bIsFPV = true;
	}
	else
	{
		TPVCamera->SetActive(true);
		FPVCamera->SetActive(false);
		bUseControllerRotationYaw = false;
		bIsFPV = false;
	}
}
