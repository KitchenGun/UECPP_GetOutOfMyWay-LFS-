#include "Tank/Component/CPP_MainGunSystemComponent.h"

#include "CPP_M1A1MainGunSystemComponent.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
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

void UCPP_MainGunSystemComponent::Server_MainGunFire_Implementation(FVector SpawnPos, FRotator Direction)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, L"ServerMaingunFireImplementation called");

	UCPP_ObjectPoolManager* ObjPoolManager = Cast<UCPP_MultiplayGameInstance>(Owner->GetGameInstance())->GetManagerClass<UCPP_ObjectPoolManager>();
	ACPP_Projectile* temp;
	//game inst �� objpool�Ŵ����� ����
	temp=Cast<ACPP_Projectile>(ObjPoolManager->GetRecycledObject(0));
	if(temp!=nullptr)
	{//�ʱ�ȭ�� ��ü�� �����ϴ� ���
		temp->OnRecycleStart(SpawnPos,Direction);
	}
	else
	{//������ ���� ����
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		temp = GetWorld()->SpawnActor<ACPP_Projectile>(ProjectileClass,SpawnPos,Direction,SpawnParameters);
		//�Ŵ����� ���� ������ ��ü �߰�
		ObjPoolManager->RegisterRecyclableObject<ACPP_Projectile>(temp);
	}
	//��ź�� �߻��� ��� �̸��� ��Ʈ�ѷ��� ����
	temp->SetEventInstigator(FString(GetOwner()->GetName()),Owner->GetController());
	
	if(FireEffectFunc.IsBound())
		FireEffectFunc.Execute();
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





