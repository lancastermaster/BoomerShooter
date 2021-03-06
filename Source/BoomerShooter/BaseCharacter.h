// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ManaType.h"
#include "BaseCharacter.generated.h"

UCLASS()
class BOOMERSHOOTER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& Damageevent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	UPROPERTY(BlueprintReadWrite)
	class AGun* Gun;

	UPROPERTY(BlueprintReadWrite)
	TArray<AGun*> Guns;

	UPROPERTY(BlueprintReadWrite)
	bool bFireButtonPressed; //left mouse or right trigger

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Attack();

	UFUNCTION(BlueprintImplementableEvent)
	void Interact();

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	void IncreaseActiveIndex();
	void DecreaseActiveIndex();
	void EquipGun(int i);

	UFUNCTION(BlueprintCallable)
	void SpawnGun(int i);

	void SpawnGuns();

	void FireButtonPressed();
	void FireButtonReleased();

	void AimButtonPressed();
	void AimButtonReleased();
	void Aim(float DeltaTime);

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	void Die();

	UFUNCTION(BlueprintImplementableEvent)
	void Dodge();

	void InitializeManaMap();
	bool HasMana();

	UPROPERTY(EditAnywhere)
	float RotationRate = 10;

	UPROPERTY(EditAnywhere)
	USceneComponent* WeaponSpawn;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<AGun>> GunClasses;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Exploration", meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<AActor>> KeyClasses;
private:

	float CameraDefaultFOV;
	float CameraZoomedFOV;
	float CameraCurrentFOV;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	float ZoomInterpSpeed;

	FTimerHandle AutoFireTimer;

	FTimerHandle SecondaryFireTimer;

	bool bShouldFire; //true when firing possible, false when waiting for timer

	bool bShouldSecondaryFire;

	bool bWeaponSecondaryPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool bAiming;

	int ActiveIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	TMap<EManaType, int32> ManaMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 MaxStress;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 Stress;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 MaxMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 Mana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 MaxFireMana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 MaxIceMana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = true))
	int32 MaxLightningMana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	class USoundCue* MeleeImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	class UParticleSystem* BloodParticles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	class USoundCue* PlayerHurtCue;

public:
	FORCEINLINE USoundCue* GetMeleeImpactSound()const{return MeleeImpactSound;}
	FORCEINLINE UParticleSystem* GetBloodParticles() const {return BloodParticles;}
	FORCEINLINE bool GetWeaponSecondaryPressed() const {return bWeaponSecondaryPressed;}
	FORCEINLINE UCameraComponent* GetPlayerCamera() const {return Camera;}
};
