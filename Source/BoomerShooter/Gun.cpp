// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"


AGun::AGun()
{
    BulletSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawn"));
    BulletSpawn -> SetupAttachment(WeaponMesh);
}

void AGun::Attack()
{
    Super::Attack();
    //UE_LOG(LogTemp, Warning, TEXT("BANG!!!"));

    if(MuzzleSound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MuzzleSound, BulletSpawn->GetComponentLocation());
        if(MuzzleFlash != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BulletSpawn->GetComponentLocation());
        }
    }
}