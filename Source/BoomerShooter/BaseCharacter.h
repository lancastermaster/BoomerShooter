// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	bool bFireButtonPressed; //left mouse or right trigger

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Attack();
	void SecondaryAction();

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	void IncreaseActiveIndex();
	void DecreaseActiveIndex();
	void EquipGun();

	//void SpawnGun();
	void SpawnGuns();

	void FireButtonPressed();
	void FireButtonReleased();

	void WeaponSecondaryPressed();
	void WeaponSecondaryReleased();

	void StartFireTimer();
	void StartSecondaryFireTimer();

	UFUNCTION()
	void AutoFireReset();

	UFUNCTION()
	void SecondaryFireReset();

	void Die();

	UPROPERTY(EditAnywhere)
	float RotationRate = 10;

	UPROPERTY(EditAnywhere)
	USceneComponent* WeaponSpawn;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AGun>> GunClasses;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

private:

	FTimerHandle AutoFireTimer;

	FTimerHandle SecondaryFireTimer;

	bool bShouldFire; //true when firing possible, false when waiting for timer

	bool bShouldSecondaryFire;

	bool bWeaponSecondaryPressed;

	int ActiveIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana", meta = (AllowPrivateAccess = true))
	float MaxMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mana", meta = (AllowPrivateAccess = true))
	float Mana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stress", meta = (AllowPrivateAccess = true))
	float MaxStress;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stress", meta = (AllowPrivateAccess = true))
	float Stress;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	class USoundCue* MeleeImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	class UParticleSystem* BloodParticles;

public:
	FORCEINLINE USoundCue* GetMeleeImpactSound()const{return MeleeImpactSound;}
	FORCEINLINE UParticleSystem* GetBloodParticles() const {return BloodParticles;}
	FORCEINLINE bool GetWeaponSecondaryPressed() const {return bWeaponSecondaryPressed;}
};
