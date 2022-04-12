
#include "Tank/Component/CPP_M1A1MainGunSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Projectile/CPP_Projectile.h"
#include "Tank/CPP_Tank_Pawn.h"

void UCPP_M1A1MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner=Cast<ACPP_Tank_Pawn>(GetOwner());
	if(IsValid(Owner))
	{
		Owner->FireFunc.BindUFunction(this,"MainGunFire");
	}
	//ź�� ����//����� 0�� ����Ұ���
	Ammunition.SetNum(4);//5�� ź�� ���� ���� Ȯ��
	Ammunition[0] = (int32)20;
	Ammunition[1] = (int32)10;
	Ammunition[2] = (int32)10;
	
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
	if(IsMainGunCanFire)
	{
		if(ProjectileClass)
		{
			FVector SpawnPos	= TankMesh->GetSocketLocation("gun_1_jntSocket");
			FRotator Direction   = Owner->GetController()->GetControlRotation();

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
				temp = GetWorld()->SpawnActor<ACPP_Projectile>(ProjectileClass,SpawnPos,Direction);
				//�Ŵ����� ���� ������ ��ü �߰�
				ObjPoolManager->RegisterRecyclableObject<ACPP_Projectile>(temp);
			}
			//��ź�� �߻��� ��� �̸��� ��Ʈ�ѷ��� ����
			temp->SetEventInstigator(FString(GetOwner()->GetName()),Owner->GetController());
			if(FireEffectFunc.IsBound())
				FireEffectFunc.Execute();
		}
	}
	//���������� �޼ҵ�� Super	
	Super::MainGunFire();
}
