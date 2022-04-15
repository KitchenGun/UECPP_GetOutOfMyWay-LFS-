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

	//objectpooling ���
	//ID�� ������ ��ü ���п� �ʿ��� �Լ�
	virtual int32 GetID() const override;
	virtual void SetID(int32 id) override;
	//���� ���� ���� ��Ÿ���� �Լ�
	virtual bool GetCanRecycle(int32 id) const override;
	virtual void SetCanRecycle(bool value) override;
	//��Ȱ��� ȣ��Ǵ� �Լ�
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
	//������
	float Damage = 20.0f;
	//�߻�����
	FString ObjName;
	AController* PlayerCtrl = nullptr;
	//�ǰ� ����
	EHitDir ProjectileHitDir = EHitDir::Max;

	TSubclassOf<UDamageType> DamageType = nullptr;
	//�浹
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
	//����
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

	//�ڵ� ��Ȱ��ȭ�� ���� Ÿ�̸�
	FTimerHandle FlyHandler;
	float FlyTime = 5.0f;
};
