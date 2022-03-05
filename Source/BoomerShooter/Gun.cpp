// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "BulletHitInterface.h"
#include "Enemy.h"
#include "Projectile.h"
#include "BaseCharacter.h"
#include "Math/UnrealMathUtility.h"



AGun::AGun()
{
    BulletSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawn"));
    BulletSpawn -> SetupAttachment(WeaponMesh);

    BulletSpawn2 = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawn2"));
    BulletSpawn2 -> SetupAttachment(WeaponMesh);

    PlayerCharacter = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

bool AGun::GetBeamEndLocation(const FVector& BulletSpawnLocation, FHitResult& OutHitResult)
{
    float RandY = FMath::FRandRange(-GunSpread, GunSpread);
    float RandZ = FMath::FRandRange(-GunSpread, GunSpread);

    FVector2D ViewPortSize;
    if(GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewPortSize);
    }

    //Get screen space location of crosshairs
    FVector2D CrossHairLocation(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    //Get world position & direction of crosshairs
    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
        UGameplayStatics::GetPlayerController(this, 0), 
        CrossHairLocation, 
        CrosshairWorldPosition,
        CrosshairWorldDirection);

    if(bScreenToWorld)
    {
        //CrosshairWorldDirection.Rotation().Yaw += RandZ;
        FRotator CrosshairWorldDirectionRotation = CrosshairWorldDirection.Rotation();
        CrosshairWorldDirectionRotation.Yaw += RandZ;
        CrosshairWorldDirectionRotation.Pitch += RandY;
        
        FHitResult ScreenTraceHit; 
        FVector Start{CrosshairWorldPosition};
        FVector End {CrosshairWorldPosition + CrosshairWorldDirectionRotation.Vector() * Range};
        FVector OutBeamLocation;
        //End.X = End.X + RandX;
        //End.Z = End.Z + RandZ; 

        //Set beam end point to line trace end point
        OutBeamLocation = End;

        //Trace outward from crosshairs world location
        GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);
        if(ScreenTraceHit.bBlockingHit) // was there a trace hit?
        {
            OutBeamLocation = ScreenTraceHit.Location;
            
            //perform trace from barrel
            const FVector WeaponTraceStart{BulletSpawnLocation};
            const FVector WeaponTraceEnd{OutBeamLocation};

            GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

            if (!OutHitResult.bBlockingHit)
            {
                OutHitResult.Location = OutBeamLocation;
                return false;
            }
            return true;
        }
    }
    return false;
}

void AGun::Attack()
{
    Super::Attack();

    if(MuzzleSound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, BulletSpawn->GetComponentLocation());
        if(MuzzleFlash != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BulletSpawn->GetComponentLocation(), BulletSpawn->GetComponentRotation());
        }
    }

    FHitResult BeamHitResult;
    bool bBeamEnd = GetBeamEndLocation(BulletSpawn->GetComponentLocation(), BeamHitResult);
    if(bBeamEnd)
    {
        //Does hit actor implement BullHitInterface
        if(BeamHitResult.Actor.IsValid())
        {
            IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());
            if(BulletHitInterface)
            {
                BulletHitInterface -> BulletHit_Implementation(BeamHitResult, PlayerCharacter, this->GetOwnerController());
            }

            AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor.Get());
            if(HitEnemy)
            {
                if(BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
                {
                    UGameplayStatics::ApplyDamage(
                    BeamHitResult.Actor.Get(), 
                    CritDamage, 
                    this->GetOwnerController(), 
                    this, 
                    UDamageType::StaticClass());
                }
                else
                {
                    UGameplayStatics::ApplyDamage(
                    BeamHitResult.Actor.Get(), 
                    Damage, 
                    this->GetOwnerController(), 
                    this, 
                    UDamageType::StaticClass());
                }
            }
        }
        else
        {
            //Spawn default particles
            if(HitParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, BeamHitResult.Location);
            }
        }
    }

    if(BeamParticles)
    {
        UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, BulletSpawn->GetComponentLocation());
        if(Beam)
        {
            Beam->SetVectorParameter(FName("Source"), BulletSpawn->GetComponentLocation());
            Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
        }
    }
    
}

void AGun::Rifle(USceneComponent* BulletStart)
{
    Super::Attack();

    if(MuzzleSound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, BulletStart->GetComponentLocation());
        if(MuzzleFlash != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BulletStart->GetComponentLocation(), BulletStart->GetComponentRotation());
        }
    }

    FHitResult BeamHitResult;
    bool bBeamEnd = GetBeamEndLocation(BulletStart->GetComponentLocation(), BeamHitResult);
    if(bBeamEnd)
    {
        //Does hit actor implement BullHitInterface
        if(BeamHitResult.Actor.IsValid())
        {
            IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());
            if(BulletHitInterface)
            {
                BulletHitInterface -> BulletHit_Implementation(BeamHitResult, PlayerCharacter, this->GetOwnerController());
            }

            AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor.Get());
            if(HitEnemy)
            {
                if(BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
                {
                    UGameplayStatics::ApplyDamage(
                    BeamHitResult.Actor.Get(), 
                    CritDamage, 
                    this->GetOwnerController(), 
                    this, 
                    UDamageType::StaticClass());
                }
                else
                {
                    UGameplayStatics::ApplyDamage(
                    BeamHitResult.Actor.Get(), 
                    Damage, 
                    this->GetOwnerController(), 
                    this, 
                    UDamageType::StaticClass());
                }
            }
        }
        else
        {
            //Spawn default particles
            if(HitParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, BeamHitResult.Location);
            }
        }

        if(BeamParticles)
        {
            UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, BulletSpawn->GetComponentLocation());
            if(Beam)
            {
                Beam->SetVectorParameter(FName("Source"), BulletStart->GetComponentLocation());
                Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
            }
        }
    }
}

void AGun::Shotgun(USceneComponent* BulletStart)
{
    if(MuzzleSound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, BulletStart->GetComponentLocation());
        if(MuzzleFlash != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BulletStart->GetComponentLocation(), BulletStart->GetComponentRotation());
        }
    }

    //float InnerPellets = FMath::RoundHalfFromZero(BulletCount/2);

    for(int i = 0; i < BulletCount; i++)
    {
        FHitResult BeamHitResult;
        bool bBeamEnd = GetBeamEndLocation(BulletStart->GetComponentLocation(), BeamHitResult);
        if(bBeamEnd)
        {
            //Does hit actor implement BullHitInterface
            if(BeamHitResult.Actor.IsValid())
            {
                IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());
                if(BulletHitInterface)
                {
                    BulletHitInterface -> BulletHit_Implementation(BeamHitResult, PlayerCharacter, this->GetOwnerController());
                }
                
                AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor.Get());
                if(HitEnemy)
                {
                    if(BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
                    {
                        UGameplayStatics::ApplyDamage(
                        BeamHitResult.Actor.Get(), 
                        CritDamage, 
                        this->GetOwnerController(), 
                        this, 
                        UDamageType::StaticClass());
                    }
                    else
                    {
                        UGameplayStatics::ApplyDamage(
                        BeamHitResult.Actor.Get(), 
                        Damage, 
                        this->GetOwnerController(), 
                        this, 
                        UDamageType::StaticClass());
                    }
                }
            }
            else
            {
                //Spawn default particles
                if(HitParticles)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(
                        GetWorld(), 
                        HitParticles, 
                        BeamHitResult.Location);
                }
            }

            if(BeamParticles)
            {
                UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
                    GetWorld(), 
                    BeamParticles, 
                    BulletStart->GetComponentLocation());
                if(Beam)
                {
                    Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
                }
            }
        }
    }
}

void AGun::LaunchProjectile(USceneComponent* BulletStart)
{
    FVector ProjectileSpawnLocation = BulletStart->GetComponentLocation();
	FRotator ProjectileSpawnRotation = BulletStart->GetComponentRotation();

    if(MuzzleSound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, BulletStart->GetComponentLocation());
        if(MuzzleFlash != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BulletStart->GetComponentLocation(), BulletStart->GetComponentRotation());
        }
    }

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnLocation, ProjectileSpawnRotation);
	Projectile->SetOwner(this->GetOwner());
}