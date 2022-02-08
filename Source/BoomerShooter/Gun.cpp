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

void AGun::Attack()
{
    Super::Attack();
    //UE_LOG(LogTemp, Warning, TEXT("BANG!!!"));

    //float RandX = FMath::FRandRange(0, 200);
    //float RandZ = FMath::FRandRange(0, 200);

    if(MuzzleSound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, BulletSpawn->GetComponentLocation());
        if(MuzzleFlash != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BulletSpawn->GetComponentLocation(), BulletSpawn->GetComponentRotation());
        }
    }

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

    //was deprojection successful?
    if(bScreenToWorld)
    {
        FHitResult ScreenTraceHit; 
        const FVector Start{CrosshairWorldPosition};
        FVector End {CrosshairWorldPosition + CrosshairWorldDirection * Range};
        //End.X = End.X + RandX;
        //End.Z = End.Z + RandZ; 

        //Set beam end point to line trace end point
        FVector BeamEndPoint{End};

        //Trace outward from crosshairs world location
        GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);
        if(ScreenTraceHit.bBlockingHit) // was there a trace hit?
        {
            BeamEndPoint = ScreenTraceHit.Location;
            if(HitParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ScreenTraceHit.Location);
            }
            if(BeamParticles)
            {
                UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, BulletSpawn->GetComponentLocation());
                if(Beam)
                {
                    Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
                }
            }
        }
    }

    /*FHitResult FireHit;
    const FVector Start{BulletSpawn->GetComponentLocation()};
    const FQuat Rotation {BulletSpawn->GetComponentRotation()};
    const FVector RotationAxis {Rotation.GetAxisX()};
    FVector End {Start + RotationAxis * Range};
    End.X = End.X + RandX;
    End.Z = End.Z + RandZ;

    FVector BeamEndPoint{ End };

    GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
    if(FireHit.bBlockingHit)
    {
        //DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
        //DrawDebugPoint(GetWorld(), FireHit.Location, 5.f, FColor::Red, false, 2.f);
        BeamEndPoint = FireHit.Location;
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, FireHit.Location);
    }
    if(BeamParticles)
    {
        UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, Start);
        if(Beam)
        {
            Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
        }
    }*/
}