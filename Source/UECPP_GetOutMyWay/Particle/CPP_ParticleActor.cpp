#include "Particle/CPP_ParticleActor.h"

#include "Particles/ParticleSystemComponent.h"

ACPP_ParticleActor::ACPP_ParticleActor()
{
	Particle = CreateDefaultSubobject<UParticleSystemComponent>(L"Particle");
	SetRootComponent(Particle);
}

void ACPP_ParticleActor::BeginPlay()
{
	Super::BeginPlay();
	Particle->OnSystemFinished.AddDynamic(this,&ACPP_ParticleActor::Disable);
}

void ACPP_ParticleActor::SetParticle(class UParticleSystem* particle)
{
	Particle->SetTemplate(particle);
	Particle->Activate();
}

void ACPP_ParticleActor::Disable(class UParticleSystemComponent* PSystem)
{
	PSystem->SetVisibility(false);
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
}

void ACPP_ParticleActor::OnRecycleStart(FVector pos,class UParticleSystem* particle)
{
	this->SetActorLocation(pos);
	SetParticle(particle);
	OnRecycleStart();
}


