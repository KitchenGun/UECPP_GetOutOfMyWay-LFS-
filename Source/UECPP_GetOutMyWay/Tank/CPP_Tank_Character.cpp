#include "Tank/CPP_Tank_Character.h"

#include "Camera/CameraComponent.h"
#include "Component/CPP_MainGunSystemComponent.h"
#include "Component/CPP_ParticleControlComponent.h"
#include "Component/CPP_TankPawnMovementComponent.h"
#include "Component/CPP_TrackMovementComponent.h"
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
	GetMesh()->bIsAutonomousTickPose =true;
	PrimaryActorTick.bCanEverTick = true;

	//actorcomp
	GetCharacterMovement()->SetIsReplicated(true);
	TankMovement = CreateDefaultSubobject<UCPP_TankPawnMovementComponent>(L"TankPawnMovement");
	TankMovement->SetIsReplicated(true);
	TrackMovement = CreateDefaultSubobject<UCPP_TrackMovementComponent>(L"TrackMovement");
	TrackMovement->SetIsReplicated(true);
	ParticleSystem = CreateDefaultSubobject<UCPP_ParticleControlComponent>(L"ParticleController");
	ParticleSystem->SetIsReplicated(true);
}

void ACPP_Tank_Character::GunDirPosWorldToScreen()
{
	if(!IsLocallyControlled())
		return;
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
		PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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
		GunSystem->FGunReloadDoneFunc.BindUFunction(this,"GunSystemSoundReloadDone");
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
	float pitch = GetControlRotation().Quaternion().Rotator().Pitch;
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
	static_cast<ECameraType>((uint8)CamType + 1) == ECameraType::MAX
		? CamType = static_cast<ECameraType>((uint8)0)
		: CamType = static_cast<ECameraType>((uint8)CamType + 1);

	if (FpViewToggleFunc.IsBound())
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
		OnWheelParticle();
		if(!HasAuthority())
			Server_OnMoveForward(value);
	}
}

void ACPP_Tank_Character::Server_OnMoveForward_Implementation(float value)
{
	TankMovement->OnMove(value);
	OnWheelParticle();
}

void ACPP_Tank_Character::OnMoveTurn(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnTurn(value);
		OnWheelParticle();
		if(!HasAuthority())
			Server_OnMoveTurn(value);
	}
}

void ACPP_Tank_Character::Server_OnMoveTurn_Implementation(float value)
{
	TankMovement->OnTurn(value);
	OnWheelParticle();
}

void ACPP_Tank_Character::OnEngineBreak()
{
	TankMovement->OnEngineBreak();
	if(!HasAuthority())
		Server_OnEngineBreak();
}

void ACPP_Tank_Character::Server_OnEngineBreak_Implementation()
{
	TankMovement->OnEngineBreak();
}

void ACPP_Tank_Character::OffEngineBreak()
{
	TankMovement->OffEngineBreak();
	if(!HasAuthority())
		Server_OffEngineBreak();
}

void ACPP_Tank_Character::Server_OffEngineBreak_Implementation()
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

void ACPP_Tank_Character::OnWheelParticle()
{
	if(TankMovement->GetIsMove())
	{
		ParticleSystem->SetIsMove(true);
		EngineSoundPlay();
		if(!HasAuthority())
			Server_EngineSoundPlay();
		IsMoveBefore =true;
	}
	else if(IsMoveBefore)
	{
		ParticleSystem->SetIsMove(false);
		EngineSoundStop();
		if(!HasAuthority())
			Server_EngineSoundStop();
		IsMoveBefore = false;
	}

	if(!HasAuthority())
		Server_OnWheelParticle();
}

void ACPP_Tank_Character::Server_OnWheelParticle_Implementation()
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

void ACPP_Tank_Character::OnFireParticle()
{
	ParticleSystem->OnFireParticle();
	GunSystemSoundPlay();
	if(!HasAuthority())
		Server_OnFireParticle();
}

void ACPP_Tank_Character::Server_OnFireParticle_Implementation()
{
	ParticleSystem->OnFireParticle();
	GunSystemSoundPlay();
}

void ACPP_Tank_Character::IdleSoundPlay()
{
	IdleAudio->SetSound(IdleLoopSound);
	IdleAudio->Play();

	if(!HasAuthority())
		Server_IdleSoundPlay();

}

void ACPP_Tank_Character::Server_IdleSoundPlay_Implementation()
{
	IdleAudio->SetSound(IdleLoopSound);
	IdleAudio->Play();
	FString temp = HasAuthority()?L"Server : ":L"Client : ";
	temp.Append(IdleAudio->Sound->GetName());
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,temp);
}

void ACPP_Tank_Character::EngineSoundPlay()
{
	if(!IsMoveBefore&&TankMovement->GetIsMove())
	{
		EngineAudio->SetSound(EngineStartSound);
		EngineAudio->Play();
		IsEngineEnd=false;
	}
	if(!HasAuthority())
		Server_EngineSoundPlay();
}

void ACPP_Tank_Character:: Server_EngineSoundPlay_Implementation()
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

	if(!HasAuthority())
		Server_EngineSoundStop();
}

void ACPP_Tank_Character::Server_EngineSoundStop_Implementation()
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
	if(IsLocallyControlled())//자신의 사운드를 들을 경우
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
	else//다른 탱크에서 사운드를 들을 경우
	{
		GunSystemAudio->SetSound(MainGunFireSound[0]);
		GunSystemAudio->Play();
	}

	if(!HasAuthority())
		Server_GunSystemSoundPlay();
}

void ACPP_Tank_Character::Server_GunSystemSoundPlay_Implementation()
{
	GunSystemAudio->SetSound(MainGunFireSound[0]);
	GunSystemAudio->Play();
}

void ACPP_Tank_Character::GunSystemSoundStop()
{
	if (!GunSystem->GetIsMainGunCanFire())
	{
		GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
		GunSystemAudio->Sound = MainGunReloadDoneSound;
		GunSystemAudio->Play();
		
		if(HasAuthority())
			Server_GunSystemSoundStop();
	}
}

void ACPP_Tank_Character::Server_GunSystemSoundStop_Implementation()
{
	GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	GunSystemAudio->Sound = MainGunReloadDoneSound;
	GunSystemAudio->Play();
}

void ACPP_Tank_Character::GunSystemSoundReloadDone()
{
	GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	GunSystemAudio->SetSound(MainGunReloadSound);
	GunSystemAudio->Play();
	if(!HasAuthority())
		Server_GunSystemSoundReloadDone();
}

void ACPP_Tank_Character::Server_GunSystemSoundReloadDone_Implementation()
{
	GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	GunSystemAudio->SetSound(MainGunReloadSound);
	GunSystemAudio->Play();
}

void ACPP_Tank_Character::TurretMoveLoop()
{
	TurretSystemAudio->SetSound(TurretLoopSound);
	TurretSystemAudio->Play();
	if(!HasAuthority())
		Server_TurretMoveLoop();
}

void ACPP_Tank_Character::Server_TurretMoveLoop_Implementation()
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
		if(!HasAuthority())
			Server_TurretMoveEnd();
	}
}

void ACPP_Tank_Character::Server_TurretMoveEnd_Implementation()
{
	TurretSystemAudio->SetSound(TurretEndSound);
	TurretSystemAudio->Play();
}

void ACPP_Tank_Character::Server_SetHP_Implementation(float value)
{
	HP = value;
}

void ACPP_Tank_Character::Client_SetHP_Implementation(float value)
{
	HP = value;
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
	
	if(IsLocallyControlled())
	{//차체가 바라보는 방향 획득해서 inverse용 transform 저장
		TankTransform = GetMesh()->GetComponentTransform();
		if(HasAuthority())
			OnRep_UpdateTankTransform(GetMesh()->GetComponentTransform()); 
	}
	
	//ui에 전달
	GunDirPosWorldToScreen();
}

void ACPP_Tank_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
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
	if(!HasAuthority())
		Server_SetHP(HP-DamageAmount);
	
	if(FSetHPFunc.IsBound()&&IsLocallyControlled())
		FSetHPFunc.Execute(HP);
	
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
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACPP_Tank_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPP_Tank_Character,TankTransform);

	DOREPLIFETIME(ACPP_Tank_Character,IdleAudio);
	DOREPLIFETIME(ACPP_Tank_Character,EngineAudio);
	DOREPLIFETIME(ACPP_Tank_Character,GunSystemAudio);
	DOREPLIFETIME(ACPP_Tank_Character,TurretSystemAudio);
	DOREPLIFETIME(ACPP_Tank_Character,HitAudio);
	DOREPLIFETIME(ACPP_Tank_Character,HP);
}

void ACPP_Tank_Character::OnRep_UpdateTankTransform(FTransform value)
{
	TankTransform = value;
}

