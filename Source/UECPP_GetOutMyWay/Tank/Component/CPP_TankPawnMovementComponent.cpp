#include "Tank/Component/CPP_TankPawnMovementComponent.h"
#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Math/Vector.h"
#include "Tank/CPP_Tank_Pawn.h"

UCPP_TankPawnMovementComponent::UCPP_TankPawnMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Owner = Cast<APawn>(GetOwner());

	ConstructorHelpers::FObjectFinder<UCurveFloat> Curvefloat(L"CurveFloat'/Game/Data/Tank/Curve/FCurv_EngineTorque.FCurv_EngineTorque'");
	EngineTorqueCurve = Curvefloat.Object;


	if(Owner != nullptr)
		TankMesh = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (TankMesh != nullptr)
	{
		TankAnimInst = Cast<UCPP_TankAnimInstance>(TankMesh->GetAnimInstance());
	}
}

void UCPP_TankPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCPP_TankPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Engine
	EngineControl();
	Movement(DeltaTime);
	//Turret
	UpdateTurretState(DeltaTime);
	//Gun
	UpdateGunState(DeltaTime);
}

void UCPP_TankPawnMovementComponent::SetWheelSpeed(float WheelSpeed)
{
	if(IsAccelerating)
	{
		TrackSpeed = WheelSpeed*0.1f;
	}
	else
	{
		TrackSpeed = TurnValue*2.0f;
	}
}

void UCPP_TankPawnMovementComponent::Movement(float DeltaTime)
{
	if (Owner != nullptr && !NextLocation.IsNearlyZero())
	{
		NextLocation = GetActorLocation() + (NextLocation * DeltaTime * Speed);
		Owner->SetActorRelativeLocation(NextLocation);
	}
	if (Owner != nullptr && !NextRotation.IsNearlyZero())
	{
		NextRotation = Owner->GetActorRotation() + (NextRotation * DeltaTime * TurnSpeed);
		//������ ���ݾ� ������ �̵��ϵ��� ����
		if (!IsAccelerating)
		{
			NextLocation = GetActorLocation() + (Owner->GetActorForwardVector() * DeltaTime * 8.0f);
			Owner->SetActorRelativeLocation(NextLocation);
		}
		Owner->SetActorRelativeRotation(NextRotation);
		TurretAngleOffSet=Owner->GetActorRotation().Quaternion().Rotator().Yaw;
	}
	NextLocation = FVector::ZeroVector;
	NextRotation = FRotator::ZeroRotator;
}

void UCPP_TankPawnMovementComponent::OnMove(float value)
{
	TankClimbingAngle = Owner->GetActorRotation().Pitch;
	FVector dir = Owner->GetActorForwardVector();

	if (value > 0&&!isBreak)
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
		if(!FMath::IsNearlyEqual(VirtualForwardVal,1))
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal+0.01f,0.0f,1.0f);
	}
	else if (FMath::IsNearlyZero(value)||isBreak)
	{
		IsAccelerating = false;
		IsMoveForward = true;

		if(VirtualForwardVal>0)
		{
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal-VirtualFriction,0.0f,1.0f);
		}
		else if(VirtualForwardVal<0)
		{
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal+VirtualFriction,-1.0f,0.0f);
		}
	}
	else if(!isBreak)
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
		if(!FMath::IsNearlyEqual(VirtualForwardVal,-1))
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal-0.01f,-1.0f,0.0f);
	}

	//���ǰ��� ���� �ӵ� ����
	float TankClimbingAnglePercentage = 0.0f;
	if(TankClimbingAngle>0)
	{//�ö󰡴� ��Ȳ
		TankClimbingAnglePercentage=TankClimbingAngle/60;
	}
	else if(TankClimbingAngle<0)
	{//�������� ��Ȳ
		TankClimbingAnglePercentage=TankClimbingAngle/60;
	}
	else
	{
		TankClimbingAnglePercentage=0;
	}
	NextLocation+=(dir*(VirtualForwardVal-TankClimbingAnglePercentage));
	CurrentVelocity=(NextLocation*Speed*0.036f).Size();
	//�ִϸ��̼ǿ� ����
	SetWheelSpeed(CurrentVelocity*VirtualForwardVal);
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

	NextRotation.Yaw+=value;
	TurnValue = value;
}

void UCPP_TankPawnMovementComponent::EngineControl()
{
	//�ӵ�
	CurrentVelocity = Owner->GetVelocity().Size();//m/s
	CurrentVelocity = (CurrentVelocity * 60) / 1000;//km/h �� ��ȯ

	RPMControl();

	//�������̼ǿ� ���� RPM ��ȭ�� ����
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

	//���� rpm ������ ������ũ ����
	EngineTorque = EngineTorqueCurve->GetFloatValue(RPM);
	//���� m/s
	Speed = (RPM * EngineTorque) / ((10 - EngineGear) * 100);

	/*��¿�*/
	//UE_LOG(LogTemp, Display, L"%.2f MaxWalkSpeed", GetCharacterMovement()->MaxWalkSpeed);//max 1250
	//UE_LOG(LogTemp, Display, L"%.2f km/h", CurrentVelocity);								//max 75
	//UE_LOG(LogTemp, Display, L"%.2f EngineTorque", EngineTorque);
	//UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
	//UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
}

void UCPP_TankPawnMovementComponent::RPMControl()
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
	SightRotator = Owner->GetController()->GetControlRotation().Quaternion().Rotator()+FRotator(0,180,0);
	TurretRotator = TankMesh->GetBoneQuaternion(L"turret_jnt").Rotator()+FRotator(0,180,0);
	if (!FMath::IsNearlyZero(SightRotator.Yaw-TurretRotator.Yaw,0.1f))
	{
		//��žȸ�� ���� ���� �Լ� ȣ��
		if(IsTurretAngleMatch)
		{
			if(TurretMoveStartFunc.IsBound())
				TurretMoveStartFunc.Execute();			
		}
		
		//��ġ ���� ���� ���
		IsTurretAngleMatch = false;
	}
	else
	{
		//��žȸ�� ���� ���� �Լ� ȣ��
		if(!IsTurretAngleMatch)
		{
			if(TurretMoveEndFunc.IsBound())
				TurretMoveEndFunc.Execute();			
		}
		IsTurretAngleMatch = true;
	}
	if(!IsTurretAngleMatch)
		TurretMove(DeltaTime);
}

void UCPP_TankPawnMovementComponent::TurretMove(float DeltaTime)
{
	if(IsTurretAngleMatch)
		return;
	//��ũ ��ž ����  �ٶ󺸴� ���� ��� ���� ������ �Ǵܿ�
	FixRotatorDirSize();
	//�� ���� ���� �Ǻ�
	if(LeftAngle<RightAngle)
	{
		if(LeftAngle>DeltaTime*TurretTurnSpeed)
		{
			TurretAngle = TurretAngle-(DeltaTime*TurretTurnSpeed);//ũ�Ⱑ ���������� ȸ��
		}
		else
		{//ȸ�� ������ ���� ������ ����ڰ� �ٶ󺸴� ������ ����
			FRotator temp = FMath::RInterpTo(FRotator(0,TurretAngle,0),
				FRotator(0,SightRotator.Yaw,0)-FRotator(0,180,0),DeltaTime,TurretTurnSpeed);
			
			TurretAngle=temp.Yaw;
		}
	}
	else if(LeftAngle>RightAngle)
	{
		if(RightAngle>DeltaTime*TurretTurnSpeed)
		{
			TurretAngle = TurretAngle+(DeltaTime*TurretTurnSpeed);//ũ�Ⱑ ���������� ȸ��
		}
		else
		{
			//ȸ�� ������ ���� ������ ����ڰ� �ٶ󺸴� ������ ����
			FRotator temp = FMath::RInterpTo(FRotator(0,TurretAngle,0),
				FRotator(0,SightRotator.Yaw,0)-FRotator(0,180,0),DeltaTime,TurretTurnSpeed);
			TurretAngle=temp.Yaw;
		}
	}
}

void UCPP_TankPawnMovementComponent::UpdateGunState(float DeltaTime)
{
	SightRotator = Owner->GetController()->GetControlRotation().Quaternion().Rotator();
	GunRotator = TankMesh->GetBoneQuaternion(L"gun_jnt").Rotator().Quaternion().Rotator();
	if(!FMath::IsNearlyEqual(SightRotator.Pitch, GunRotator.Pitch,0.01f))
	{
		//��ġ ���� ���� ���
		IsGunAngleMatch = false;
		IsSightUpZero = SightRotator.Pitch>=0?true:false;
	}
	else
	{
		IsGunAngleMatch = true;
	}
	GunMove(DeltaTime);
}

void UCPP_TankPawnMovementComponent::GunMove(float DeltaTime)
{
	if(IsGunAngleMatch)//��ġ�ϸ� ��ȯ
		return;
	//�ü��� ���ǰ��� ���ļ� tankmesh�� gun rotator�� �°� ����
	float targetPitch = SightRotator.Pitch-Cast<ACPP_Tank_Pawn>(Owner)->GetGunAngleOffset();
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
}

void UCPP_TankPawnMovementComponent::OnEngineBreak()
{
	isBreak=true;
	VirtualFriction = 0.02f;
}

void UCPP_TankPawnMovementComponent::OffEngineBreak()
{
	isBreak = false;
	VirtualFriction = 0.01f;
}


