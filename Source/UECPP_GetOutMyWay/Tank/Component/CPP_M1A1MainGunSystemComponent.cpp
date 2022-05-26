
#include "Tank/Component/CPP_M1A1MainGunSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Projectile/CPP_Projectile.h"
#include "Tank/CPP_Tank_Character.h"


void UCPP_M1A1MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
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
	FString temps = Owner->HasAuthority()?L"Server : ":L"Client : ";
	temps.Append(IsMainGunCanFire?L"IsMainGunCanFire true":L"IsMainGunCanFire false");
	GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::White,temps);

	if(IsMainGunCanFire&&IsValid(ProjectileClass))
	{
		FVector SpawnPos	= TankMesh->GetSocketLocation("gun_1_jntSocket");
		FRotator Direction = TankMesh->GetSocketRotation("gun_1_jntSocket");

		UE_LOG(LogTemp,Display,L"server call");
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
		//���������� �޼ҵ�� Super	
		Super::MainGunFire();
		
		if(!Owner->HasAuthority())
			Server_MainGunFire(SpawnPos,Direction);
	}
	
}

void UCPP_M1A1MainGunSystemComponent::Server_MainGunFire_Implementation(FVector SpawnPos, FRotator Direction)
{
	UE_LOG(LogTemp,Display,L"client call");
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
	
	//���������� �޼ҵ�� Super	
	Super::MainGunFire();
}
