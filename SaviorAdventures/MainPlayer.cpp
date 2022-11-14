// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Rope.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerAnimInstance.h"
#include "Components/ArrowComponent.h"


// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	GetCapsuleComponent()->InitCapsuleSize(48.f, 105.f);

	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; //character moves on the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f; //it allows you to move the character in the air at this rate

	HangingReloadTime = 0.5f;
	
	TracerSphereRadius = 20.f;
	LedgeHeightLocation = FVector(0.f);
	PelvisSocket = FName("PelvisSocket");
	IsClimbingLedge = false;

	LeftArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftArrow"));
	LeftArrow->SetupAttachment(GetRootComponent());

	RightArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightArrow"));
	RightArrow->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult Hit;

	ForwardTracer();
	HeightTracer(Hit);

}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayer::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnAtRate", this, &AMainPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &AMainPlayer::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMainPlayer::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("LeaveLedge", EInputEvent::IE_Pressed, this, &AMainPlayer::ExitLedge);

}

void AMainPlayer::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f) {

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if (!HangingFromLedge) {

			if (!Hanging) {
				AddMovementInput(Direction, Value);
			}
			else {
				HangingRope->AddSwingForce(Direction * Value, 300.f);
			}
		}



	}
}

void AMainPlayer::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f) {

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (!HangingFromLedge) {
			if (!Hanging) {
				AddMovementInput(Direction, Value);
			}
			else {
				HangingRope->AddSwingForce(Direction * Value, 300.f);
			}
		}
	}
}


void AMainPlayer::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	
}

void AMainPlayer::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainPlayer::GrabRope(ARope* Rope)
{
	if (!HangingRope) {
		HangingRope = Rope;

		FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);

		GetCapsuleComponent()->AttachToComponent(Rope->CableEnd, rules);

		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;


		UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(),
			FVector(0.f, 0.f, -100.f / Rope->CableEnd->GetRelativeScale3D().Z),
			GetCapsuleComponent()->GetRelativeRotation(),
			false,
			false,
			0.4f, true, EMoveComponentAction::Move, LatentInfo);
		Hanging = true;


	}
}



void AMainPlayer::Jump()
{
	if (HangingFromLedge) {
		ClimbLedge();
	}
	else {
		if (!Hanging) {
			Super::Jump();
		}
		else {

			Hanging = false;
			FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, false);
			GetCapsuleComponent()->DetachFromComponent(rules);


			FVector CableVelocity = HangingRope->CableEnd->ComponentVelocity;

			LaunchCharacter(CableVelocity * 300, false, false);


			GetWorldTimerManager().SetTimer(HangingHandle, this, &AMainPlayer::DisableHanging, HangingReloadTime);

		}

	}
}


void AMainPlayer::DisableHanging()
{
	HangingRope = nullptr;
}


/*****************************************************GRABBING LEDGE LOGIC******************************************************/

void AMainPlayer::ForwardTracer()
{
	FHitResult Hit;

	FVector Final = FVector(GetActorForwardVector().X * 150.f, GetActorForwardVector().Y * 150.f, GetActorForwardVector().Z);

	TArray<AActor*> ActorsToIgnore;

	bool DoesHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
								GetActorLocation(),
								GetActorLocation() + Final,
								TracerSphereRadius,
								UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), false,
								ActorsToIgnore,
								EDrawDebugTrace::ForOneFrame,
								Hit, true, FLinearColor::Red, FLinearColor::Green);

	if (DoesHit) {
		WallLocation = Hit.Location;
		WallNormal = Hit.Normal;
	}

}

void AMainPlayer::HeightTracer(FHitResult &Hit)
{

	//We start from +500 because we trace from above, because if we try directly to the point we collide with, it won't detect height
	FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 250.f) + GetActorForwardVector() * 70.f;

	FVector Final = FVector(Start.X, Start.Y, Start.Z - 250.f);


	TArray<AActor*> ActorsToIgnore;

	bool DoesHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
													Start,
													Final,
													TracerSphereRadius,
													UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), 
												false,
													ActorsToIgnore,
													EDrawDebugTrace::ForOneFrame,
													Hit, true, FLinearColor::Red, FLinearColor::Green);

	if (DoesHit) {
		LedgeHeightLocation = Hit.Location;

		float HeightDifference = GetMesh()->GetSocketLocation(PelvisSocket).Z - LedgeHeightLocation.Z;

		if ((HeightDifference >= -50 && HeightDifference <= 0) && !IsClimbingLedge) {
			GrabLedge();
		}
	}
}


void AMainPlayer::GrabLedge()
{
	Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance())->CanGrab(true);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	HangingFromLedge = true;

	FVector ExtendedNormal = (WallNormal * FVector(22.f, 22.f, 0.f));
	FVector TargetLocation = FVector(ExtendedNormal.X + WallLocation.X, ExtendedNormal.Y + WallLocation.Y, LedgeHeightLocation.Z - 120);

	FRotator RotationOnXVector = UKismetMathLibrary::MakeRotFromX(WallNormal);
	FRotator TargetRotation = FRotator(RotationOnXVector.Pitch, RotationOnXVector.Yaw - 180, RotationOnXVector.Roll);


	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), TargetLocation, TargetRotation
										, false, false, 0.13f, false, EMoveComponentAction::Move, LatentInfo);
	GetCharacterMovement()->StopMovementImmediately();
}

void AMainPlayer::ExitLedge()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance())->CanGrab(false);

	HangingFromLedge = false;

}

void AMainPlayer::ClimbLedge()
{

	if (!IsClimbingLedge) {

		Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance())->ClimbLedge(true);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		IsClimbingLedge = true;
		HangingFromLedge = false;
	}

}

void AMainPlayer::PlayClimbLedge(bool IsClimbing)
{
	IsClimbingLedge = IsClimbing;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);


	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), GetActorLocation(), FRotator(0.f, GetCapsuleComponent()->GetComponentRotation().Yaw, 0.f)
		, false, false, 0.1f, false, EMoveComponentAction::Move, LatentInfo);
	
	Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance())->CanGrab(false);
}


/*****************************************************LEDGE SIDE MOVEMENT***********************************************************/

void AMainPlayer::LeftTracer()
{
	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;

	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), LeftArrow->GetComponentLocation(), LeftArrow->GetComponentLocation(),
											20.f, 60.f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), false,
											ActorsToIgnore, EDrawDebugTrace::ForOneFrame, Hit, true, FLinearColor::Red,
											FLinearColor::Green);

	
}