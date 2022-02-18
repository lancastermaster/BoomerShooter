// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BoomerShooterAIController.generated.h"

/**
 * 
 */
UCLASS()
class BOOMERSHOOTER_API ABoomerShooterAIController : public AAIController
{
	GENERATED_BODY()

	public:
		ABoomerShooterAIController();

		virtual void OnPossess(APawn* InPawn) override;

		
	protected:


	private:
		UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = true))
		class UBlackboardComponent* BlackboardComp;

		UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = true))
		class UBehaviorTreeComponent* BehaviorTreeComp;

	public:
		FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const {return BlackboardComp;}
	
};
