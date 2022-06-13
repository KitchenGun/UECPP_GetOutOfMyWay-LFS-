
#include "Tank/Component/CPP_M1A1MainGunSystemComponent.h"
#include "Projectile/CPP_Projectile.h"
#include "Camera/CameraComponent.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Tank/CPP_Tank_Character.h"



void UCPP_M1A1MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner= Cast<ACPP_Tank_Character>(GetOwner());
	ObjPoolManager = Cast<UCPP_MultiplayGameInstance>(Owner->GetGameInstance())->GetManagerClass<UCPP_ObjectPoolManager>();
}

UCPP_M1A1MainGunSystemComponent::UCPP_M1A1MainGunSystemComponent()
{
	//���۷��� ��������
	ProjectileClass =ConstructorHelpers::FClassFinder<ACPP_Projectile>
		(L"Blueprint'/Game/BP/Projectile/BP_ProjectileAP.BP_ProjectileAP_C'").Class;//APź
	ReloadTime =M1A1ReloadTime;
}

void UCPP_M1A1MainGunSystemComponent::MainGunFire()
{
	if(Owner->HasAuthority())
	{
		if(IsMainGunCanFire&&IsValid(ProjectileClass))
		{
			FVector SpawnPos	= TankMesh->GetSocketLocation("gun_1_jntSocket");
			FRotator Direction = TankMesh->GetSocketRotation("gun_1_jntSocket");
			tempProjectile = nullptr;
			//game inst �� objpool�Ŵ����� ����
			tempProjectile=Cast<class ACPP_Projectile>(ObjPoolManager->GetRecycledObject(0));
			if(tempProjectile!=nullptr)
			{//�ʱ�ȭ�� ��ü�� �����ϴ� ���
				tempProjectile->OnRecycleStart(SpawnPos,Direction);
				FString temp = Owner->HasAuthority()?L"Server : ":L"Client : ";
				temp.Append(L"OnRecycleStart call");
				GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,temp);
			}
			else
			{//������ ���� ����
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				tempProjectile = GetWorld()->SpawnActor<ACPP_Projectile>(ProjectileClass,SpawnPos,Direction,SpawnParameters);
				//�Ŵ����� ���� ������ ��ü �߰�
				ObjPoolManager->RegisterRecyclableObject<ACPP_Projectile>(tempProjectile);

				FString temp = Owner->HasAuthority()?L"Server : ":L"Client : ";
				temp.Append(L"SpawnActor call");
				GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,temp);
			}
			//�߻� ����Ʈ
			if(FireEffectFunc.IsBound())
				FireEffectFunc.Execute();
		
			//���������� �޼ҵ�� Super	
			Super::MainGunFire();
		}
	}
	else
	{
		Server_MainGunFire();
	}
}

void UCPP_M1A1MainGunSystemComponent::Server_MainGunFire_Implementation()
{
	MainGunFire();
}
