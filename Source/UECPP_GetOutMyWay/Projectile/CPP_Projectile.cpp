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
	//����
	Root = CreateDefaultSubobject<USceneComponent>(L"Root");
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(L"Collider");
	Shell = CreateDefaultSubobject<UStaticMeshComponent>(L"Shell");
	WarHead = CreateDefaultSubobject<UStaticMeshComponent>(L"WarHead");
	Effect = CreateDefaultSubobject<UStaticMeshComponent>(L"Effect");
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(L"ProjectileMovement");
	SetParticle();

	//��ӱ��� ����
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
	//���� ����
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
	//�浹ü�� �Ž��� Ȱ��ȭ ���̸� �Ʒ��� false ��ȯ
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
	//���� Ŵ
	IsOverlap=false;
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Shell->SetVisibility(true);
	WarHead->SetVisibility(true);
	Effect->SetVisibility(true);
	//capsule�� ȸ���Ǿ� �־ �̷��� �����ؼ� ����� -> -Capsule->GetUpVector()
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
	//���� Ŵ
	IsOverlap=false;
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Shell->SetVisibility(true);
	WarHead->SetVisibility(true);
	Effect->SetVisibility(true);
	//capsule�� ȸ���Ǿ� �־ �̷��� �����ؼ� ����� -> -Capsule->GetUpVector()
	ProjectileMovement->Velocity = Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	StartPos = this->GetActorLocation();
	ProjectileMovement->SetComponentTickEnabled(true);
}

void ACPP_Projectile::Disable()
{
	//���� ����
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Shell->SetVisibility(false);
	WarHead->SetVisibility(false);
	Effect->SetVisibility(false);
	ProjectileMovement->SetComponentTickEnabled(false);
	//Ÿ�̸� �ʱ�ȭ
	if (GetWorldTimerManager().IsTimerActive(FlyHandler))
		GetWorldTimerManager().ClearTimer(FlyHandler);

	SetCanRecycle(true);
}

void ACPP_Projectile::BeginPlay()
{
	SetReplicateMovement(true);
	//���� �浹�� �������ؼ� overlap ���
	Capsule->OnComponentBeginOverlap.AddDynamic(this,&ACPP_Projectile::OnBeginOverlap);
	//capsule�� ȸ���Ǿ� �־ �̷��� �����ؼ� ����� -> -Capsule->GetUpVector()
	StartPos = this->GetActorLocation();
	ProjectileMovement->Velocity = Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	GetWorldTimerManager().SetTimer(FlyHandler,this,&ACPP_Projectile::FlyTimeOver,FlyTime,false);
	Super::BeginPlay();
}

float ACPP_Projectile::GetHitAngle(UPrimitiveComponent* OtherComp,const FHitResult& Hit)
{
	//SphereTraceMulti�� ����ؼ� overlap���� ���Ҽ����� hit pos�� ����
	{
		const FVector start=StartPos;
		const FVector end= Capsule->GetComponentLocation()+Capsule->GetUpVector()*300;
		TArray<AActor*> ignore;
		TArray<FHitResult> HitResults;
		
		//�浹 ��ġ ��ȯ
		//UKismetSystemLibrary::profile �浹��ü�� preset�� ��ġ�ϰ� �ִ� �͸� ��ȯ��
		//UKismetSystemLibrary::object  �浹��ü�� object type�� ��ġ�ϴ� �͸� ��ȯ��
		//UWorld::channel �浹��ü�� channel�� ��ġ�ϴ� �͸� ��ȯ��
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
	
	//���� ���� �Ǻ���
	FVector compScale =Cast<UBoxComponent>(OtherComp)->GetScaledBoxExtent();
	//InverseTransform
	FVector HitPosInverseTransform =OtherComp->GetComponentTransform().InverseTransformPosition(HitPos);
	

	//�浹�� ������Ʈ�� ���� ����
	FVector HitObjVec = FVector::ZeroVector;
	
	//�浹 �� �ľ�
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
	//�浹�� ������Ʈ�� ���⺤�� ����ȭ
	HitObjVec = HitObjVec.GetSafeNormal();


	//�浹 ���� ����
	FVector HitVec = GetActorLocation()-StartPos;
	HitVec =HitVec.GetSafeNormal();

	//�κ����� ��Ÿ�� ���ؾ���
	float angle =FMath::Acos(FVector::DotProduct(HitVec,HitObjVec));
	angle = FMath::RadiansToDegrees(angle);
	return angle;
}


void ACPP_Projectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//��� ���� ���� �浹�� ����� �ٸ��� ������ ������ ���� ź���� �����ҷ��� ��
	//�浹�� ����Ʈ ����
	AddParticle();
	
	//��Ȱ��ȭ
	Disable();
}

void ACPP_Projectile::AddParticle()
{
	UCPP_ObjectPoolManager* ObjPoolManager = Cast<UCPP_MultiplayGameInstance>(GetGameInstance())->GetManagerClass<UCPP_ObjectPoolManager>();
	
	UParticleSystem* EffectType;

	//����Ʈ ����
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
	
	//game inst�� objpool�Ŵ����� ����
	ACPP_ParticleActor* temp=Cast<ACPP_ParticleActor>(ObjPoolManager->GetRecycledObject(1));
	
	if(temp!=nullptr)
	{//�ʱ�ȭ�� ��ü�� �����ϴ� ���
		temp->OnRecycleStart(this->GetActorLocation(),EffectType);
	}
	else
	{//������ ���� ����
		temp = GetWorld()->SpawnActor<ACPP_ParticleActor>(ParticleActorClass,this->GetActorLocation(),FRotator::ZeroRotator);
		temp->SetParticle(EffectType);
		//�Ŵ����� ���� ������ ��ü �߰�
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
