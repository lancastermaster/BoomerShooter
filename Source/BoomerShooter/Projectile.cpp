// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/PlayerController.h"
#include "Enemy.h"
#include "Components/SphereComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	RootComponent = SphereComp;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	MovementComp->InitialSpeed = 600.f;
	MovementComp->MaxSpeed = 600.f;

	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particles"));
	TrailParticles->SetupAttachment(RootComponent);

	CritDamage = Damage * 3.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();

	if(MyOwner == nullptr)
	{
		Destroy();
		return;
	}

	AController* MyOwnerInstigator = MyOwner->GetInstigatorController();
	UClass* DamageTypeClass = UDamageType::StaticClass();

	if(OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		if(Hit.Actor.IsValid())
		{
			IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(Hit.Actor.Get());
            if(BulletHitInterface)
            {
                BulletHitInterface -> BulletHit_Implementation(Hit);
            }

            AEnemy* HitEnemy = Cast<AEnemy>(Hit.Actor.Get());
            if(HitEnemy)
            {
                UGameplayStatics::ApplyDamage(
                HitEnemy, 
                Damage, 
                this->GetOwnerController(), 
                this, 
                UDamageType::StaticClass());
            }
		}

		//UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		//UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, MyOwner, DamageTypeClass);
		/*if(ProjectileParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				this,
				ProjectileParticles,
				GetActorLocation(),
				GetActorRotation(),
				true
			);
		}*/
	}
	Destroy();
}

AController* AProjectile::GetOwnerController() const
{
    APawn *OwnerPawn = Cast<APawn>(GetOwner());

    if(OwnerPawn == nullptr)
    {
        return nullptr;
    }
    return OwnerPawn -> GetController();
}