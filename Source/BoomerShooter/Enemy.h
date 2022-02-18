// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class BOOMERSHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

	public:
		// Sets default values for this character's properties
		AEnemy();

		// Called every frame
		virtual void Tick(float DeltaTime) override;

		// Called to bind functionality to input
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		virtual void BulletHit_Implementation(FHitResult HitResult) override;

		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

		UFUNCTION(BlueprintPure)
		float GetHealth() const {return Health;}

		UFUNCTION(BlueprintPure)
		float GetMaxHealth() const {return MaxHealth;}
		

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		UFUNCTION(BlueprintNativeEvent)
		void ShowHealthBar();
		void ShowHealthBar_Implementation();

		UFUNCTION(BlueprintImplementableEvent)
		void HideHealthBar();

		void Die();

		void PlayHitMontage(FName Section, float PlayRate = 1.0f);

		UFUNCTION(BlueprintCallable)
		void PlayAttackMontage(FName Section, float PlayRate = 1.0f);

		UFUNCTION(BlueprintPure)
		FName GetAttackSectionName();

		void ResetHitReactTimer();

		//called when something overlaps the agrosphere
		UFUNCTION()
		void AgroSphereOverlap(
			UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, 
			bool bSweep, 
			const FHitResult& SweepResult);

		UFUNCTION(BlueprintCallable)
		void SetStunned(bool Stunned);

		UFUNCTION()
		void CombatRangeOverlap(
			UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, 
			bool bSweep, 
			const FHitResult& SweepResult);

		UFUNCTION()
		void CombatRangeEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

		//spawned or played when hit
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class UParticleSystem* ImpactParticles;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class USoundCue* ImpactSound;


	private:
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
		float MaxHealth = 100.f;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float Health = 0.f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		FString HeadBone;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float HealthBarDisplayTime;

		FTimerHandle HealthBarTimer;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		bool CanSeeHealthBar;

		
		FTimerHandle HitReactTimer;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float HitReactTimeMin;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float HitReactTimeMax;

		bool bCanHitReact;

		UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = true))
		class UBehaviorTree* BehaviorTree;

		UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = true, MakeEditWidget = true))
		FVector PatrolPoint;

		UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = true, MakeEditWidget = true))
		FVector PatrolPoint2;

		class ABoomerShooterAIController* EnemyController;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class USphereComponent* AgroSphere;

		//true when enemy is hit
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		bool bStunned;

		// 0: no stun chance, 1: 100% stun chance
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		float StunChance = .5f;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		bool bInAttackRange;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		USphereComponent* CombatRangeSphere;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class UAnimMontage* HitMontage;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
		class UAnimMontage* AttackMontage;

		//the four attack montage section names
		FName AttackAFast;
		FName AttackBFast;
		FName AttackA;
		FName AttackB;


	public:
		FORCEINLINE FString GetHeadBone() const {return HeadBone;}

		FORCEINLINE UBehaviorTree* GetBehaviorTree() const {return BehaviorTree;}
};
