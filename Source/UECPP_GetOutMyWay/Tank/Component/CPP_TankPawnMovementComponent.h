#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CPP_TankPawnMovementComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FTurretMoveStart);
DECLARE_DYNAMIC_DELEGATE(FTurretMoveEnd);

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_TankPawnMovementComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCPP_TankPawnMovementComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	//ABP에 전달할 변수 설정 함수
	UFUNCTION(Server,Reliable)
	void SetWheelSpeed(float WheelSpeed);
	void SetWheelSpeed_Implementation(float WheelSpeed);
	//이동
	UFUNCTION(Server,Reliable)
	void Movement();
	void Movement_Implementation();

	UFUNCTION(Server, Reliable)
	void OnMove(float value);
	void OnMove_Implementation(float value);

	UFUNCTION(Server, Reliable)
	void OnTurn(float value);
	void OnTurn_Implementation(float value);
	
	UFUNCTION(Server, Reliable)
	void OnEngineBreak();
	void OnEngineBreak_Implementation();
	
	UFUNCTION(Server, Reliable)
	void OffEngineBreak();
	void OffEngineBreak_Implementation();
	
	//get&set
	FORCEINLINE float GetTrackSpeed() { return TrackSpeed; }
	FORCEINLINE float GetTurretAngle() { return TurretAngle; }
	FORCEINLINE float GetGunAngle() {return GunAngle;}
	FORCEINLINE float GetGunAngleOffset() { return GunAngleOffSet; }
	FORCEINLINE bool GetIsMove() {return IsAccelerating||IsTurning;}
	FORCEINLINE void FixRotatorDirSize()
	{
		if(SightRotator.Yaw>TurretRotator.Yaw)
		{//포탑기준 오른쪽보는중
			LeftAngle = TurretRotator.Yaw+(360.0f-SightRotator.Yaw);
			RightAngle = SightRotator.Yaw - TurretRotator.Yaw;
		}
		else
		{//포탑기준 왼쪽보는중
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
	UPROPERTY(Replicated)
	FVector NextLocation = FVector::ZeroVector;
	UPROPERTY(Replicated)
	float InputVal;
	UPROPERTY(Replicated)
	FRotator NextRotation = FRotator::ZeroRotator;
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

	//Engine 변수
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

	//Engine 객체 별로 수정할 데이터변수
	UPROPERTY(Replicated)
	float TurnSpeed = 35;//선회 속도
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
	UPROPERTY(Replicated)
	FRotator SightRotator =  FRotator::ZeroRotator;
	UPROPERTY(Replicated)
	FRotator TurretRotator = FRotator::ZeroRotator; //world로 연산하도록 해야함
	UPROPERTY(Replicated)
	bool IsTurretAngleMatch = true;
	UPROPERTY(Replicated)
	bool IsSightRight = false; //차체 기준으로 오른쪽인가 왼쪽인가
	UPROPERTY(Replicated)
	bool IsTurretRight = false; //차체 기준으로 오른쪽인가 왼쪽인가
	UPROPERTY(Replicated)
	FVector SightDir = FVector::ZeroVector;
	UPROPERTY(Replicated)
	FVector TurretDir = FVector::ZeroVector;
	UPROPERTY(Replicated)
	float TurretAngle = 0.0f;
	UPROPERTY(Replicated)
	float LeftAngle=0;
	UPROPERTY(Replicated)
	float RightAngle=0;
	//Turret 객체 별로 수정해야할 데이터 변수
	float TurretTurnSpeed = 100.0f;

	//Gun
	bool IsGunAngleMatch = true;
	FRotator GunRotator = FRotator::ZeroRotator;
	bool IsGunUpZero=true;
	bool IsSightUpZero=true;
	float GunAngle=0.0f;
	float GunAngleOffSet=0.0f;
	
	//Gun 객체 별로 수정해야할 데이터 변수
	float GunMoveSpeed = 50.0f;
	
};
