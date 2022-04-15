#pragma once
#include "CoreMinimal.h"
#include "Common/Interface/CPP_Objectpooling.h"
#include "GameFramework/Actor.h"
#include "CPP_Projectile.generated.h"

UENUM()
enum class EHitDir
{
	Front,
	Side,
	Back,
	UpSide,
	DownSide,
	Max
};


UENUM()
enum class EHitEffect
{
	TankHit,
	TankRicochet,
	Ground,
	Max
};

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Projectile : public AActor,public ICPP_Objectpooling
{
	GENERATED_BODY()
	
public:	
	ACPP_Projectile();

	FORCEINLINE	void SetEventInstigator(FString objName,AController* playerCtrl)
	{
		ObjName=objName;
		PlayerCtrl = playerCtrl;
	}

	//objectpooling 기법
	//ID를 가지고 객체 구분에 필요한 함수
	virtual int32 GetID() const override;
	virtual void SetID(int32 id) override;
	//재사용 가능 여부 나타내는 함수
	virtual bool GetCanRecycle(int32 id) const override;
	virtual void SetCanRecycle(bool value) override;
	//재활용시 호출되는 함수
	virtual void OnRecycleStart() override;
	void OnRecycleStart(FVector pos,FRotator dir);
	virtual void Disable();
protected:
	virtual void BeginPlay() override;

	float GetHitAngle(UPrimitiveComponent* OtherComp, const FHitResult& Hit);
	
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void AddParticle();

	void SetParticle();
	
	FORCEINLINE void HitEffectSet(EHitEffect value)
	{
		ProjectileHitEffect = value;
	}

	UFUNCTION()
	void FlyTimeOver();
protected:
	//데미지
	float Damage = 20.0f;
	//발사정보
	FString ObjName;
	AController* PlayerCtrl = nullptr;
	//피격 방향
	EHitDir ProjectileHitDir = EHitDir::Max;

	TSubclassOf<UDamageType> DamageType = nullptr;
	//충돌
	FVector HitPos =FVector::ZeroVector;
	bool IsOverlap = false;
	EHitEffect ProjectileHitEffect= EHitEffect::Max; 
private:
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* Root;
	UPROPERTY(VisibleDefaultsOnly)
	class UCapsuleComponent* Capsule;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Shell;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* WarHead;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Effect;
	//방향
	FVector StartPos=FVector::ZeroVector;
	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	//particle
	class UParticleSystem* HitGround;
	class UParticleSystem* HitRicochet;
	class UParticleSystem* HitArmor;
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	TSubclassOf<class ACPP_ParticleActor> ParticleActorClass;
	
	//Objectpool
	int32 ObjectPoolID = 0;
	bool IsCanRecycle = false;

	//자동 비활성화를 위한 타이머
	FTimerHandle FlyHandler;
	float FlyTime = 5.0f;
};
