#include "Tank/CPP_Tank_Character.h"

#include "Camera/CameraComponent.h"
#include "Component/CPP_MainGunSystemComponent.h"
#include "Component/CPP_ParticleControlComponent.h"
#include "Component/CPP_TankPawnMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACPP_Tank_Character::ACPP_Tank_Character()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ACPP_Tank_Character::GunDirPosWorldToScreen()
{
	FVector start = GetMesh()->GetSocketLocation("gun_1_jntSocket");
	FRotator Dir = GetMesh()->GetSocketRotation("gun_1_jntSocket");
	FVector end = start+Dir.Vector()*5e+4f;
	FHitResult hit;
	TArray<AActor*> ignore;
	ignore.Add(this);
	const bool isHit =
			UKismetSystemLibrary::LineTraceSingle(GetWorld(),start,end,
				ETraceTypeQuery::TraceTypeQuery4,false,ignore,EDrawDebugTrace::None,hit,true);
	
	FVector2D pos = FVector2D{0,0};
	if(isHit)
	{
		UGameplayStatics::ProjectWorldToScreen(PC,hit.ImpactPoint,pos,true);
		if(FGunSightPosFunc.IsBound())
			FGunSightPosFunc.Execute(pos);
		if(FSetRangeTextFunc.IsBound())
			FSetRangeTextFunc.Execute(static_cast<int>(FVector::Dist(start,hit.ImpactPoint)/80.0f));
	}
	else
	{
		UGameplayStatics::ProjectWorldToScreen(PC,end,pos);
		if(FGunSightPosFunc.IsBound())
			FGunSightPosFunc.Execute(pos);
		if(FSetRangeTextFunc.IsBound())
			FSetRangeTextFunc.Execute(0);
	}
}

void ACPP_Tank_Character::BeginPlay()
{
	Super::BeginPlay();
	if(PC==nullptr)
	{
		PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	}
	//바인딩
	if(IsValid(TankMovement))
	{
		TankMovement->TurretMoveStartFunc.BindUFunction(this,"TurretMoveLoop");
		TankMovement->TurretMoveEndFunc.BindUFunction(this,"TurretMoveEnd");
	}	
	if(IsValid(GunSystem))
	{
		GunSystem->FireEffectFunc.BindUFunction(this,"OnFireParticle");
		GunSystem->GunReloadDoneFunc.BindUFunction(this,"GunSystemSoundReloadDone");
	}
	GetCharacterMovement()->MaxWalkSpeed = 400;
}

void ACPP_Tank_Character::OnVerticalLook(float value)
{
	AddControllerPitchInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);
}

void ACPP_Tank_Character::OnHorizontalLook(float value)
{
	AddControllerYawInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);
}

void ACPP_Tank_Character::CamPitchLimitSmooth()
{
	float pitch = PC->GetControlRotation().Quaternion().Rotator().Pitch;
	float minAngle = PitchLimitMin;
	float maxAngle = PitchLimitMax;
	//탱크의 pitch를 구해서 등판각을 받음
	if(!FMath::IsNearlyZero(Turret->GetComponentRotation().Pitch,0.1f))
	{
		displacementAngle = FRotator(GunnerSpringArm->GetComponentRotation().Quaternion()).Pitch;
		minAngle = PitchLimitMin+displacementAngle;
		maxAngle = PitchLimitMax+displacementAngle;
	}
	else
	{
		displacementAngle =0;
	}
	pitch = FMath::Clamp(pitch, minAngle, maxAngle);
	
	FRotator temp = FRotator(pitch, PC->GetControlRotation().Quaternion().Rotator().Yaw, PC->GetControlRotation().Quaternion().Rotator().Roll);
	PC->SetControlRotation(temp);
}

void ACPP_Tank_Character::CamChange()
{
	static_cast<ECameraType>((uint8)CamType+1)==ECameraType::MAX
	?CamType=static_cast<ECameraType>((uint8)0)
	:CamType=static_cast<ECameraType>((uint8)CamType+1);

	if(FpViewToggleFunc.IsBound())
		FpViewToggleFunc.Execute();
	switch (CamType)
	{
	case ECameraType::THIRD:
		Camera->SetActive(true);
		GunnerCam->SetFieldOfView(90);
		GunnerCam->SetActive(false);
		break;
	case ECameraType::GUNNER:
		GunnerCam->SetActive(true);
		Camera->SetActive(false);
		break;
	default:
		break;
	}
}

void ACPP_Tank_Character::OnMoveForward(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnMove(value);
		OnWheelParticle_Implementation();
	}
}

void ACPP_Tank_Character::OnMoveTurn(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnTurn(value);
		OnWheelParticle();
	}
}

void ACPP_Tank_Character::OnEngineBreak()
{
	TankMovement->OnEngineBreak();
}

void ACPP_Tank_Character::OffEngineBreak()
{
	TankMovement->OffEngineBreak();
}

void ACPP_Tank_Character::OnMainGunFire()
{
	if(FireFunc.IsBound())
		FireFunc.Execute();
}

void ACPP_Tank_Character::ZoomToggle()
{
	if(CamType == ECameraType::GUNNER)
	{
		if(IsZoom)
		{
			IsZoom=false;
			GunnerCam->SetFieldOfView(90);
			
			if(FZoomToggleFunc.IsBound())
				FZoomToggleFunc.Execute();
		}
		else
		{
			IsZoom=true;
			GunnerCam->SetFieldOfView(45);
			
			if(FZoomToggleFunc.IsBound())
				FZoomToggleFunc.Execute();
		}
	}
}

void ACPP_Tank_Character::OnWheelParticle_Implementation()
{
	if (IsLocallyControlled())
	{
		if(TankMovement->GetIsMove())
		{
			ParticleSystem->SetIsMove(true);
			EngineSoundPlay();
			IsMoveBefore =true;
		}
		else if(IsMoveBefore)
		{
			ParticleSystem->SetIsMove(false);
			EngineSoundStop();
			IsMoveBefore = false;
		}
	}
	if (GetLocalRole() == ROLE_Authority)
	{
		if(TankMovement->GetIsMove())
		{
			ParticleSystem->SetIsMove(true);
			ParticleSystem->OnRepWheelParticle();
			EngineSoundPlay();
			IsMoveBefore =true;
		}
		else if(IsMoveBefore)
		{
			ParticleSystem->SetIsMove(false);
			ParticleSystem->OnRepWheelParticle();
			EngineSoundStop();
			IsMoveBefore = false;
		}
	}
}

void ACPP_Tank_Character::OnFireParticle()
{
	ParticleSystem->OnFireParticle();
	GunSystemSoundPlay();
}

void ACPP_Tank_Character::IdleSoundPlay()
{
	IdleAudio->SetSound(IdleLoopSound);
	IdleAudio->Play();
}

void ACPP_Tank_Character::EngineSoundPlay()
{
	if(!IsMoveBefore&&TankMovement->GetIsMove())
	{
		EngineAudio->SetSound(EngineStartSound);
		EngineAudio->Play();
		IsEngineEnd=false;
	}
}

void ACPP_Tank_Character::EngineSoundStop()
{
	if(!TankMovement->GetIsMove())
	{
		EngineAudio->Sound = EngineEndSound;
		if(!IsEngineEnd)
		{
			EngineAudio->Stop();
			EngineAudio->Play();
		}
		IsEngineEnd = true;
	}
	else if(IsMoveBefore&&TankMovement->GetIsMove())
	{
		EngineAudio->Sound = EngineLoopSound;
		EngineAudio->Play();
	}
}

void ACPP_Tank_Character::GunSystemSoundPlay()
{
	GunSystemAudio->AttenuationSettings = MainGunSoundAttenuation;
	if(CamType==ECameraType::THIRD)
	{
		GunSystemAudio->SetSound(MainGunFireSound[0]);
		GunSystemAudio->Play();
	}
	else if(CamType==ECameraType::GUNNER)
	{
		GunSystemAudio->SetSound(MainGunFireSound[1]);
		GunSystemAudio->Play();
	}
}

void ACPP_Tank_Character::GunSystemSoundStop()
{
	if (!GunSystem->GetIsMainGunCanFire())
	{
		GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
		GunSystemAudio->Sound = MainGunReloadDoneSound;
		GunSystemAudio->Play();
	}
}

void ACPP_Tank_Character::GunSystemSoundReloadDone()
{
	GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	GunSystemAudio->SetSound(MainGunReloadSound);
	GunSystemAudio->Play();
}

void ACPP_Tank_Character::TurretMoveLoop()
{
	TurretSystemAudio->SetSound(TurretLoopSound);
	TurretSystemAudio->Play();
}

void ACPP_Tank_Character::TurretMoveEnd()
{
	if (TurretSystemAudio->Sound == TurretLoopSound)
	{
		TurretSystemAudio->SetSound(TurretEndSound);
		TurretSystemAudio->Play();
	}
}

void ACPP_Tank_Character::Dead()
{
	IsDead =true;
}

void ACPP_Tank_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Turret Collider 위치 변환 탱크 포탑에 충돌체 같이 움직이기 위해서
	Turret->SetWorldLocation(GetMesh()->GetSocketLocation("turret_jntSocket"));
	Turret->SetWorldRotation(GetMesh()->GetSocketRotation("turret_jntSocket"));
	//ui에 전달
	GunDirPosWorldToScreen();
}

void ACPP_Tank_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPP_Tank_Character::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPP_Tank_Character::OnHorizontalLook);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPP_Tank_Character::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveTurn", this, &ACPP_Tank_Character::OnMoveTurn);
	
	PlayerInputComponent->BindAction("EngineBreak",IE_Pressed,this, &ACPP_Tank_Character::OnEngineBreak);
	PlayerInputComponent->BindAction("EngineBreak", IE_Released, this, &ACPP_Tank_Character::OffEngineBreak);
	PlayerInputComponent->BindAction("ViewChange",IE_Pressed,this,&ACPP_Tank_Character::CamChange);
	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&ACPP_Tank_Character::OnMainGunFire);
	PlayerInputComponent->BindAction("Zoom",IE_Pressed,this, &ACPP_Tank_Character::ZoomToggle);
}

float ACPP_Tank_Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HP-=DamageAmount;

	if(!IsDead&&HP<=0)
	{
		Dead();
	}
	else
	{
		if(DamageAmount>=20)
		{
			HitAudio->Sound = TankHitSound;
			HitAudio->Play();
		}
		else
		{
			HitAudio->Sound = TankRicochetHitSound;
			HitAudio->Play();
		}
	}
	UE_LOG(LogTemp,Display,L"%.2f",HP);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACPP_Tank_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_Tank_Character,TrackMovement);
	DOREPLIFETIME(ACPP_Tank_Character,TankMovement);
	DOREPLIFETIME(ACPP_Tank_Character,GunSystem);
	DOREPLIFETIME(ACPP_Tank_Character,ParticleSystem);
	
	DOREPLIFETIME(ACPP_Tank_Character,MuzzleFlashEffect);
	DOREPLIFETIME(ACPP_Tank_Character,ShockWaveEffect);
	DOREPLIFETIME(ACPP_Tank_Character,WheelsEffect);
	DOREPLIFETIME(ACPP_Tank_Character,HP);
}

