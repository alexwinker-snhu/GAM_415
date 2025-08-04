// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "GAM415Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Comp");
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");

	RootComponent = boxComp;
	mesh->SetupAttachment(boxComp); // mesh attached to box component
	sceneCapture->SetupAttachment(mesh); // scene capture setup to mesh

	mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverLapBegin); // sets up box comp
	mesh->SetHiddenInSceneCapture(true); // set hidden in scene capture to true

	if (mat)
	{
		mesh->SetMaterial(0, mat); // sets material 
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePortals();
}

void APortal::OnOverLapBegin(class UPrimitiveComponent* OverLappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGAM415Character* playerChar = Cast<AGAM415Character>(OtherActor);

	if (playerChar)
	{
		if (OtherPortal)
		{
			if (!playerChar->isTeleporting)
			{
				playerChar->isTeleporting = true; // make sure is teleporting true 
				FVector loc = OtherPortal->GetActorLocation(); // get location of other portal
				playerChar->SetActorLocation(loc); // set player to location of other portal

				FTimerHandle TimerHandle; // sets up timer handle
				FTimerDelegate TimerDelegate; // set up timer delegate
				TimerDelegate.BindUFunction(this, "SetBool", playerChar); // binds timer delegate to a bool and 
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1, false); // set timer manager


			}
		}
	}
}

void APortal::SetBool(AGAM415Character* playerChar)
{
	if (playerChar)
	{
		playerChar->isTeleporting = false; // set teleporting to false
	}
}

void APortal::UpdatePortals()
{
	FVector Location = this->GetActorLocation() - OtherPortal->GetActorLocation(); // get location of actor
	FVector camLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentLocation();// get camera location
	FRotator camRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentRotation();// get camera rotation
	FVector CombinedLocation = camLocation + Location; // finds both location

	sceneCapture->SetWorldLocationAndRotation(CombinedLocation, camRotation); //set scene capture with location and rotation
}



