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

	UPROPERTY(BlueprintReadWrite)
	class AGun* Gun;

	UPROPERTY(BlueprintReadWrite)
	TArray<AGun*> Guns;

	bool bFireButtonPressed; //left mouse or right trigger

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Attack();

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

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

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

	bool bShouldFire; //true when firing possible, false when waiting for timer

	int ActiveIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana", meta = (AllowPrivateAccess = true))
	float MaxMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mana", meta = (AllowPrivateAccess = true))
	float Mana;

};
