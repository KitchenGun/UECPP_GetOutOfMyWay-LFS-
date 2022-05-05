#include "Tank/Component/CPP_MainGunSystemComponent.h"

#include "CPP_M1A1MainGunSystemComponent.h"

UCPP_MainGunSystemComponent::UCPP_MainGunSystemComponent()
{
}

void Server_MainGunFire();

bool UCPP_MainGunSystemComponent::Server_MainGunFire_Validate()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, L"Server_MainGunFire_Validate called");
	return true;
}
void UCPP_MainGunSystemComponent::Server_MainGunFire_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, L"ServerMaingunFireImplementation called");
}

void UCPP_MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(GetOwner()))
	{
		TankMesh = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	}
}

void UCPP_MainGunSystemComponent::ReloadDone()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	IsMainGunCanFire = true;
	if(GunReloadDoneFunc.IsBound())
		GunReloadDoneFunc.Execute();
}

void UCPP_MainGunSystemComponent::MainGunFire()
{
	if(IsMainGunCanFire)
	{
		IsMainGunCanFire = false;
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle,this,&UCPP_MainGunSystemComponent::ReloadDone,ReloadTime,false);
	}
}





