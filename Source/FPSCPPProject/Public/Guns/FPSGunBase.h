// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSGunBase.generated.h"

UCLASS()
class FPSCPPPROJECT_API AFPSGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSGunBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Magazine")
	int32 MaxMagazine;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Magazine")
	int32 CurrentMagazine;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float HeadDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float ChestDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float LimbDamage; 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
