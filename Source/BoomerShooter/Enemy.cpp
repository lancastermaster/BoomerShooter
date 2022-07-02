// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BoomerShooterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "BaseCharacter.h"

// Sets default values
AEnemy::AEnemy() :
	bCanHitReact (true),
	bStunned(false),
	HitReactTimeMin (.5f),
	HitReactTimeMax (3.f),
	AttackWaitTime (1.f),
	DeathTime (4.f),
	bCanAttack (true),
	bDying (false)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Agro Sphere"));
	AgroSphere -> SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Combat Range Sphere"));
	CombatRangeSphere -> SetupAttachment(GetRootComponent());

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Weapon Box"));
	LeftWeaponCollision -> SetupAttachment(GetMesh(), FName("LeftWeaponBone"));

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision -> SetupAttachment(GetMesh(), FName("RightWeaponBone"));

	AttackAFast = (TEXT("AttackA_Fast"));
	AttackBFast = (TEXT("AttackB_Fast"));
	AttackA = (TEXT("AttackA"));
	AttackB = (TEXT("AttackB"));
	LeftWeaponSocket = (TEXT("FX_Trail_L_01"));
	RightWeaponSocket = (TEXT("FX_Trail_R_01"));

	
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

	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(
		this, 
		&AEnemy::OnLeftWeaponOverlap);

	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(
		this, 
		&AEnemy::OnRightWeaponOverlap);

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Health = MaxHealth;
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	EnemyController = Cast<ABoomerShooterAIController>(GetController());

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);

	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("StartingPosition"), this->GetActorLocation());
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName ("CanAttack"), true);

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

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
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
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(EnemyController)
	{
		if(EventInstigator)
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			FName("Target"),
			EventInstigator->GetPawn());
	}
	if(Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	if(bDying)return DamageAmount;

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

	return DamageAmount;
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
	if(bDying)return;
	bDying = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage, 1.f);
		//AnimInstance->Montage_JumpToSection(FName("DeathA"), HitMontage);
	}

	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
		EnemyController->StopMovement();
	}
	HideHealthBar();
	//Destroy();
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(
		DeathTimer,
		this, 
		&AEnemy::DestroyEnemy, 
		DeathTime);
}

void AEnemy::DestroyEnemy()
{
	Destroy();
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
	//if(bInAttackRange)
	//{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if(AnimInstance && AttackMontage)
		{
			AnimInstance->Montage_Play(AttackMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, AttackMontage);
		}

		bCanAttack = false;
		GetWorldTimerManager().SetTimer(
			AttackWaitTimer,
			this,
			&AEnemy::ResetCanAttack,
			AttackWaitTime
		);

		if(EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName ("CanAttack"), false);
		}
	//}
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{FMath::RandRange(0, AttackMontageSections.Num()-1)};

	SectionName = AttackMontageSections[Section];

	return SectionName;
}

void AEnemy::PlayRangedAttackMontage(float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && RangedAttackMontage)
	{
		AnimInstance->Montage_Play(RangedAttackMontage, PlayRate);
	}

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(
	AttackWaitTimer,
	this,
	&AEnemy::ResetCanAttack,
	AttackWaitTime
	);

	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName ("CanAttack"), false);
	}
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	if(bUseAgroSphere)
	{
		if(OtherActor == nullptr)return;

		auto Character = Cast<ABaseCharacter>(OtherActor);
		if(Character)
		{
			if(EnemyController)
			{
				if(EnemyController->GetBlackboardComponent())
				{
					//set value of target blackboard key
					EnemyController->GetBlackboardComponent()->SetValueAsObject(
					TEXT("Target"), 
					Character);
				}
			}
		}
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

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<ABaseCharacter>(OtherActor);
	if(Character)
	{
		DoDamage(Character);

		SpawnBlood(Character, LeftWeaponSocket);
	}
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<ABaseCharacter>(OtherActor);
	if(Character)
	{
		DoDamage(Character);

		SpawnBlood(Character, RightWeaponSocket);
	}
}

void AEnemy::DoDamage(ABaseCharacter* Victim)
{
	if(Victim == nullptr)return;

	if(Victim)
	{
		UGameplayStatics::ApplyDamage(Victim, BaseDamage, EnemyController, this, UDamageType::StaticClass());
		if(Victim->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, Victim->GetMeleeImpactSound(), GetActorLocation());
		}
	}
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision -> SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision -> SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::SpawnBlood(ABaseCharacter* Victim, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket{GetMesh() -> GetSocketByName(SocketName)};
		if(TipSocket)
		{
			const FTransform SocketTransform{TipSocket->GetSocketTransform(GetMesh())};
			if(Victim->GetBloodParticles())
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					Victim->GetBloodParticles(),
					SocketTransform
				);
			}
		}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName ("CanAttack"), true);
	}
}