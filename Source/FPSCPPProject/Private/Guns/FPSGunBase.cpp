// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCPPProject/Public/Guns/FPSGunBase.h"

// Sets default values
AFPSGunBase::AFPSGunBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxAmmo = 150;
	CurrentAmmo = MaxAmmo;
	MaxMagazine = 30;
	CurrentMagazine = MaxMagazine;

	HeadDamage = 90.0f;
	ChestDamage = 40.0f;
	LimbDamage = 20.0f; 

}

// Called when the game starts or when spawned
void AFPSGunBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

