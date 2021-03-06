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
#include "Sound/SoundCue.h"
#include "Engine/EngineTypes.h"

// Sets default values
ABaseCharacter::ABaseCharacter():
	bAiming(false),
	bShouldFire(true),
	bShouldSecondaryFire(true),
	bFireButtonPressed(false),
	bWeaponSecondaryPressed(false),
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(60.f),
	ZoomInterpSpeed(20.f),
	CameraCurrentFOV(0.f),
	ActiveIndex(1)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera -> SetupAttachment(RootComponent);

	WeaponSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Spawn Point"));
	WeaponSpawn -> SetupAttachment(Camera);
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
	Stress = 0;

	InitializeManaMap();
	SpawnGuns();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Aim(DeltaTime);
	if(bFireButtonPressed) StartFireTimer();
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
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &ABaseCharacter::Interact);
	PlayerInputComponent->BindAction(TEXT("Dodge"), EInputEvent::IE_Pressed, this, &ABaseCharacter::Dodge);
}

void ABaseCharacter::InitializeManaMap()
{
	ManaMap.Add(EManaType::EMT_Fire, MaxFireMana);
	ManaMap.Add(EManaType::EMT_Ice, MaxIceMana);
	ManaMap.Add(EManaType::EMT_Lightning, MaxLightningMana);
}

bool ABaseCharacter::HasMana()
{
	if(!Gun)return false;

	auto ManaType = Gun->GetManaType();

	if(ManaMap.Contains(ManaType))
	{
		return ManaMap[ManaType] > 0;
	}
	return false;
}

void ABaseCharacter::Aim(float DeltaTime)
{
	if(bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetPlayerCamera()->SetFieldOfView(CameraCurrentFOV);
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
	if(!Gun) return;
	if(!HasMana()) return;
	auto ManaType = Gun->GetManaType();
	int32 CurrentMana = ManaMap[ManaType];

	if(CurrentMana > 0)
	{
		CurrentMana--;
		ManaMap.Add(ManaType, CurrentMana);
		Gun->Attack();
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
	UGameplayStatics::PlaySoundAtLocation(this, PlayerHurtCue, GetActorLocation());
	return DamageAmount;
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