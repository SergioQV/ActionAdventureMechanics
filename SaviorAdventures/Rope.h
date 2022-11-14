// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rope.generated.h"

UCLASS()
class SAVIORADVENTURES_API ARope : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARope();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	class UCableComponent* Cable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	USceneComponent* OurRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	UStaticMeshComponent* CableEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	class UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	class UCapsuleComponent* EndCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	float MaxHeight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void AddSwingForce(FVector Direction, float ForceRate);

};
