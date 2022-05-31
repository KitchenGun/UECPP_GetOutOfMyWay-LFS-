#include "Tank/Component/CPP_MainGunSystemComponent.h"

#include "CPP_M1A1MainGunSystemComponent.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Projectile/CPP_Projectile.h"
#include "Tank/CPP_Tank_Character.h"

UCPP_MainGunSystemComponent::UCPP_MainGunSystemComponent()
{
	Owner=Cast<ACPP_Tank_Character>(GetOwner());
	if(IsValid(Owner))
	{
		Owner->FireFunc.BindUFunction(this,"MainGunFire");
	}
}

void UCPP_MainGunSystemComponent::SetIsMainGunCanFire_Implementation(bool value)
{
	IsMainGunCanFire = value;
	
	if(FSetAmmoFunc.IsBound()&&Owner->IsLocallyControlled())
		FSetAmmoFunc.Execute(value?1:0);
}

void UCPP_MainGunSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCPP_MainGunSystemComponent,IsMainGunCanFire);
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
	if(FSetAmmoFunc.IsBound()&&Owner->IsLocallyControlled())
		FSetAmmoFunc.Execute(1);
	SetIsMainGunCanFire(true);
	
	if(FGunReloadDoneFunc.IsBound())
		FGunReloadDoneFunc.Execute();
}

void UCPP_MainGunSystemComponent::MainGunFire()
{
	if(IsMainGunCanFire)
	{
		IsMainGunCanFire = false;
		if(FSetAmmoFunc.IsBound()&&Owner->IsLocallyControlled())
			FSetAmmoFunc.Execute(0);
		SetIsMainGunCanFire(false);
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle,this,&UCPP_MainGunSystemComponent::ReloadDone,ReloadTime,false);
	}
}





