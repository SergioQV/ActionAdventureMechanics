// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SAVIORADVENTURES_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	APawn* Pawn;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool IsHanging;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float SwingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing)
	bool bIsHangingFromLedge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing)
	bool bIsClimbing;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing)
	UAnimMontage* ClimbMontage;

	virtual void NativeInitializeAnimation() override;


	UFUNCTION(BlueprintCallable, Category = Animations)
	void UpdateAnimationProperties();

	void CanGrab(bool PlayerCanGrab);

	void ClimbLedge(bool PlayerCanClimb);

	UFUNCTION(BlueprintCallable, Category = Climbing)
	void StopClimbing();
	
};
