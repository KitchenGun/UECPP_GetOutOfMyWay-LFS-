
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
	//레퍼런스 가져오기
	ProjectileClass =ConstructorHelpers::FClassFinder<ACPP_Projectile>
		(L"Blueprint'/Game/BP/Projectile/BP_ProjectileAP.BP_ProjectileAP_C'").Class;//AP탄
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
			//game inst 에 objpool매니져에 접근
			tempProjectile=Cast<class ACPP_Projectile>(ObjPoolManager->GetRecycledObject(0));
			if(tempProjectile!=nullptr)
			{//초기화할 객체가 존재하는 경우
				tempProjectile->OnRecycleStart(SpawnPos,Direction);
				FString temp = Owner->HasAuthority()?L"Server : ":L"Client : ";
				temp.Append(L"OnRecycleStart call");
				GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,temp);
			}
			else
			{//없으면 새로 생성
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				tempProjectile = GetWorld()->SpawnActor<ACPP_Projectile>(ProjectileClass,SpawnPos,Direction,SpawnParameters);
				//매니져에 새로 생성한 객체 추가
				ObjPoolManager->RegisterRecyclableObject<ACPP_Projectile>(tempProjectile);

				FString temp = Owner->HasAuthority()?L"Server : ":L"Client : ";
				temp.Append(L"SpawnActor call");
				GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,temp);
			}
			//발사 이펙트
			if(FireEffectFunc.IsBound())
				FireEffectFunc.Execute();
		
			//재장전관련 메소드는 Super	
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
