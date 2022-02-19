// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/EngineTypes.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BoomerShooterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "BaseCharacter.h"

// Sets default values
AEnemy::AEnemy()
{
	bCanHitReact = true;
	bStunned = false;
	HitReactTimeMin = .5f;
	HitReactTimeMax = 3.f;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Agro Sphere"));
	AgroSphere -> SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Combat Range Sphere"));
	CombatRangeSphere -> SetupAttachment(GetRootComponent());

	AttackAFast = (TEXT("AttackA_Fast"));
	AttackBFast = (TEXT("AttackB_Fast"));
	AttackA = (TEXT("AttackA"));
	AttackB = (TEXT("AttackB"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(
		this, 
		&AEnemy::AgroSphereOverlap);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&AEnemy::CombatRangeOverlap);

	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(
		this,
		&AEnemy::CombatRangeEndOverlap);

	Health = MaxHealth;
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	EnemyController = Cast<ABoomerShooterAIController>(GetController());

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);

	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint,
		25.f,
		12,
		FColor::Red,
		true);

		DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint2,
		25.f,
		12,
		FColor::Red,
		true);

	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);

		EnemyController->RunBehaviorTree(BehaviorTree);
	}

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), 
			ImpactParticles, 
			HitResult.Location, 
			FRotator(0.f), 
			true);
	}
	if(CanSeeHealthBar)
	{
		ShowHealthBar();
	}

	const float Stunned = FMath::RandRange(0.f,1.f);
	if(Stunned <= StunChance)
	{
		//stun enemy
		PlayHitMontage(FName("HitReactFront"));
		SetStunned(true);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}
	return 0.f;
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(
		HealthBarTimer,
		this, 
		&AEnemy::HideHealthBar, 
		HealthBarDisplayTime);
}

void AEnemy::Die()
{
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance)
	{
		AnimInstance->Montage_Play(HitMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("DeathA"), HitMontage);
	}
	HideHealthBar();
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if(bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if(AnimInstance)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}

		bCanHitReact = false;
		const float HitReactTime = FMath::FRandRange(HitReactTimeMin, HitReactTimeMax);
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, HitReactTime);
	}
}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	if(bInAttackRange)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if(AnimInstance)
		{
			AnimInstance->Montage_Play(AttackMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, AttackMontage);
		}
	}
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{FMath::RandRange(1,4)};

	switch(Section)
	{
		case 1:
			SectionName = AttackAFast;
			break;

		case 2:
			SectionName = AttackBFast;
			break;

		case 3:
			SectionName = AttackA;
			break;

		case 4:
			SectionName = AttackB;
			break;
	}

	return SectionName;
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr)return;

	auto Character = Cast<ABaseCharacter>(OtherActor);
	if(Character)
	{
		//set value of target blackboard key
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"), 
			Character);
	}

}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr)return;

	auto ShooterCharacter = Cast<ABaseCharacter>(OtherActor);
	if(ShooterCharacter)
	{
		bInAttackRange = true;

		if(EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(
				TEXT("InAttackRange"), 
				bInAttackRange);
		}
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor == nullptr)return;

	auto ShooterCharacter = Cast<ABaseCharacter>(OtherActor);
	if(ShooterCharacter)
	{
		bInAttackRange = false;

		if(EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(
				TEXT("InAttackRange"), 
				bInAttackRange);
		}
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(
			TEXT("Stunned"), 
			Stunned);
	}
}