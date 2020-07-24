// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCPPProject/Public/CharacterInterface/CharacterInterfaceBase.h"
#include "FPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

UCLASS()
class FPSCPPPROJECT_API AFPSCharacter : public ACharacter, public ICharacterInterfaceBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* SpringArmComp; //The Spring Arm Component for the FPSCharacter. Length will usually be 0 unless situations/events cause length to temporarily increase. 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* CameraComp; //The FPS Camera Component for the FPSCharacter. 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		UStaticMeshComponent* MeshComp; //The Static Mesh Component of the FPSCharacter. 

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
		float TraceDistance; //The distance that the line trace will go for the FPSCharacter. 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool Dead;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void Shoot();
	void Shoot_Implementation();
	void StopShooting();
	void Reload();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void StartFocus();
	virtual void StartFocus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void EndFocus();
	virtual void EndFocus_Implementation();

private:
	AActor* FocusedActor;
	void StartFocused();
	void EndFocused();
	float DamageToApply(UPrimitiveComponent* HitComponent);
	void Crouching();
	void NotCrouching();

	bool bShooting;
	bool bCrouching;

};
