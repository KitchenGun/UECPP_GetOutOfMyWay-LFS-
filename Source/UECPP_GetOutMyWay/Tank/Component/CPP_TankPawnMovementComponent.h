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

	//ABP�� ������ ���� ���� �Լ�
	void SetWheelSpeed(float WheelSpeed);
	void SetTurretAngleDetail();
	//�̵�
	void Movement(float DeltaTime);
	void OnMove(float value);
	void OnTurn(float value);
	
	void OnEngineBreak();
	void OffEngineBreak();

	
	//get&set
	FORCEINLINE float GetTrackSpeed() { return TrackSpeed; }
	FORCEINLINE float GetTurretAngle() { return TurretAngle; }
	FORCEINLINE float GetTurretAngleOffset() { return TurretAngleOffSet; }
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
	void EngineControl();
	void RPMControl();
	
	void UpdateTurretState(float DeltaTime);
	void TurretMove(float DeltaTime);
	void UpdateGunState(float DeltaTime);
	void GunMove(float DeltaTime);
private:
	class APawn* Owner;
	
	//mesh&ani
	class USkeletalMeshComponent* TankMesh;
	class UCPP_TankAnimInstance* TankAnimInst;

	//movement
	FVector NextLocation = FVector::ZeroVector;
	FRotator NextRotation = FRotator::ZeroRotator;
	FVector PrevPos = FVector::ZeroVector;
	float SpeedTimer = 0;
	float VirtualForwardVal=0;
	float VirtualFriction = 0.01f;
	float TankClimbingAngle =0;
	//TrackSpeed
	float TrackSpeed = 0;

	//Engine ����
	bool IsMoveForward = true;
	float TurnValue =0;
	float EngineTorque = 0.0f;
	int EngineGear = 0;
	float RPM = 500;
	bool IsAccelerating = false;
	bool IsTurning = false;
	float CurrentVelocity = 0;//
	float Speed = 100;
	bool isBreak = false;

	//Engine ��ü ���� ������ �����ͺ���
	float TurnSpeed = 35;//��ȸ �ӵ�
	class UCurveFloat* EngineTorqueCurve;
	int MaxEngineGear = 4;
	float IdleRPM = 200;
	float RPMDisplacement = 200;
	float MinRPM = 200;
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
	float TurretAngleOffSet=0.0f;
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
