// Fill out your copyright notice in the Description page of Project Settings.


#include "LockKey.h"
#include "Components/BoxComponent.h"

// Sets default values
ALockKey::ALockKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KeyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Key Root"));
	RootComponent = KeyRoot;
	
	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Key Mesh"));
	KeyMesh -> SetupAttachment(RootComponent);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider -> SetupAttachment(KeyMesh);
}

// Called when the game starts or when spawned
void ALockKey::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALockKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

