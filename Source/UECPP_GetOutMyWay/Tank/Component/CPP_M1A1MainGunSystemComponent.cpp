
#include "Tank/Component/CPP_M1A1MainGunSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Projectile/CPP_Projectile.h"
#include "Tank/CPP_Tank_Character.h"


void UCPP_M1A1MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	//탄약 세팅//현재는 0만 사용할거임
	Ammunition.SetNum(4);//5개 탄종 넣을 공간 확보
	Ammunition[0] = (int32)20;
	Ammunition[1] = (int32)10;
	Ammunition[2] = (int32)10;
	
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
		//game inst 에 objpool매니져에 접근
		temp=Cast<ACPP_Projectile>(ObjPoolManager->GetRecycledObject(0));
		if(temp!=nullptr)
		{//초기화할 객체가 존재하는 경우
			temp->OnRecycleStart(SpawnPos,Direction);
		}
		else
		{//없으면 새로 생성
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			temp = GetWorld()->SpawnActor<ACPP_Projectile>(ProjectileClass,SpawnPos,Direction,SpawnParameters);
			//매니져에 새로 생성한 객체 추가
			ObjPoolManager->RegisterRecyclableObject<ACPP_Projectile>(temp);
		}
		//포탄에 발사한 사람 이름과 컨트롤러를 던짐
		temp->SetEventInstigator(FString(GetOwner()->GetName()),Owner->GetController());
		if(FireEffectFunc.IsBound())
			FireEffectFunc.Execute();
		//재장전관련 메소드는 Super	
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
	//game inst 에 objpool매니져에 접근
	temp=Cast<ACPP_Projectile>(ObjPoolManager->GetRecycledObject(0));
	if(temp!=nullptr)
	{//초기화할 객체가 존재하는 경우
		temp->OnRecycleStart(SpawnPos,Direction);
	}
	else
	{//없으면 새로 생성
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		temp = GetWorld()->SpawnActor<ACPP_Projectile>(ProjectileClass,SpawnPos,Direction,SpawnParameters);
		//매니져에 새로 생성한 객체 추가
		ObjPoolManager->RegisterRecyclableObject<ACPP_Projectile>(temp);
	}
	//포탄에 발사한 사람 이름과 컨트롤러를 던짐
	temp->SetEventInstigator(FString(GetOwner()->GetName()),Owner->GetController());
	if(FireEffectFunc.IsBound())
		FireEffectFunc.Execute();
	
	//재장전관련 메소드는 Super	
	Super::MainGunFire();
}
