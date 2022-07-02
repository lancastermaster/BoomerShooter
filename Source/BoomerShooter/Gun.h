// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ManaType.h"
#include "Gun.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
		enum class EGunType : uint8 {
			RIFLE = 0 UMETA(DisplayName = "RIFLE"),
			SHOTGUN = 1 UMETA(DisplayName = "SHOTGUN"),
			PROJECTILELAUNCHER = 2 UMETA(DisplayName = "PROJECTILELAUNCHER"),
			SUPERSHOTGUN = 3 UMETA(DisplayName = "SUPERSHOTGUN")
		};

UENUM(BlueprintType)
		enum class EModType : uint8 {
			AIM = 0 UMETA(DisplayName = "AIM"),
			QUICKFIRE = 1 UMETA(DisplayName = "QUICKFIRE"),
			OVERCHARGE = 2 UMETA(DisplayName = "OVERCHARGE")
		};

UCLASS()
class BOOMERSHOOTER_API AGun : public AWeapon
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		AGun();

		UFUNCTION(BlueprintCallable, Category = "Combat")
		virtual void Attack() override;

		void Rifle(USceneComponent* BulletStart);

		void Shotgun(USceneComponent* BulletStart);

		void LaunchProjectile(USceneComponent* BulletStart);

		UPROPERTY(EditAnywhere, Category = "Combat")
		float FireRate;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float ModFireRate;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		EGunType GunType;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		EModType GunMod;

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
		class USceneComponent* BulletSpawn2;

		UPROPERTY(EditAnywhere, Category = "Combat")
		int BulletCount;

		UPROPERTY(EditAnywhere, Category = "Combat")
		TSubclassOf<class AProjectile> ProjectileClass;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float ModManaCost = 0.f;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float BaseManaCost = 0.f;
		
		class ABaseCharacter* PlayerCharacter;
		
	private:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float ZoomFactor;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		EManaType ManaType;

	public:
		FORCEINLINE float GetModManaCost() const {return ModManaCost;}
		FORCEINLINE float GetBaseManaCost() const {return BaseManaCost;}
		FORCEINLINE USceneComponent* GetBulletSpawn() const {return BulletSpawn;}
		FORCEINLINE USceneComponent* GetBulletSpawn2() const {return BulletSpawn2;}
		FORCEINLINE EManaType GetManaType() const {return ManaType;}
};
