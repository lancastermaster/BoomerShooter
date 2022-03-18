// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LockKey.generated.h"

UCLASS()
class BOOMERSHOOTER_API ALockKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALockKey();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class USceneComponent* KeyRoot;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* KeyMesh;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxCollider;

public:	

};
