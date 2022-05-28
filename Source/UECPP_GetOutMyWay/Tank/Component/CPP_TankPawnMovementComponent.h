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
	void SetWheelSpeed(float WheelSpeed);
	UFUNCTION(NetMulticast,Reliable)
	void Server_SetWheelSpeed(float WheelSpeed);
	void Server_SetWheelSpeed_Implementation(float WheelSpeed);
	//이동
	UFUNCTION(Server,Reliable)
	void Movement();
	void Movement_Implementation();

	void OnMove(float value);
	void OnTurn(float value);
	
	UFUNCTION(Server, Reliable)
	void OnEngineBreak();
	void OnEngineBreak_Implementation();
	
	UFUNCTION(Server, Reliable)
	void OffEngineBreak();
	void OffEngineBreak_Implementation();
	
	//get&set
	FORCEINLINE float GetTrackSpeed() { return TrackSpeed; }
	FORCEINLINE float GetTurretAngle(){return TurretAngle;}

	FORCEINLINE float GetGunAngle() {return GunAngle;}
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
	//turret
	UFUNCTION()
	void OnRep_SightRotation(FRotator value);
	UFUNCTION()
	void OnRep_TurretIsMatch(bool value);
	UFUNCTION(Server,Reliable)
	void Server_SetTurretAngle(float value);
	void Server_SetTurretAngle_Implementation(float value);
	UFUNCTION(NetMulticast,Reliable)
	void NetMulticastSetTurretAngle(float value);
	void NetMulticastSetTurretAngle_Implementation(float value);

	UFUNCTION()
	void OnRep_GunIsMatch(bool value);
	UFUNCTION(Server,Reliable)
	void Server_SetGunAngle(float value);
	void Server_SetGunAngle_Implementation(float value);
	UFUNCTION(NetMulticast,Reliable)
	void NetMulticastSetGunAngle(float value);
	void NetMulticastSetGunAngle_Implementation(float value);
	
	//engine
	UFUNCTION(Server,Reliable)
	void EngineControl();
	void EngineControl_Implementation();
	UFUNCTION(Server,Reliable)
	void RPMControl();
	void RPMControl_Implementation();
	
	UFUNCTION()
	void UpdateTurretState(float DeltaTime);
	UFUNCTION()
	void TurretMove(float DeltaTime);
	UFUNCTION()
	void UpdateGunState(float DeltaTime);
	UFUNCTION()
	void GunMove(float DeltaTime);
private:
	class APawn* Owner = nullptr;
	
	//mesh&ani
	class USkeletalMeshComponent* TankMesh = nullptr;
	class UCPP_TankAnimInstance* TankAnimInst = nullptr;

	//movement
	FVector NextLocation = FVector::ZeroVector;
	float InputVal;
	FRotator NextRotation = FRotator::ZeroRotator;
	FVector PrevPos = FVector::ZeroVector;
	float SpeedTimer = 0;
	float VirtualForwardVal=0;
	float VirtualFriction = 0.01f;
	float TankClimbingAngle =0;
	//TrackSpeed
	float TrackSpeed = 0;

	//Engine 변수
	bool IsMoveForward = true;
	float TurnValue =0;
	float EngineTorque = 0.0f;
	int EngineGear = 0;
	float RPM = 500;
	bool IsAccelerating = false;
	bool IsTurning = false;
	float CurrentVelocity = 0;
	UPROPERTY(Replicated)
	float Speed = 100;
	bool isBreak = false;

	//Engine 객체 별로 수정할 데이터변수
	float TurnSpeed = 35;//선회 속도
	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* EngineTorqueCurve;
	int MaxEngineGear = 4;
	float IdleRPM = 200;
	float RPMDisplacement = 200;
	float MinRPM = 200;
	float MaxRPM = 900;

	//Turret
	UPROPERTY(ReplicatedUsing=OnRep_SightRotation)
	FRotator SightRotator =  FRotator::ZeroRotator;
	FRotator TurretRotator = FRotator::ZeroRotator; //world로 연산하도록 해야함
	UPROPERTY(ReplicatedUsing=OnRep_TurretIsMatch)
	bool IsTurretAngleMatch = true;
	bool IsSightRight = false; //차체 기준으로 오른쪽인가 왼쪽인가
	bool IsTurretRight = false; //차체 기준으로 오른쪽인가 왼쪽인가
	FVector SightDir = FVector::ZeroVector;
	FVector TurretDir = FVector::ZeroVector;
	UPROPERTY(Replicated)
	float TurretAngle = 0.0f;
	float LeftAngle=0;
	float RightAngle=0;
	//Turret 객체 별로 수정해야할 데이터 변수
	float TurretTurnSpeed = 100.0f;

	//Gun
	UPROPERTY(ReplicatedUsing=OnRep_GunIsMatch)
	bool IsGunAngleMatch = true;
	FRotator GunRotator = FRotator::ZeroRotator;
	bool IsGunUpZero=true;
	bool IsSightUpZero=true;
	UPROPERTY(Replicated)
	float GunAngle=0.0f;
	
	//Gun 객체 별로 수정해야할 데이터 변수
	float GunMoveSpeed = 50.0f;
	
};
