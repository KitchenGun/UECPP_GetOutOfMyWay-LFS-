#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CPP_TankPawnMovementComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FTurretMoveStart);
DECLARE_DYNAMIC_DELEGATE(FTurretMoveEnd);

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_TankPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	UCPP_TankPawnMovementComponent();
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	//ABP�� ������ ���� ���� �Լ�
	void SetWheelSpeed(float WheelSpeed);
	//�̵�
	UFUNCTION(Server,Reliable)
	void Movement();
	void Movement_Implementation();

	UFUNCTION(Server, Reliable)
	void OnMove(float value);
	void OnMove_Implementation(float value);

	UFUNCTION(Server, Reliable)
	void OnTurn(float value);
	void OnTurn_Implementation(float value);

	void OnEngineBreak();
	void OffEngineBreak();

	
	//get&set
	FORCEINLINE float GetTrackSpeed() { return TrackSpeed; }
	FORCEINLINE float GetTurretAngle() { return TurretAngle; }
	FORCEINLINE float GetGunAngle() {return GunAngle;}
	FORCEINLINE float GetGunAngleOffset() { return GunAngleOffSet; }
	FORCEINLINE bool GetIsMove() {return IsAccelerating||IsTurning;}
	FORCEINLINE void FixRotatorDirSize()
	{
		if(SightRotator.Yaw>TurretRotator.Yaw)
		{//��ž���� �����ʺ�����
			LeftAngle = TurretRotator.Yaw+(360.0f-SightRotator.Yaw);
			RightAngle = SightRotator.Yaw - TurretRotator.Yaw;
		}
		else
		{//��ž���� ���ʺ�����
			LeftAngle = TurretRotator.Yaw - SightRotator.Yaw;
			RightAngle = SightRotator.Yaw+(360.0f-TurretRotator.Yaw);
		}
	}

public:
	FTurretMoveStart TurretMoveStartFunc;
	FTurretMoveEnd TurretMoveEndFunc;
private:
	UFUNCTION(Server,Reliable)
	void EngineControl();
	void EngineControl_Implementation();
	UFUNCTION(Server,Reliable)
	void RPMControl();
	void RPMControl_Implementation();
	
	UFUNCTION(Server,Reliable)
	void UpdateTurretState(float DeltaTime);
	void UpdateTurretState_Implementation(float DeltaTime);
	UFUNCTION(Server,Reliable)
	void TurretMove(float DeltaTime);
	void TurretMove_Implementation(float DeltaTime);
	UFUNCTION(Server,Reliable)
	void UpdateGunState(float DeltaTime);
	void UpdateGunState_Implementation(float DeltaTime);
	UFUNCTION(Server,Reliable)
	void GunMove(float DeltaTime);
	void GunMove_Implementation(float DeltaTime);
private:
	class APawn* Owner;
	
	//mesh&ani
	class USkeletalMeshComponent* TankMesh;
	class UCPP_TankAnimInstance* TankAnimInst;

	//movement
	UPROPERTY(ReplicatedUsing=OnRep_NextTransformUpdate)
	FVector NextLocation = FVector::ZeroVector;
	
	UPROPERTY(ReplicatedUsing=OnRep_NextTransformUpdate)
	FRotator NextRotation = FRotator::ZeroRotator;
	UFUNCTION()
	void OnRep_NextTransformUpdate();
	UPROPERTY(Replicated)
	FVector PrevPos = FVector::ZeroVector;
	UPROPERTY(Replicated)
	float SpeedTimer = 0;
	UPROPERTY(Replicated)
	float VirtualForwardVal=0;
	UPROPERTY(Replicated)
	float VirtualFriction = 0.01f;
	UPROPERTY(Replicated)
	float TankClimbingAngle =0;
	//TrackSpeed
	UPROPERTY(Replicated)
	float TrackSpeed = 0;

	//Engine ����
	UPROPERTY(Replicated)
	bool IsMoveForward = true;
	UPROPERTY(Replicated)
	float TurnValue =0;
	UPROPERTY(Replicated)
	float EngineTorque = 0.0f;
	UPROPERTY(Replicated)
	int EngineGear = 0;
	UPROPERTY(Replicated)
	float RPM = 500;
	UPROPERTY(Replicated)
	bool IsAccelerating = false;
	UPROPERTY(Replicated)
	bool IsTurning = false;
	UPROPERTY(Replicated)
	float CurrentVelocity = 0;
	UPROPERTY(Replicated)
	float Speed = 100;
	UPROPERTY(Replicated)
	bool isBreak = false;

	//Engine ��ü ���� ������ �����ͺ���
	UPROPERTY(Replicated)
	float TurnSpeed = 35;//��ȸ �ӵ�
	class UCurveFloat* EngineTorqueCurve;
	UPROPERTY(Replicated)
	int MaxEngineGear = 4;
	UPROPERTY(Replicated)
	float IdleRPM = 200;
	float RPMDisplacement = 200;
	UPROPERTY(Replicated)
	float MinRPM = 200;
	UPROPERTY(Replicated)
	float MaxRPM = 900;

	//Turret
	FRotator SightRotator =  FRotator::ZeroRotator;
	FRotator TurretRotator = FRotator::ZeroRotator; //world�� �����ϵ��� �ؾ���
	bool IsTurretAngleMatch = true;
	bool IsSightRight = false; //��ü �������� �������ΰ� �����ΰ�
	bool IsTurretRight = false; //��ü �������� �������ΰ� �����ΰ�
	FVector SightDir = FVector::ZeroVector;
	FVector TurretDir = FVector::ZeroVector;
	float TurretAngle = 0.0f;
	float LeftAngle=0;
	float RightAngle=0;
	//Turret ��ü ���� �����ؾ��� ������ ����
	float TurretTurnSpeed = 100.0f;

	//Gun
	bool IsGunAngleMatch = true;
	FRotator GunRotator = FRotator::ZeroRotator;
	bool IsGunUpZero=true;
	bool IsSightUpZero=true;
	float GunAngle=0.0f;
	float GunAngleOffSet=0.0f;
	
	//Gun ��ü ���� �����ؾ��� ������ ����
	float GunMoveSpeed = 50.0f;
	
};
