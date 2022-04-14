#include "Particle/CPP_ParticleActor.h"

#include "Particles/ParticleSystemComponent.h"

ACPP_ParticleActor::ACPP_ParticleActor()
{
	Particle = CreateDefaultSubobject<UParticleSystemComponent>(L"Particle");
}

void ACPP_ParticleActor::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(PlayTimer,this,&ACPP_ParticleActor::Disable,ParticleDuration,false);
}

void ACPP_ParticleActor::SetParticle(class UParticleSystem* particle)
{
	Particle->Template = particle;
	Particle->Activate();
	UE_LOG(LogTemp,Display,L"%s",*particle->GetName());
	UE_LOG(LogTemp,Display,L"%s",*this->GetActorLocation().ToString());
}

void ACPP_ParticleActor::Disable()
{
	UE_LOG(LogTemp,Display,L"Disable")
	Particle->SetVisibility(false);
	SetCanRecycle(true);
}

int32 ACPP_ParticleActor::GetID() const
{
	return ObjectPoolID;
}

void ACPP_ParticleActor::SetID(int32 id)
{
	ObjectPoolID = id;
}

bool ACPP_ParticleActor::GetCanRecycle(int32 id) const
{
	return IsCanRecycle;
}

void ACPP_ParticleActor::SetCanRecycle(bool value)
{
	IsCanRecycle = value;
}

void ACPP_ParticleActor::OnRecycleStart()
{
	IsCanRecycle = false;
	Particle->SetVisibility(true);
	Particle->Activate(true);
	GetWorld()->GetTimerManager().SetTimer(PlayTimer,this,&ACPP_ParticleActor::Disable,ParticleDuration,false);
}

void ACPP_ParticleActor::OnRecycleStart(FVector pos,class UParticleSystem* particle)
{
	this->SetActorLocation(pos);
	SetParticle(particle);
	OnRecycleStart();
}


