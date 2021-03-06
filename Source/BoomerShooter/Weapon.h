// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
	enum class EWeaponDamageType : uint8 {
		Ice = 0 UMETA(DisplayName = "Ice"),
		Lightning = 1 UMETA(DisplayName = "Lightning"),
		Fire = 2 UMETA(DisplayName = "Fire")
	};

UCLASS()
class BOOMERSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	float CritDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UTexture2D* Crosshair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EWeaponDamageType WeaponDamageType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AController* GetOwnerController() const;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	float CritDamage;*/

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* WeaponMesh;

private:	

};
