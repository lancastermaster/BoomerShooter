// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"


AGun::AGun()
{
    BulletSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawn"));
    BulletSpawn -> SetupAttachment(WeaponMesh);
}

bool AGun::GetBeamEndLocation(const FVector& BulletSpawnLocation, FVector& OutBeamLocation)
{
    float RandX = FMath::FRandRange(0, GunSpread);
    float RandZ = FMath::FRandRange(0, GunSpread);

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
        FHitResult ScreenTraceHit; 
        const FVector Start{CrosshairWorldPosition};
        FVector End {CrosshairWorldPosition + CrosshairWorldDirection * Range};
        End.X = End.X + RandX;
        End.Z = End.Z + RandZ; 

        //Set beam end point to line trace end point
        OutBeamLocation = End;

        //Trace outward from crosshairs world location
        GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);
        if(ScreenTraceHit.bBlockingHit) // was there a trace hit?
        {
            OutBeamLocation = ScreenTraceHit.Location;
            

            //perfomr trace from barrel
            FHitResult WeaponTraceHit;
            const FVector WeaponTraceStart{BulletSpawnLocation};
            const FVector WeaponTraceEnd{OutBeamLocation};

            GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

            if (WeaponTraceHit.bBlockingHit)
            {
                OutBeamLocation = WeaponTraceHit.Location;
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

    FVector BeamEnd;
    bool bBeamEnd = GetBeamEndLocation(BulletSpawn->GetComponentLocation(), BeamEnd);
    if(bBeamEnd)
    {
        if(HitParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, BeamEnd);
        }

        if(BeamParticles)
        {
            UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, BulletSpawn->GetComponentLocation());
            if(Beam)
            {
                Beam->SetVectorParameter(FName("Target"), BeamEnd);
            }
        }
    }
}

void AGun::Shotgun()
{
    if(MuzzleSound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, BulletSpawn->GetComponentLocation());
        if(MuzzleFlash != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BulletSpawn->GetComponentLocation(), BulletSpawn->GetComponentRotation());
        }
    }

    for(int i = 0; i < BulletCount; i++)
    {
        FVector BeamEnd;
        bool bBeamEnd = GetBeamEndLocation(BulletSpawn->GetComponentLocation(), BeamEnd);
        if(bBeamEnd)
        {
            if(HitParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, BeamEnd);
            }

            if(BeamParticles)
            {
                UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, BulletSpawn->GetComponentLocation());
                if(Beam)
                {
                    Beam->SetVectorParameter(FName("Target"), BeamEnd);
                }
            }
        }
    }

}