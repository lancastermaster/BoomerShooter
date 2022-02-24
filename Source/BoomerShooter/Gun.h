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
			NONE = 0 UMETA(DisplayName = "NONE"),
			RIFLE = 1 UMETA(DisplayName = "RIFLE"),
			SHOTGUN = 2 UMETA(DisplayName = "SHOTGUN"),
			PROJECTILELAUNCHER = 3 UMETA(DisplayName = "PROJECTILELAUNCHER")
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

		void LaunchProjectile();

		UPROPERTY(EditAnywhere, Category = "Combat")
		float FireRate;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float ModFireRate;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		EGunType GunType;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		EGunType GunMod;

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

		UPROPERTY(EditAnywhere, Category = "Combat")
		TSubclassOf<class AProjectile> ProjectileClass;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float ModManaCost = 0.f;
		
		class ABaseCharacter* PlayerCharacter;
	private:

	public:
		FORCEINLINE float GetModManaCost() const {return ModManaCost;}
};
