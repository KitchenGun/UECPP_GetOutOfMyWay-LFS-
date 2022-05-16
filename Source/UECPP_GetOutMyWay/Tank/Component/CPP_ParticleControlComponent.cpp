#include "Tank/Component/CPP_ParticleControlComponent.h"

#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Tank/CPP_Tank_Character.h"

UCPP_ParticleControlComponent::UCPP_ParticleControlComponent()
{
	if(IsValid(GetOwner()))
		OwnerTank = Cast<ACPP_Tank_Character>(GetOwner());
	
}

void UCPP_ParticleControlComponent::WheelParticle_Implementation()
{
	for(int i =0;i<WheelsEffect.Num();i++)
	{
		WheelsEffect[i]->SetActive(IsMove);
	}
}

void UCPP_ParticleControlComponent::BeginPlay()
{
	Super::BeginPlay();
	
	MuzzleFlashEffect = OwnerTank->GetMuzzleFlashEffect();
	ShockWaveEffect = OwnerTank->GetShockWaveEffect();
	WheelsEffect = OwnerTank->GetWheelsEffect();
}


void UCPP_ParticleControlComponent::OnFireParticle()
{
	//particle 사용
	MuzzleFlashEffect->Activate(true);
	ShockWaveEffect->Activate(true);
	FVector Start = MuzzleFlashEffect->GetComponentLocation();
	FVector End = MuzzleFlashEffect->GetComponentLocation();
	TArray<AActor*> ignore;
	TArray<FHitResult> HitResults;
	TArray<AActor*> ImpactArray;
	float blastRange = 1000;
	float ShockWaveForce=1e+2;
	//포 발사에 따른 충격파 구현
	const bool Hit =
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,blastRange,
			ETraceTypeQuery::TraceTypeQuery2,false,ignore,EDrawDebugTrace::None,HitResults,true);
	if(Hit)
	{
		int32 index;
		for(FHitResult temp:HitResults)
		{
			AActor* tempActor=Cast<AActor>(temp.Actor);
			if(IsValid(tempActor))
			{
				ImpactArray.Find(tempActor,index);
				if(index==INDEX_NONE)
				{
					ImpactArray.Add(tempActor);
					UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(tempActor->GetRootComponent());
					if(IsValid(MeshComponent)&&MeshComponent->BodyInstance.bSimulatePhysics)
					{
						MeshComponent->AddImpulse(FVector(MeshComponent->GetComponentLocation()-(Start-FVector(0,200,0)))*ShockWaveForce);
					}
				}
			}
		}
	}
	
}

void UCPP_ParticleControlComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UCPP_ParticleControlComponent,IsMove,COND_OwnerOnly);
}






