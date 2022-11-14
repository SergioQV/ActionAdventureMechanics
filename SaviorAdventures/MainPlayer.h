// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"

UCLASS()
class SAVIORADVENTURES_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

private:
	void ForwardTracer();
	void HeightTracer(FHitResult &Hit);
	void LeftTracer();

	void GrabLedge();
	void ExitLedge();
	void ClimbLedge();

public:
	// Sets default values for this character's properties
	AMainPlayer();

	/** Camera Boom Positioning the Camera Behind the player	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**Follow Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**Base turn rates to scale turning functions for the camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rope")
	class ARope* HangingRope;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rope")
	bool Hanging;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope")
	float HangingReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climbing")
	float TracerSphereRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climbing")
	FVector LedgeHeightLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climbing")
	FVector WallLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climbing")
	FVector WallNormal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climbing")
	FName PelvisSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Climbing")
	bool IsClimbingLedge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Climbing")
	bool HangingFromLedge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SideMovement")
	UArrowComponent* LeftArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SideMovement")
	UArrowComponent* RightArrow;


	FTimerHandle HangingHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	/** Called via input to turn at a given Rate
	* @param Rate This is a normalized rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given Rate
	* @param Rate This is a normalized rate
	*/
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void GrabRope(ARope* Rope);

	void DisableHanging();

	void PlayClimbLedge(bool IsClimbing);
};
