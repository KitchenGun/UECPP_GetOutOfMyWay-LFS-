#include "Tank/Component/CPP_TankPawnMovementComponent.h"
#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"
#include "Net/UnrealNetwork.h"
#include "Tank/CPP_Tank_Character.h"

UCPP_TankPawnMovementComponent::UCPP_TankPawnMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Owner = Cast<APawn>(GetOwner());
}

void UCPP_TankPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if(Owner != nullptr)
		TankMesh = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (TankMesh != nullptr)
	{
		TankAnimInst = Cast<UCPP_TankAnimInstance>(TankMesh->GetAnimInstance());
	}
}

void UCPP_TankPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	if(!Owner->IsLocallyControlled())
		return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Engine
	EngineControl();
	Movement();
	if(!IsDead)
	{
		//Turret
		UpdateTurretState(DeltaTime);
		//Gun
		UpdateGunState(DeltaTime);
	}
	//dead
	IsDead = Cast<ACPP_Tank_Character>(Owner)->GetIsDead();
}


void UCPP_TankPawnMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCPP_TankPawnMovementComponent,SightRotator);
	DOREPLIFETIME(UCPP_TankPawnMovementComponent,IsTurretAngleMatch);
	DOREPLIFETIME(UCPP_TankPawnMovementComponent,TurretAngle);
	DOREPLIFETIME(UCPP_TankPawnMovementComponent,IsGunAngleMatch);
	DOREPLIFETIME(UCPP_TankPawnMovementComponent,GunAngle);
	DOREPLIFETIME(UCPP_TankPawnMovementComponent,Speed);
}


void UCPP_TankPawnMovementComponent::SetWheelSpeed(float WheelSpeed)
{
	
	if(IsAccelerating)
	{
		TrackSpeed = WheelSpeed*0.2f;
	}
	else
	{
		TrackSpeed = TurnValue*2.0f;
	}
	Server_SetWheelSpeed(TrackSpeed);
}

void UCPP_TankPawnMovementComponent::Server_SetWheelSpeed_Implementation(float WheelSpeed)
{
	TrackSpeed = WheelSpeed;
}


void UCPP_TankPawnMovementComponent::Movement_Implementation()
{
	if (Owner != nullptr && !NextLocation.IsNearlyZero())
	{
		NextLocation = GetOwner()->GetActorLocation() + (NextLocation * GetWorld()->DeltaTimeSeconds * Speed);
		Owner->SetActorRelativeLocation(NextLocation);
	}
	if (Owner != nullptr && !NextRotation.IsNearlyZero())
	{
		NextRotation = GetOwner()->GetActorRotation() + (NextRotation * GetWorld()->DeltaTimeSeconds * TurnSpeed);
		//정지시 조금씩 앞으로 이동하도록 제작
		if (!IsAccelerating)
		{
			NextLocation =Owner->GetActorLocation() + (Owner->GetActorForwardVector() * GetWorld()->DeltaTimeSeconds * 8.0f);
			Owner->SetActorRelativeLocation(NextLocation);
		}
		Owner->SetActorRelativeRotation(NextRotation);
	}
	NextLocation = FVector::ZeroVector;
	NextRotation = FRotator::ZeroRotator;
}

void UCPP_TankPawnMovementComponent::OnMove(float value)
{
	TankClimbingAngle = Owner->GetActorRotation().Pitch;
	FVector dir = Owner->GetActorForwardVector();

	if (value > 0 && !isBreak)
	{
		IsAccelerating = true;
		MaxEngineGear = 4;
		if (!IsMoveForward)
		{
			EngineGear = 0;
			RPMControl();
			RPM = FMath::Clamp<float>(RPM, MinRPM, MaxRPM);
			IsMoveForward = true;
		}
		if (!FMath::IsNearlyEqual(VirtualForwardVal, 1))
			VirtualForwardVal = FMath::Clamp(VirtualForwardVal + 0.01f, 0.0f, 1.0f);
	}
	else if (FMath::IsNearlyZero(value) || isBreak)
	{
		IsAccelerating = false;
		IsMoveForward = true;

		if (VirtualForwardVal > 0)
		{
			VirtualForwardVal = FMath::Clamp(VirtualForwardVal - VirtualFriction, 0.0f, 1.0f);
		}
		else if (VirtualForwardVal < 0)
		{
			VirtualForwardVal = FMath::Clamp(VirtualForwardVal + VirtualFriction, -1.0f, 0.0f);
		}
	}
	else if (!isBreak)
	{
		IsAccelerating = true;
		MaxEngineGear = 2;
		//기어에 맞게 RPM 조절
		if (IsMoveForward)
		{
			EngineGear = 0;
			RPMControl();
			RPM = FMath::Clamp<float>(RPM, MinRPM, MaxRPM);
			IsMoveForward = false;
		}
		if (!FMath::IsNearlyEqual(VirtualForwardVal, -1))
			VirtualForwardVal = FMath::Clamp(VirtualForwardVal - 0.01f, -1.0f, 0.0f);
	}

	//등판각에 따른 속도 조절
	float TankClimbingAnglePercentage = 0.0f;
	if (TankClimbingAngle > 0)
	{//올라가는 상황
		TankClimbingAnglePercentage = TankClimbingAngle / 30;
	}
	else if (TankClimbingAngle < 0)
	{//내려가는 상황
		TankClimbingAnglePercentage = TankClimbingAngle / 30;
	}
	else
	{
		TankClimbingAnglePercentage = 0;
	}

	//뒤집히면 못움직이게 함
	float TankYaw = Owner->GetActorRotation().Roll;
	if (TankYaw<70 && TankYaw>-70)
	{//최종 움직임 전달
		NextLocation += (dir * (VirtualForwardVal - TankClimbingAnglePercentage));
		InputVal = VirtualForwardVal - TankClimbingAnglePercentage;
		CurrentVelocity = (Speed*3.14f);//바퀴의 회전속도에 맞게 적용
		/*출력용*/
		//if(Owner->IsLocallyControlled())
		//{
		//	UE_LOG(LogTemp, Display, L"%.2f EngineTorque", EngineTorque);
		//	UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
		//	UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
		//	UE_LOG(LogTemp, Display, L"%.2f Speed", Speed);
		//}
		//애니메이션에 전달
		SetWheelSpeed(CurrentVelocity * VirtualForwardVal*GetWorld()->DeltaTimeSeconds);
	}
	else
	{
		//애니메이션에 전달
		SetWheelSpeed(10 * value * VirtualForwardVal*GetWorld()->DeltaTimeSeconds);
	}
}

void UCPP_TankPawnMovementComponent::OnTurn(float value)
{
	//가속중이 아니라면 실제와 유사하게 회전방향으로 조금씩 이동하도록 만듬
	if (!FMath::IsNearlyZero(value))
	{
		//후진시 방향을 반전하기 위해서
		if (!IsMoveForward)
			value *= -1;
		//선회 할경우 기어 감소
		RPM -= (100 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm이 낮아지면 기어를 낮춤
			if (EngineGear - 1 >= 0)
				EngineGear--;
		}
		if (RPM <= IdleRPM)
		{
			RPM = IdleRPM;
		}
		IsTurning = true;
	}
	else if (FMath::IsNearlyZero(value))
	{
		IsTurning = false;
	}
	//뒤집히면 못움직이게 함
	float TankYaw = Owner->GetActorRotation().Roll;
	if(TankYaw<100&&TankYaw>-100)
	{	
		NextRotation.Yaw+=value;
	}
	TurnValue = value;
}

void UCPP_TankPawnMovementComponent::OnRep_SightRotation(FRotator value)
{
	SightRotator = value;
}

void UCPP_TankPawnMovementComponent::OnRep_TurretIsMatch(bool value)
{
	IsTurretAngleMatch = value;
}

void UCPP_TankPawnMovementComponent::Server_SetTurretAngle_Implementation(float value)
{
	TurretAngle = value;
}

void UCPP_TankPawnMovementComponent::NetMulticastSetTurretAngle_Implementation(float value)
{
	TurretAngle = value;
}

void UCPP_TankPawnMovementComponent::OnRep_GunIsMatch(bool value)
{
	IsGunAngleMatch = value;
}

void UCPP_TankPawnMovementComponent::Server_SetGunAngle_Implementation(float value)
{
	GunAngle = value;
}

void UCPP_TankPawnMovementComponent::NetMulticastSetGunAngle_Implementation(float value)
{
	GunAngle = value;
}

void UCPP_TankPawnMovementComponent::EngineControl_Implementation()
{
	//속도
	CurrentVelocity = Owner->GetVelocity().Size();//m/s
	CurrentVelocity = (CurrentVelocity * 60) / 1000;//km/h 로 변환

	RPMControl();

	//1.엑셀레이션에 따른 RPM 변화량 적용
	if (IsAccelerating)
	{
		//가속시 RPM증가
		if (MaxRPM >= RPM)
		{
			RPM += (RPMDisplacement * GetWorld()->DeltaTimeSeconds);
		}
		else
		{
			//maxRpm 도달시 기어를 올림
			if (EngineGear + 1 <= MaxEngineGear)
				EngineGear++;
		}

	}
	else
	{
		//가속이 멈출경우 RPM감소
		RPM -= (300 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm이 낮아지면 기어를 낮춤
			if (EngineGear - 1 >= 0)
				EngineGear--;
		}
		if (RPM <= IdleRPM)
		{
			RPM = IdleRPM;
		}
	}

	//2.구한 rpm 값으로 엔진토크 설정
	EngineTorque = EngineTorqueCurve->GetFloatValue(RPM);
	//3.단위 m/s
	//속도는 (rpm*토크)/(임의의 상수 - 엔진 기어)*임의의 상수
	Speed = (RPM * EngineTorque) / ((10 - EngineGear) * 100);
}

void UCPP_TankPawnMovementComponent::RPMControl_Implementation()
{
	//기어에 맞는 rpm 세팅
	switch (EngineGear)
	{
	case 0:
		MinRPM = 200;
		MaxRPM = 250;
		break;
	case 1:
		MinRPM = 200;
		MaxRPM = 600;
		break;
	case 2:
		MinRPM = 550;
		MaxRPM = 700;
		break;
	case 3:
		MinRPM = 650;
		MaxRPM = 800;
		break;
	case 4:
		MinRPM = 750;
		MaxRPM = 900;
		break;
	default:
		break;
	}
}

void UCPP_TankPawnMovementComponent::UpdateTurretState(float DeltaTime)
{
	if(Owner->IsLocallyControlled())
	{
		SightRotator = UKismetMathLibrary::InverseTransformRotation(Cast<ACPP_Tank_Character>(Owner)->GetTankTransform(),Owner->GetControlRotation()).GetDenormalized();
		if(Owner->HasAuthority())
		{
			OnRep_SightRotation(UKismetMathLibrary::InverseTransformRotation(Cast<ACPP_Tank_Character>(Owner)->GetTankTransform(),Owner->GetControlRotation()).GetDenormalized());
		}
	}
	TurretRotator = TankMesh->GetBoneQuaternion(L"turret_jnt",EBoneSpaces::ComponentSpace).Rotator().GetDenormalized();
	if (!FMath::IsNearlyZero(SightRotator.Yaw-TurretRotator.Yaw,0.01f))
	{
		//포탑회전 관련 사운드 함수 호출
		if(IsTurretAngleMatch)
		{
			if(TurretMoveStartFunc.IsBound())
				TurretMoveStartFunc.Execute();			
		}
		if(Owner->IsLocallyControlled())
		{
			//일치 하지 않을 경우
			IsTurretAngleMatch = false;
			if(Owner->HasAuthority())
				OnRep_TurretIsMatch(false);	
		}
	}
	else
	{
		//포탑회전 관련 사운드 함수 호출
		if(!IsTurretAngleMatch)
		{
			if(TurretMoveEndFunc.IsBound())
				TurretMoveEndFunc.Execute();			
		}
		if(Owner->IsLocallyControlled())
		{
			//일치 하는 경우
			IsTurretAngleMatch = true;
			if(Owner->HasAuthority())
				OnRep_TurretIsMatch(true);		
		}
	}
	if(!IsTurretAngleMatch)
		TurretMove(DeltaTime);
	
}

void UCPP_TankPawnMovementComponent::TurretMove(float DeltaTime)
{
	if(IsTurretAngleMatch&&!Owner->IsLocallyControlled())
		return;
	//탱크 포탑 기준  바라보는 곳과 몇도가 차이 나는지 판단용
	FixRotatorDirSize();
	//더 작은 각도 판별
	if(LeftAngle<RightAngle)
	{
		if(LeftAngle>DeltaTime*TurretTurnSpeed)
		{
			TurretAngle = TurretAngle-(DeltaTime*TurretTurnSpeed);
			if(!Owner->HasAuthority())
				Server_SetTurretAngle(TurretAngle);
			else
				NetMulticastSetTurretAngle(TurretAngle);
		}
		else
		{//회전 방향의 남은 각도와 사용자가 바라보는 각도와 보간
			TurretAngle = (FMath::RInterpTo(FRotator(0,TurretAngle,0),SightRotator,DeltaTime,TurretTurnSpeed).Yaw);
			if(!Owner->HasAuthority())
				Server_SetTurretAngle(TurretAngle);
			else
				NetMulticastSetTurretAngle(TurretAngle);
		}
	}
	else if(LeftAngle>RightAngle)
	{
		if(RightAngle>DeltaTime*TurretTurnSpeed)
		{
			TurretAngle=(TurretAngle+(DeltaTime*TurretTurnSpeed));//크기가 작은쪽으로 회전
			if(!Owner->HasAuthority())
				Server_SetTurretAngle(TurretAngle);
			else
				NetMulticastSetTurretAngle(TurretAngle);
		}
		else
		{
			TurretAngle=(FMath::RInterpTo(FRotator(0,TurretAngle,0),SightRotator,DeltaTime,TurretTurnSpeed).Yaw);
			if(!Owner->HasAuthority())
				Server_SetTurretAngle(TurretAngle);
			else
				NetMulticastSetTurretAngle(TurretAngle);
		}
	}
}

void UCPP_TankPawnMovementComponent::UpdateGunState(float DeltaTime)
{
	if(Owner->IsLocallyControlled())
	{
		GunRotator = TankMesh->GetBoneQuaternion(L"gun_jnt").Rotator().Quaternion().Rotator();
		if(!FMath::IsNearlyEqual(SightRotator.Pitch, GunRotator.Pitch,0.01f))
		{
			//일치 하지 않을 경우
			IsGunAngleMatch = false;
			if(Owner->HasAuthority())
				OnRep_GunIsMatch(false);
			IsSightUpZero = SightRotator.Pitch>=0?true:false;
		}
		else
		{
			IsGunAngleMatch = true;
			if(Owner->HasAuthority())
				OnRep_GunIsMatch(true);
		}
		GunMove(DeltaTime);
	}
}

void UCPP_TankPawnMovementComponent::GunMove(float DeltaTime)
{
	if(IsGunAngleMatch)//매치하면 반환
		return;
	SightRotator = SightRotator.GetEquivalentRotator();
	//시선을 등판각을 합쳐서 tankmesh의 gun rotator에 맞게 변경
	float targetPitch = SightRotator.Quaternion().Rotator().Pitch-Cast<ACPP_Tank_Character>(Owner)->GetGunAngleOffset();
	//gun본의 rotator를 받아와옴
	float GunRotationPitch = TankMesh->GetBoneQuaternion(L"gun_jnt",EBoneSpaces::ComponentSpace).Rotator().Pitch;
	if(GunRotationPitch>targetPitch)
	{//목표로하는 각도가 gun의 각도보다 작은 경우
		GunRotationPitch = FMath::Clamp(GunRotationPitch-(GunMoveSpeed*DeltaTime),targetPitch,GunRotationPitch);
	}
	else if(GunRotationPitch<targetPitch)
	{//목표로하는 각도가 gun의 각도보다 큰 경우
		GunRotationPitch =FMath::Clamp(GunRotationPitch+(GunMoveSpeed*DeltaTime),GunRotationPitch,targetPitch);
	}
	//마지막으로 animinst에서 접근하는 변수에 넣어준다.
	GunAngle = GunRotationPitch;
	if(Owner->HasAuthority())
		NetMulticastSetGunAngle(GunRotationPitch);
	else
		Server_SetGunAngle(GunRotationPitch);
}

void UCPP_TankPawnMovementComponent::OnEngineBreak_Implementation()
{
	isBreak=true;
	VirtualFriction = 0.02f;
}

void UCPP_TankPawnMovementComponent::OffEngineBreak_Implementation()
{
	isBreak = false;
	VirtualFriction = 0.005f;
}


