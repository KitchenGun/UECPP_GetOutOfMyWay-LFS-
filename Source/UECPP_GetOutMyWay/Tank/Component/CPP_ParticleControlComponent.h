#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tank/CPP_Tank_Character.h"
#include "CPP_ParticleControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_ParticleControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCPP_ParticleControlComponent();
	UFUNCTION()
	void OnRepWheelParticle();
	
	void OnFireParticle();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	//getset
	UFUNCTION()
	void SetIsMove(bool value)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "IsControlled");
		IsMove=value;
		if(GetOwnerRole()==ROLE_Authority)
		{
			OnRepWheelParticle();
		}
	}
protected:
	virtual void BeginPlay() override;
	
private:
	class ACPP_Tank_Character* OwnerTank;
	//Particle
	class UParticleSystemComponent* MuzzleFlashEffect;
	class UParticleSystemComponent* ShockWaveEffect;
	TArray<class UParticleSystemComponent*> WheelsEffect;

	UPROPERTY(ReplicatedUsing=OnRepWheelParticle)
	bool IsMove = false;
};
