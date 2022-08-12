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
		//������ ���ݾ� ������ �̵��ϵ��� ����
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
		//�� �°� RPM ����
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

	//���ǰ��� ���� �ӵ� ����
	float TankClimbingAnglePercentage = 0.0f;
	if (TankClimbingAngle > 0)
	{//�ö󰡴� ��Ȳ
		TankClimbingAnglePercentage = TankClimbingAngle / 30;
	}
	else if (TankClimbingAngle < 0)
	{//�������� ��Ȳ
		TankClimbingAnglePercentage = TankClimbingAngle / 30;
	}
	else
	{
		TankClimbingAnglePercentage = 0;
	}

	//�������� �������̰� ��
	float TankYaw = Owner->GetActorRotation().Roll;
	if (TankYaw<70 && TankYaw>-70)
	{//���� ������ ����
		NextLocation += (dir * (VirtualForwardVal - TankClimbingAnglePercentage));
		InputVal = VirtualForwardVal - TankClimbingAnglePercentage;
		CurrentVelocity = (Speed*3.14f);//������ ȸ���ӵ��� �°� ����
		/*��¿�*/
		//if(Owner->IsLocallyControlled())
		//{
		//	UE_LOG(LogTemp, Display, L"%.2f EngineTorque", EngineTorque);
		//	UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
		//	UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
		//	UE_LOG(LogTemp, Display, L"%.2f Speed", Speed);
		//}
		//�ִϸ��̼ǿ� ����
		SetWheelSpeed(CurrentVelocity * VirtualForwardVal*GetWorld()->DeltaTimeSeconds);
	}
	else
	{
		//�ִϸ��̼ǿ� ����
		SetWheelSpeed(10 * value * VirtualForwardVal*GetWorld()->DeltaTimeSeconds);
	}
}

void UCPP_TankPawnMovementComponent::OnTurn(float value)
{
	//�������� �ƴ϶�� ������ �����ϰ� ȸ���������� ���ݾ� �̵��ϵ��� ����
	if (!FMath::IsNearlyZero(value))
	{
		//������ ������ �����ϱ� ���ؼ�
		if (!IsMoveForward)
			value *= -1;
		//��ȸ �Ұ�� ��� ����
		RPM -= (100 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm�� �������� �� ����
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
	//�������� �������̰� ��
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
	//�ӵ�
	CurrentVelocity = Owner->GetVelocity().Size();//m/s
	CurrentVelocity = (CurrentVelocity * 60) / 1000;//km/h �� ��ȯ

	RPMControl();

	//1.�������̼ǿ� ���� RPM ��ȭ�� ����
	if (IsAccelerating)
	{
		//���ӽ� RPM����
		if (MaxRPM >= RPM)
		{
			RPM += (RPMDisplacement * GetWorld()->DeltaTimeSeconds);
		}
		else
		{
			//maxRpm ���޽� �� �ø�
			if (EngineGear + 1 <= MaxEngineGear)
				EngineGear++;
		}

	}
	else
	{
		//������ ������ RPM����
		RPM -= (300 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm�� �������� �� ����
			if (EngineGear - 1 >= 0)
				EngineGear--;
		}
		if (RPM <= IdleRPM)
		{
			RPM = IdleRPM;
		}
	}

	//2.���� rpm ������ ������ũ ����
	EngineTorque = EngineTorqueCurve->GetFloatValue(RPM);
	//3.���� m/s
	//�ӵ��� (rpm*��ũ)/(������ ��� - ���� ���)*������ ���
	Speed = (RPM * EngineTorque) / ((10 - EngineGear) * 100);
}

void UCPP_TankPawnMovementComponent::RPMControl_Implementation()
{
	//�� �´� rpm ����
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
		//��žȸ�� ���� ���� �Լ� ȣ��
		if(IsTurretAngleMatch)
		{
			if(TurretMoveStartFunc.IsBound())
				TurretMoveStartFunc.Execute();			
		}
		if(Owner->IsLocallyControlled())
		{
			//��ġ ���� ���� ���
			IsTurretAngleMatch = false;
			if(Owner->HasAuthority())
				OnRep_TurretIsMatch(false);	
		}
	}
	else
	{
		//��žȸ�� ���� ���� �Լ� ȣ��
		if(!IsTurretAngleMatch)
		{
			if(TurretMoveEndFunc.IsBound())
				TurretMoveEndFunc.Execute();			
		}
		if(Owner->IsLocallyControlled())
		{
			//��ġ �ϴ� ���
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
	//��ũ ��ž ����  �ٶ󺸴� ���� ��� ���� ������ �Ǵܿ�
	FixRotatorDirSize();
	//�� ���� ���� �Ǻ�
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
		{//ȸ�� ������ ���� ������ ����ڰ� �ٶ󺸴� ������ ����
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
			TurretAngle=(TurretAngle+(DeltaTime*TurretTurnSpeed));//ũ�Ⱑ ���������� ȸ��
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
			//��ġ ���� ���� ���
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
	if(IsGunAngleMatch)//��ġ�ϸ� ��ȯ
		return;
	SightRotator = SightRotator.GetEquivalentRotator();
	//�ü��� ���ǰ��� ���ļ� tankmesh�� gun rotator�� �°� ����
	float targetPitch = SightRotator.Quaternion().Rotator().Pitch-Cast<ACPP_Tank_Character>(Owner)->GetGunAngleOffset();
	//gun���� rotator�� �޾ƿͿ�
	float GunRotationPitch = TankMesh->GetBoneQuaternion(L"gun_jnt",EBoneSpaces::ComponentSpace).Rotator().Pitch;
	if(GunRotationPitch>targetPitch)
	{//��ǥ���ϴ� ������ gun�� �������� ���� ���
		GunRotationPitch = FMath::Clamp(GunRotationPitch-(GunMoveSpeed*DeltaTime),targetPitch,GunRotationPitch);
	}
	else if(GunRotationPitch<targetPitch)
	{//��ǥ���ϴ� ������ gun�� �������� ū ���
		GunRotationPitch =FMath::Clamp(GunRotationPitch+(GunMoveSpeed*DeltaTime),GunRotationPitch,targetPitch);
	}
	//���������� animinst���� �����ϴ� ������ �־��ش�.
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


