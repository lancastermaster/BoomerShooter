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

	class AGun* Gun;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Attack();

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);

	void LookUp(float AxisValue);

	void LookRight(float AxisValue);

	void SpawnGun();

	UPROPERTY(EditAnywhere)
	float RotationRate = 10;

	UPROPERTY(EditAnywhere)
	USceneComponent* WeaponSpawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;
private:	

};
