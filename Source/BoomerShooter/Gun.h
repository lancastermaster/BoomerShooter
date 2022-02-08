// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

/**
 * 
 */
UCLASS()
class BOOMERSHOOTER_API AGun : public AWeapon
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		AGun();

		virtual void Attack() override;

	protected:
		// Called when the game starts or when spawned
		//virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, Category = "Combat")
		class UParticleSystem* MuzzleFlash;
		
		UPROPERTY(EditAnywhere, Category = "Combat")
		class UParticleSystem* BeamParticles;

		UPROPERTY(EditAnywhere, Category = "Combat")
		class UParticleSystem* HitParticles;

		UPROPERTY(EditAnywhere, Category = "Combat")
		class USoundBase* MuzzleSound;

		UPROPERTY(EditAnywhere, Category = "Combat")
		class USoundBase* HitSound;
		
		UPROPERTY(EditAnywhere, Category = "Combat")
		float Range;

		UPROPERTY(EditAnywhere, Category = "Combat")
		class USceneComponent* BulletSpawn;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float FireRate;
		
		FTimerHandle FireRateHandle;
	
	private:
};
