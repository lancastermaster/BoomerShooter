// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/SceneComponent.h"
#include "Gun.h"
#include "Camera/CameraComponent.h"
#include "Containers/Array.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera -> SetupAttachment(RootComponent);

	WeaponSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Spawn Point"));
	WeaponSpawn -> SetupAttachment(Camera);

	bShouldFire = true;
	bFireButtonPressed = false;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Mana = MaxMana;
	
	SpawnGuns();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bFireButtonPressed)
	{
		StartFireTimer();
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Move Forward"), this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"), this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ABaseCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Look Right"), this, &ABaseCharacter::LookRight);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ABaseCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &ABaseCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ABaseCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("NextWeapon"), EInputEvent::IE_Pressed, this, &ABaseCharacter::IncreaseActiveIndex);
	PlayerInputComponent->BindAction(TEXT("PreviousWeapon"), EInputEvent::IE_Pressed, this, &ABaseCharacter::DecreaseActiveIndex);
}

void ABaseCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void ABaseCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void ABaseCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::LookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::Attack()
{
	/*FVector GunLocationStart = Gun -> GetActorLocation();
	FVector GunLocationFire = FVector(GunLocationStart.X - 5, GunLocationStart.Y, GunLocationStart.Z);

	Gun -> AddActorLocalOffset(GunLocationFire);
	Gun -> AddActorLocalOffset(GunLocationStart);*/

	switch(Gun->GunType)
	{
		case EGunType::RIFLE:
		Gun->Attack();
		break;

		case EGunType::SHOTGUN:
		Gun->Shotgun();
		break;

		case EGunType::PROJECTILELAUNCHER:
		Gun->LaunchProjectile();
		break;

		case EGunType::LASER:
		Gun->Laser();
		break;
	}
}

void ABaseCharacter::SpawnGuns()
{
	for(int i = 0; i < GunClasses.Num();i++)
	{
		Guns.EmplaceAt(i, GetWorld()->SpawnActor<AGun>(GunClasses[i]));

		if(Guns[i] != nullptr)
		{
			Guns[i]->AttachToComponent(WeaponSpawn, FAttachmentTransformRules::KeepRelativeTransform);
			Guns[i]->SetOwner(this);
			Guns[i] -> SetActorHiddenInGame(true);
		}
	}
	EquipGun();
	Gun -> SetActorHiddenInGame(false);
}

void ABaseCharacter::FireButtonPressed()
{
    bFireButtonPressed = true;
}

void ABaseCharacter::FireButtonReleased()
{
    bFireButtonPressed = false;
}

void ABaseCharacter::StartFireTimer()
{
	if(bShouldFire)
	{
		Attack();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &ABaseCharacter::AutoFireReset, Gun->FireRate);
	}
}

void ABaseCharacter::AutoFireReset()
{
	bShouldFire = true;
	if(bFireButtonPressed)
	{
		StartFireTimer();
	}
}

void ABaseCharacter::EquipGun()
{
	Gun = Guns[ActiveIndex];
}

void ABaseCharacter::IncreaseActiveIndex()
{
	Gun -> SetActorHiddenInGame(true);
	if(ActiveIndex < (Guns.Num()-1))
	{
		ActiveIndex++;
	}
	else
	{
		ActiveIndex = 0;
	}
	EquipGun();
	Gun -> SetActorHiddenInGame(false);
}

void ABaseCharacter::DecreaseActiveIndex()
{
	Gun -> SetActorHiddenInGame(true);
	if(ActiveIndex > 0)
	{
		ActiveIndex--;
	}
	else
	{
		ActiveIndex = (Guns.Num()-1);
	}
	EquipGun();
	Gun -> SetActorHiddenInGame(false);
}