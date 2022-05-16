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
	UFUNCTION(NetMulticast,Reliable)
	void WheelParticle();
	void WheelParticle_Implementation();
	
	void OnFireParticle();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	//getset
	UFUNCTION(NetMulticast,Reliable)
	void SetIsMove(bool value);
	void SetIsMove_Implementation(bool value)
	{
		IsMove=value;
		WheelParticle();
	}
protected:
	virtual void BeginPlay() override;
	
private:
	class ACPP_Tank_Character* OwnerTank;
	//Particle
	class UParticleSystemComponent* MuzzleFlashEffect;
	class UParticleSystemComponent* ShockWaveEffect;
	UPROPERTY(EditAnywhere,Category="Effects")
	TArray<class UParticleSystemComponent*> WheelsEffect;

	UPROPERTY(Replicated)
	bool IsMove = false;
};
