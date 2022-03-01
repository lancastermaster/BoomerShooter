// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/SceneComponent.h"
#include "Gun.h"
#include "Camera/CameraComponent.h"
#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"
#include "BoomerShooterPlayerController.h"
#include "BoomerShooterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera -> SetupAttachment(RootComponent);

	WeaponSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Spawn Point"));
	WeaponSpawn -> SetupAttachment(Camera);

	bAiming = false;
	bShouldFire = true;
	bShouldSecondaryFire = true;
	bFireButtonPressed = false;
	bWeaponSecondaryPressed = false;
	CameraDefaultFOV = 0.f; //set in BeginPlay
	CameraZoomedFOV = 60.f;
	ZoomInterpSpeed = 20.f;
	CameraCurrentFOV = 0.f;
	ActiveIndex = 1;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(Camera)
	{
		CameraDefaultFOV = GetPlayerCamera() -> FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	Health = MaxHealth;
	Mana = MaxMana;
	Stress = 0.f;
	
	SpawnGuns();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//setting CameraFOV
	if(bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetPlayerCamera()->SetFieldOfView(CameraCurrentFOV);

	if(bFireButtonPressed) StartFireTimer();

	//if(bWeaponSecondaryPressed) StartSecondaryFireTimer();
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
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Pressed, this, &ABaseCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Released, this, &ABaseCharacter::AimButtonReleased);
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
	if(Mana > 0.f && Mana > Gun->GetBaseManaCost())
	{
		SpendMana(Gun->GetBaseManaCost());
		switch(Gun->GunType)
		{
			case EGunType::RIFLE:
			Gun->Rifle(Gun->GetBulletSpawn());
			break;

			case EGunType::SHOTGUN:
			Gun->Shotgun(Gun->GetBulletSpawn());
			break;

			case EGunType::PROJECTILELAUNCHER:
			Gun->LaunchProjectile(Gun->GetBulletSpawn());
			break;

			case EGunType::SUPERSHOTGUN:
			Gun->Shotgun(Gun->GetBulletSpawn());
			Gun->Shotgun(Gun->GetBulletSpawn2());
			break;
		}
	}
}

void ABaseCharacter::SpawnGuns()
{
	for(int i = 0; i < GunClasses.Num();i++)
	{
		SpawnGun(i);
	}
	EquipGun(ActiveIndex);
	Gun -> SetActorHiddenInGame(false);
}

void ABaseCharacter::SpawnGun(int i)
{
	Guns.EmplaceAt(i, GetWorld()->SpawnActor<AGun>(GunClasses[i]));

	if(Guns[i] != nullptr)
	{
		Guns[i]->AttachToComponent(WeaponSpawn, FAttachmentTransformRules::KeepRelativeTransform);
		Guns[i]->SetOwner(this);
		Guns[i] -> SetActorHiddenInGame(true);
	}
}

void ABaseCharacter::FireButtonPressed()
{
    bFireButtonPressed = true;
}

void ABaseCharacter::FireButtonReleased()
{
    bFireButtonPressed = false;
}

void ABaseCharacter::AimButtonPressed()
{
	bAiming = true; 
}

void ABaseCharacter::AimButtonReleased()
{
	bAiming = false;
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

void ABaseCharacter::EquipGun(int i)
{
	Gun = Guns[i];
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

	if(Guns[ActiveIndex])
	{
		EquipGun(ActiveIndex);
		Gun -> SetActorHiddenInGame(false);
	}
	else IncreaseActiveIndex();
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

	if(Guns[ActiveIndex])
	{
		EquipGun(ActiveIndex);
		Gun -> SetActorHiddenInGame(false);
	}
	else DecreaseActiveIndex();
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& Damageevent, class AController* EventInstigator, AActor* DamageCauser)
{
	if(Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();

		auto EnemyController = Cast<ABoomerShooterAIController>(EventInstigator);
		if(EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(
				FName(TEXT("PlayerDead")), 
				true);
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void ABaseCharacter::SpendMana(float ManaCost)
{
	if(Mana <= 0.f)return;
	if(Mana < ManaCost)return;
	Mana -= ManaCost;
}

void ABaseCharacter::Die()
{
	ABoomerShooterPlayerController* PC = Cast<ABoomerShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(PC)
	{
		DisableInput(PC);
		PC->bShowMouseCursor = true;
	}
}