#pragma once

#include "CoreMinimal.h"
#include "Common/Interface/CPP_Objectpooling.h"
#include "GameFramework/Actor.h"
#include "CPP_ParticleActor.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_ParticleActor : public AActor,public ICPP_Objectpooling
{
	GENERATED_BODY()
	
public:	
	ACPP_ParticleActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* Particle;
	void SetParticle(class UParticleSystem* particle);
	void Disable();
	//objectpooling ���
	//ID�� ������ ��ü ���п� �ʿ��� �Լ�
	virtual int32 GetID() const override;
	virtual void SetID(int32 id) override;
	//���� ���� ���� ��Ÿ���� �Լ�
	virtual bool GetCanRecycle(int32 id) const override;
	virtual void SetCanRecycle(bool value) override;
	//��Ȱ��� ȣ��Ǵ� �Լ�
	virtual void OnRecycleStart() override;
	void OnRecycleStart(FVector pos,class UParticleSystem* particle);

private:
	//Objectpool
	int32 ObjectPoolID = 1;
	bool IsCanRecycle = false;

	float ParticleDuration = 2.0f; 

	FTimerHandle PlayTimer;
};
