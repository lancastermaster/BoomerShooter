// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BOOMERSHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float CritDamage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UParticleSystemComponent* ProjectileParticles;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UParticleSystemComponent* TrailParticles;
};
