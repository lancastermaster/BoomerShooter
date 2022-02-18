// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KhaimeraAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BOOMERSHOOTER_API UKhaimeraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	public:
		UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	protected:

	private:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement",meta = (AllowPrivateAccess = true))
		float Speed;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		class AEnemy* Enemy;
};
