#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_MainGunSystemComponent.generated.h"

DECLARE_DELEGATE(FFireEffect)
DECLARE_DELEGATE(FReloadDone)
DECLARE_DELEGATE_OneParam(FSetAMMO,int32)
DECLARE_DELEGATE_OneParam(FCamShake,float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_MainGunSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPP_MainGunSystemComponent();
	UFUNCTION()
	virtual void MainGunFire();

	FFireEffect FireEffectFunc;
	FReloadDone FGunReloadDoneFunc;
	FSetAMMO FSetAmmoFunc;
	FCamShake FCamShakeFunc;

	FORCEINLINE bool GetIsMainGunCanFire() {return IsMainGunCanFire;}
	
	UFUNCTION(NetMulticast,Reliable)
	void SetIsMainGunCanFire(bool value);
	void SetIsMainGunCanFire_Implementation(bool value);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
protected:
	virtual void BeginPlay() override;
private:
	void ReloadDone();
	
protected:
	class ACPP_Tank_Character* Owner;
	
	UPROPERTY(EditDefaultsOnly, Category = "Shell")
	TSubclassOf<class ACPP_Projectile> ProjectileClass;

	//reload
	FTimerHandle ReloadTimerHandle;
	float ReloadTime = 1.0f;
	UPROPERTY(Replicated)
	bool IsMainGunCanFire = true;
	//mesh
	class USkeletalMeshComponent* TankMesh;
	//particle
	TSubclassOf<class ACPP_ParticleActor> ParticleActorClass;
	
};
