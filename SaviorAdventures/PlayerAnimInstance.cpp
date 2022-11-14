// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainPlayer.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
	}
}

void UPlayerAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
	}

	if (Pawn) {
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		IsHanging = Cast<AMainPlayer>(Pawn)->Hanging;

		if (IsHanging) {
			SwingSpeed = Pawn->GetVelocity().Dot(Pawn->GetActorForwardVector());
		}
	}
}

void UPlayerAnimInstance::CanGrab(bool PlayerCanGrab)
{
	bIsHangingFromLedge = PlayerCanGrab;
}

void UPlayerAnimInstance::ClimbLedge(bool PlayerCanClimb)
{
	bIsClimbing = PlayerCanClimb;
	Montage_Play(ClimbMontage);
}

void UPlayerAnimInstance::StopClimbing()
{
	Montage_Stop(0.f);

	Cast<AMainPlayer>(Pawn)->PlayClimbLedge(false);

	bIsClimbing = false;
}
