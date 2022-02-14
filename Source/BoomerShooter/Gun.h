// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
		enum class EGunType : uint8 {
			RIFLE = 0 UMETA(DisplayName = "RIFLE"),
			SHOTGUN = 1 UMETA(DisplayName = "SHOTGUN"),
			PROJECTILELAUNCHER = 2 UMETA(DisplayName = "PROJECTILELAUNCHER")
		};

UCLASS()
class BOOMERSHOOTER_API AGun : public AWeapon
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		AGun();

		virtual void Attack() override;

		void Shotgun();

		UPROPERTY(EditAnywhere, Category = "Combat")
		float FireRate;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		EGunType GunType;

	protected:
		// Called when the game starts or when spawned
		//virtual void BeginPlay() override;

		bool GetBeamEndLocation(const FVector& BulletSpawnLocation, FHitResult& OutHitResult);

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
		float GunSpread;

		UPROPERTY(EditAnywhere, Category = "Combat")
		class USceneComponent* BulletSpawn;

		UPROPERTY(EditAnywhere, Category = "Combat")
		int BulletCount;
		
	private:
};
