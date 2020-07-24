// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCPPProject/Public/Characters/FPSCharacter.h"
#include "FPSCPPProject/Public/CharacterInterface/CharacterInterfaceBase.h"
#include "FPSCPPProject/Public/Health/HealthComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetInputLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	TraceDistance = 200000.0f;

	Dead = false;
	bShooting = false;
	bCrouching = false;
	Speed = 0.0f;
	bFwdPressed = false;
	bRightPressed = false; 

}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

/*
*If a controller exists and the player is being told to move a non-zero distance from its position,
*determine the player's yaw rotation, and use the yaw rotation to determine the direction to move in.
*Then, add movement in that direction
*@param value - How much to move the player by.
*/
void AFPSCharacter::MoveForward(float value)
{

	//We know there is a controller and that the movement value passed is not zero. 
	if (Controller && value != 0.0f)
	{

		bFwdPressed = true; 

		if (bCrouching) 
		{

			value = value / 2.0f;

		}

		FString FValue = FString::SanitizeFloat(value);

		Speed = value; 

		const FRotator Rotation = Controller->GetControlRotation(); //We are getting the player's rotation based on the controller. 
		const FRotator YawRotation(0, Rotation.Yaw, 0); //We now know the player's YawRotation. 

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //We are getting the direction to move the player in based on that YawRotation and the X Axis. 
		AddMovementInput(Direction, value); //We are moving the player based on direction and value. 

	}

}

/*
*If a controller exists and the player is being told to move a non-zero distance from its position,
*determine the player's yaw rotation, and use the yaw rotation to determine the direction to move in.
*Then, add movement in that direction
*@param value - How much to move the player by.
*/
void AFPSCharacter::MoveRight(float value)
{

	//We know there is a controller and that the movement value passed is not zero. 
	if (Controller && value != 0.0f)
	{

		bRightPressed = true; 

		if (bCrouching) 
		{

			value = value / 2.0f;

		}

		Speed = value;

		const FRotator Rotation = Controller->GetControlRotation();//We are getting the player's rotation based on the controller. 
		const FRotator YawRotation(0, Rotation.Yaw, 0);//We now know the player's YawRotation. 

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);//We are getting the direction to move the player in based on that YawRotation and the Y Axis. 
		AddMovementInput(Direction, value);//We are moving the player based on direction and value. 

	}

}

void AFPSCharacter::TraceForward_Implementation()
{

	FVector Location; 
	FRotator Rotation; 
	FHitResult Hit; 

	GetController()->GetPlayerViewPoint(Location, Rotation);

	FVector Start = Location; 
	FVector End = Start + (Rotation.Vector() * TraceDistance);

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 3.0f);

	//Make sure we are hitting something and that we are not accidentally hitting something attached to the player. 
	if (bHit && Hit.GetActor() != this) 
	{

		//DrawDebugBox(GetWorld(), Hit.ImpactPoint,FVector(5,5,5), FColor::Emerald, false, 2.0f); //Debug box for debugging purposes. Shows a box where the line trace hits something in the world. 
		AActor* OtherCharacter = Hit.GetActor(); //Another Character in the world hit by the line trace. 

		//We know an actor was hit. 
		if (OtherCharacter) 
		{
			
			//We know that the hit character is not the one currently focused. 
			if (OtherCharacter != FocusedActor) 
			{

				//If we have a focused actor, end the focus on it. 
				if (FocusedActor) 
				{

					EndFocused();

				}

				FocusedActor = OtherCharacter; //Set the focus actor to the current Character being hit. 
				StartFocused(); //Then start the focus on the newly hit Character. 

			}

		}

	}
	//If nothing is hit or if the line trace hits something attached to the player, check to see if we have a focused actor. 
	else
	{

		//If we have a focused actor, end the focus and set focused actor to null. 
		if (FocusedActor)
		{

			EndFocused();
			FocusedActor = nullptr;

		}

	}

}

void AFPSCharacter::Shoot_Implementation() 
{

	bShooting = true; //Setting the boolean bShooting to true. bShooting determines whether or not the player is still shooting their gun. 

	FVector Location; //Player camera location. 
	FRotator Rotation; //Player camera rotation. 
	FHitResult Hit; //Hit result. Will be based on line trace in this function. 

	GetController()->GetPlayerViewPoint(Location, Rotation); //setting the camera location and rotation. 

	FVector Start = Location; //Start location for the line trace. Based on camera location. 
	FVector End = Start + (Rotation.Vector() * TraceDistance); //End location for the line trace. Based on camera starting location, the camera's rotation vector, and the line trace distance determined in the player's constructor. 

	FCollisionQueryParams TraceParams; //The line trace parameters. 
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams); //Determining if something was hit. 

	//If we hit someone when we shoot, find out where the other player was shot and determine damage. 
	if (bHit)
	{

		AActor* OtherCharacter = Hit.GetActor(); //Another Character in the world hit by the line trace. 
		

		UPrimitiveComponent* HitComponent = Hit.GetComponent(); //The component hit by the line trace. 

		float Damage = DamageToApply(HitComponent); //Determining the damage dealt to the Other Character based on where it was hit. 
		FDamageEvent DamageEvent; //The damage event to occur. 
		
		OtherCharacter->TakeDamage(Damage, DamageEvent, this->GetController(), this); //Telling the Other Character to take damage. 

		//if (OtherCharacter)
		//{

		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Losing Health"));

		//}




		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, HitComponent->GetName());

	}

}

//Called when the player releases the shoot key. Sets the boolean bShooting to false to signal the game to stop shooting the player's gun. 
void AFPSCharacter::StopShooting()
{

	bShooting = false;
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Stop Shooting"));
	

}

void AFPSCharacter::Reload()
{
}

void AFPSCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
}

void AFPSCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//do a line trace from the player's camera every tick. 
	TraceForward();

	//If the player is holding down the shoot key, keep shooting. 
	if (bShooting) 
	{

		//Shoot();

	}

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AFPSCharacter::Shoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AFPSCharacter::StopShooting);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSCharacter::Crouching);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &APawn::AddControllerYawInput);

}

void AFPSCharacter::StartFocus_Implementation()
{

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, TEXT("Start Focus"));

}

void AFPSCharacter::EndFocus_Implementation()
{
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("End Focus"));

}

void AFPSCharacter::StartFocused()
{

	//Get the interface the Focused Actor is currently using
	ICharacterInterfaceBase* Interface = Cast<ICharacterInterfaceBase>(FocusedActor);

	//If the focused actor is using the interactable interface, start the focus
	if (Interface)
	{

		Interface->Execute_StartFocus(FocusedActor);

	}

}

void AFPSCharacter::EndFocused()
{

	//Get the interface the Focused Actor is currently using
	ICharacterInterfaceBase* Interface = Cast<ICharacterInterfaceBase>(FocusedActor);

	//If the focused actor is using the interactable interface, start the focus
	if (Interface)
	{

		Interface->Execute_EndFocus(FocusedActor);

	}

}

//Determines what damage to apply to the hit actor based on the component hit. 
//@param HitComponent is the component hit by the line trace done in the Shoot() function. The name of this component determines the damage done. 
float AFPSCharacter::DamageToApply(UPrimitiveComponent* HitComponent)
{
	
	//If the player lands a headshot, do 90 damage. 
	if (HitComponent->GetName() == "Head") 
	{

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "90.0");
		return 90.0f;

	}
	//If the player lands a shot on the torso or shoulders, do 40 damage. 
	else if (HitComponent->GetName() == "Torso" || HitComponent->GetName() == "Shoulder_Left" || HitComponent->GetName() == "Shoulder_Right")
	{

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "40.0");
		return 40.0f;

	}
	//If the player lands a shot anywhere else, do 20 damage. 
	else 
	{

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "20.0");
		return 20.0f;

	}

}

//Sets crouching to the opposite of what it was set as before. Allows for a 'toggle' functionality with the crouch key. 
void AFPSCharacter::Crouching() 
{

	bCrouching = !bCrouching;

}

void AFPSCharacter::NotCrouching() 
{

	bCrouching = false;

}

void AFPSCharacter::FwdReleased()
{

	bFwdPressed = false;
	if (!bRightPressed) 
	{

		Speed = 0.0f;

	}

}

void AFPSCharacter::RightReleased() 
{

	bRightPressed = false; 
	if (!bFwdPressed) 
	{

		Speed = 0.0f;

	}

}
