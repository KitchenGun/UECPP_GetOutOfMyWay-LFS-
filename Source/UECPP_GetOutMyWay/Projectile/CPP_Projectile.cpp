#include "Projectile/CPP_Projectile.h"

#include "DrawDebugHelpers.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particle/CPP_ParticleActor.h"
#include "Particles/ParticleSystemComponent.h"

ACPP_Projectile::ACPP_Projectile()
{
	bReplicates = true;
	//생성
	Root = CreateDefaultSubobject<USceneComponent>(L"Root");
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(L"Collider");
	Shell = CreateDefaultSubobject<UStaticMeshComponent>(L"Shell");
	WarHead = CreateDefaultSubobject<UStaticMeshComponent>(L"WarHead");
	Effect = CreateDefaultSubobject<UStaticMeshComponent>(L"Effect");
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(L"ProjectileMovement");
	SetParticle();

	//상속구조 정리
	SetRootComponent(Root);
	Capsule->SetupAttachment(Root);
	Shell->SetupAttachment(Capsule);
	WarHead->SetupAttachment(Capsule);
	Effect->SetupAttachment(Capsule);
	//S*R*T
	Capsule->SetRelativeRotation(FRotator(-90,0,0));
	Capsule->SetCapsuleHalfHeight(70);
	Capsule->SetCapsuleRadius(20);
	Shell->SetRelativeScale3D(FVector(10,10,10));
	Shell->SetRelativeRotation(FRotator(0,90,90));
	Shell->SetRelativeLocation(FVector(0,0,-10));
	WarHead->SetRelativeScale3D(FVector(0.05f,0.05f,0.05f));
	WarHead->SetRelativeRotation(FRotator(90,180,0));
	WarHead->SetRelativeLocation(FVector(0,0,30));
	Effect->SetRelativeScale3D(FVector(0.4f,0.1f,0.1f));
	Effect->SetRelativeRotation(FRotator(-90,0,0));
	Effect->SetRelativeLocation(FVector(0,0,45));
	//세부 설정
	Capsule->BodyInstance.SetCollisionProfileName("BlockAll");
	Capsule->SetNotifyRigidBodyCollision(true);
	ConstructorHelpers::FObjectFinder<UStaticMesh> shellMesh (L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_BulletShell_01.SM_BulletShell_01'");
	Shell->SetStaticMesh(shellMesh.Object);
	Shell->BodyInstance.SetCollisionProfileName("NoCollision");
	ConstructorHelpers::FObjectFinder<UStaticMesh> warheadMesh(L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_RocketBooster_02_SM.SM_RocketBooster_02_SM'");
	WarHead->SetStaticMesh(warheadMesh.Object);
	WarHead->BodyInstance.SetCollisionProfileName("NoCollision");
	ConstructorHelpers::FObjectFinder<UMaterial> warheadMat(L"Material'/Game/VigilanteContent/Shared/Levels/Platform/LOGO/Materials/M_Vigilante_Logo.M_Vigilante_Logo'");
	WarHead->SetMaterial(0,warheadMat.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> effectMesh(L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_RocketBooster_03_SM.SM_RocketBooster_03_SM'");
	Effect->SetStaticMesh(effectMesh.Object);
	Effect->BodyInstance.SetCollisionProfileName("NoCollision");
	ProjectileMovement->InitialSpeed = 1e+1f;
	ProjectileMovement->MaxSpeed = 1e+1f;
	ProjectileMovement->ProjectileGravityScale = 0;
	ProjectileMovement->SetIsReplicated(true);
	
}

int32 ACPP_Projectile::GetID() const
{
	return ObjectPoolID;
}

void ACPP_Projectile::SetID(int32 id)
{
	ObjectPoolID = id;
}

bool ACPP_Projectile::GetCanRecycle(int32 id) const
{
	//충돌체와 매쉬가 활성화 중이면 아래와 false 반환
	return IsCanRecycle;
}

void ACPP_Projectile::SetCanRecycle(bool value)
{
	IsCanRecycle = value;
}

void ACPP_Projectile::OnRecycleStart()
{
	
}

void ACPP_Projectile::OnRecycleStart(FVector pos, FRotator dir)
{
	
	FTransform Transform;
	Transform.SetLocation(pos);
	Transform.SetRotation(FQuat(dir));
	SetActorTransform(Transform);
	
	SetCanRecycle(false);
	//상태 킴
	IsOverlap=false;
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Shell->SetVisibility(true);
	WarHead->SetVisibility(true);
	Effect->SetVisibility(true);
	//capsule이 회전되어 있어서 이렇게 변경해서 사용함 -> -Capsule->GetUpVector()
	ProjectileMovement->Velocity = Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	StartPos = this->GetActorLocation();
	ProjectileMovement->SetComponentTickEnabled(true);
	
	if(!HasAuthority())
		Server_OnRecycleStart(pos,dir);
	
	GetWorldTimerManager().SetTimer(FlyHandler,this,&ACPP_Projectile::FlyTimeOver,FlyTime,false);
}


void ACPP_Projectile::Server_OnRecycleStart_Implementation(FVector pos,FRotator dir)
{
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,L"calllllll");
	FTransform Transform;
	Transform.SetLocation(pos);
	Transform.SetRotation(FQuat(dir));
	SetActorTransform(Transform);
	
	SetCanRecycle(false);
	//상태 킴
	IsOverlap=false;
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Shell->SetVisibility(true);
	WarHead->SetVisibility(true);
	Effect->SetVisibility(true);
	//capsule이 회전되어 있어서 이렇게 변경해서 사용함 -> -Capsule->GetUpVector()
	ProjectileMovement->Velocity = Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	StartPos = this->GetActorLocation();
	ProjectileMovement->SetComponentTickEnabled(true);
}

void ACPP_Projectile::Disable()
{
	//상태 정지
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Shell->SetVisibility(false);
	WarHead->SetVisibility(false);
	Effect->SetVisibility(false);
	ProjectileMovement->SetComponentTickEnabled(false);
	//타이머 초기화
	if (GetWorldTimerManager().IsTimerActive(FlyHandler))
		GetWorldTimerManager().ClearTimer(FlyHandler);

	SetCanRecycle(true);
}

void ACPP_Projectile::BeginPlay()
{
	SetReplicateMovement(true);
	//물리 충돌을 막기위해서 overlap 사용
	Capsule->OnComponentBeginOverlap.AddDynamic(this,&ACPP_Projectile::OnBeginOverlap);
	//capsule이 회전되어 있어서 이렇게 변경해서 사용함 -> -Capsule->GetUpVector()
	StartPos = this->GetActorLocation();
	ProjectileMovement->Velocity = Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	GetWorldTimerManager().SetTimer(FlyHandler,this,&ACPP_Projectile::FlyTimeOver,FlyTime,false);
	Super::BeginPlay();
}

float ACPP_Projectile::GetHitAngle(UPrimitiveComponent* OtherComp,const FHitResult& Hit)
{
	//SphereTraceMulti를 사용해서 overlap에서 구할수없는 hit pos를 구함
	{
		const FVector start=StartPos;
		const FVector end= Capsule->GetComponentLocation()+Capsule->GetUpVector()*300;
		TArray<AActor*> ignore;
		TArray<FHitResult> HitResults;
		
		//충돌 위치 반환
		//UKismetSystemLibrary::profile 충돌객체의 preset이 일치하고 있는 것만 반환함
		//UKismetSystemLibrary::object  충돌객체의 object type이 일치하는 것만 반환함
		//UWorld::channel 충돌객체의 channel이 일치하는 것만 반환함
		const bool isHit =
			UKismetSystemLibrary::LineTraceMulti(GetWorld(),start,end,
				ETraceTypeQuery::TraceTypeQuery3,false,ignore,EDrawDebugTrace::None,HitResults,true);
		

		if(isHit)
		{
			for(auto temp : HitResults)
			{
				if(Cast<UBoxComponent>(OtherComp)&&OtherComp->GetName()==temp.GetComponent()->GetName())
				{
					HitPos = temp.ImpactPoint;
				}
			}
		}
	}
	
	//맞은 부위 판별용
	FVector compScale =Cast<UBoxComponent>(OtherComp)->GetScaledBoxExtent();
	//InverseTransform
	FVector HitPosInverseTransform =OtherComp->GetComponentTransform().InverseTransformPosition(HitPos);
	

	//충돌된 컴포넌트의 방향 벡터
	FVector HitObjVec = FVector::ZeroVector;
	
	//충돌 면 파악
	if(FMath::IsNearlyEqual(abs(HitPosInverseTransform.Y),abs(compScale.Y),0.1f))
	{
		ProjectileHitDir = EHitDir::Side;
		HitObjVec = OtherComp->GetComponentRotation().Vector();
	}
	else if(FMath::IsNearlyEqual(HitPosInverseTransform.X,compScale.X,0.1f))
	{
		ProjectileHitDir = EHitDir::Front;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(0,90.0f,0)).Vector();
	}
	else if(FMath::IsNearlyEqual(HitPosInverseTransform.X,-compScale.X,0.1f))
	{
		ProjectileHitDir = EHitDir::Back;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(0,-90.0f,0)).Vector();
	}
	else if(FMath::IsNearlyEqual(HitPosInverseTransform.Z,compScale.Z,0.1f))
	{
		ProjectileHitDir = EHitDir::UpSide;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(-90.0f,0,0)).Vector();
	}
	else 
	{
		ProjectileHitDir = EHitDir::DownSide;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(90.0f,0,0)).Vector();
	}
	//충돌된 컴포넌트의 방향벡터 정규화
	HitObjVec = HitObjVec.GetSafeNormal();


	//충돌 방향 벡터
	FVector HitVec = GetActorLocation()-StartPos;
	HitVec =HitVec.GetSafeNormal();

	//두벡터의 세타를 구해야함
	float angle =FMath::Acos(FVector::DotProduct(HitVec,HitObjVec));
	angle = FMath::RadiansToDegrees(angle);
	return angle;
}


void ACPP_Projectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//상속 받은 다음 충돌시 결과를 다르게 보내는 것으로 여러 탄종을 구현할려고 함
	//충돌시 이펙트 생성
	AddParticle();
	
	//비활성화
	Disable();
}

void ACPP_Projectile::AddParticle()
{
	UCPP_ObjectPoolManager* ObjPoolManager = Cast<UCPP_MultiplayGameInstance>(GetGameInstance())->GetManagerClass<UCPP_ObjectPoolManager>();
	
	UParticleSystem* EffectType;

	//이펙트 설정
	switch (ProjectileHitEffect)
	{
	case EHitEffect::TankHit:
		EffectType = HitArmor;
		break;
	case EHitEffect::TankRicochet:
		EffectType = HitRicochet;
		break;
	case EHitEffect::Ground:
		EffectType = HitGround;
		break;
	default:
		EffectType = HitGround;
		break;
	}
	
	//game inst에 objpool매니져에 접근
	ACPP_ParticleActor* temp=Cast<ACPP_ParticleActor>(ObjPoolManager->GetRecycledObject(1));
	
	if(temp!=nullptr)
	{//초기화할 객체가 존재하는 경우
		temp->OnRecycleStart(this->GetActorLocation(),EffectType);
	}
	else
	{//없으면 새로 생성
		temp = GetWorld()->SpawnActor<ACPP_ParticleActor>(ParticleActorClass,this->GetActorLocation(),FRotator::ZeroRotator);
		temp->SetParticle(EffectType);
		//매니져에 새로 생성한 객체 추가
		ObjPoolManager->RegisterRecyclableObject<ACPP_ParticleActor>(temp);
	}
}

void ACPP_Projectile::SetParticle()
{
	ParticleActorClass = ConstructorHelpers::FClassFinder<ACPP_ParticleActor>(L"Blueprint'/Game/BP/Effect/BP_ParticleActor.BP_ParticleActor_C'").Class;
	HitArmor = ConstructorHelpers::FObjectFinder<UParticleSystem>(L"ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Side.P_Explosion_Side'").Object;
	HitRicochet = ConstructorHelpers::FObjectFinder<UParticleSystem>(L"ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Sparks/P_Sparks_C.P_Sparks_C'").Object;
	HitGround = ConstructorHelpers::FObjectFinder<UParticleSystem>(L"ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Brick.P_Brick'").Object;
}


void ACPP_Projectile::FlyTimeOver()
{
	Disable();
}

void ACPP_Projectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_Projectile,IsCanRecycle);
}
