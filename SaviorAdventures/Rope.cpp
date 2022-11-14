// Fill out your copyright notice in the Description page of Project Settings.


#include "Rope.h"
#include "CableComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayer.h"
#include "Components/SceneComponent.h"


// Sets default values
ARope::ARope()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OurRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OurRootComponent"));
	SetRootComponent(OurRootComponent);

	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	Cable->SetupAttachment(GetRootComponent());
	Cable->bAttachEnd = false;
	Cable->CableLength = 400.f;

	CableEnd = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CableEnd"));
	CableEnd->SetupAttachment(GetRootComponent());
	ConstructorHelpers::FObjectFinder<UStaticMesh> CableEndAsset(TEXT("StaticMesh '/Engine/BasicShapes/Sphere.Sphere'")); //Para encontrar la esfera
	CableEnd->SetStaticMesh(CableEndAsset.Object);
	CableEnd->SetSimulatePhysics(true);

	Cable->AttachEndTo.ComponentProperty = FName("CableEnd");
	Cable->bAttachEnd = true;
	Cable->EndLocation = FVector(0.f);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RopePhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(GetRootComponent());
	
	FConstrainComponentPropName name1;
	FConstrainComponentPropName name2;
	name1.ComponentName = FName("OurRootComponent");
	name2.ComponentName = FName("CableEnd");
	
	PhysicsConstraint->ComponentName1 = name1;
	PhysicsConstraint->ComponentName2 = name2;

	EndCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EndCollider"));
	EndCollider->SetupAttachment(CableEnd);

	CableEnd->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	
	MaxHeight = 800.f;
}

// Called when the game starts or when spawned
void ARope::BeginPlay()
{
	Super::BeginPlay();

	EndCollider->OnComponentBeginOverlap.AddDynamic(this, &ARope::OnOverlapBegin);
	EndCollider->OnComponentEndOverlap.AddDynamic(this, &ARope::OnOverlapEnd);
	
}

// Called every frame
void ARope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARope::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);

	if (MainPlayer) {
		MainPlayer->GrabRope(this);
	}

}

void ARope::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ARope::AddSwingForce(FVector Direction, float ForceRate)
{
	float diff = GetActorLocation().Z - CableEnd->GetComponentTransform().GetLocation().Z;

	if (diff >= MaxHeight) {
		CableEnd->AddForce(Direction * ForceRate, NAME_None, true);
	}
}

