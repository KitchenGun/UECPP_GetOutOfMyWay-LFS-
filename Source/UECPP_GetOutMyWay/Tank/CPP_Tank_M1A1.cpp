#include "Tank/CPP_Tank_M1A1.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Component/CPP_M1A1MainGunSystemComponent.h"
#include "Component/CPP_ParticleControlComponent.h"
#include "Component/CPP_TankUIComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/AssetManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


ACPP_Tank_M1A1::ACPP_Tank_M1A1()
{
	//parameter
	ParameterSet();
	//mesh
	RootSet();
	//collider
	CollisionSet();
	//camera
	CameraSet();
	//particle
	ParticleSet();
	//sound
	SoundSet();

	GunSystem = CreateDefaultSubobject<UCPP_M1A1MainGunSystemComponent>(L"GunSystem");
	GunSystem->SetIsReplicated(true);
	TankUI = CreateDefaultSubobject<UCPP_TankUIComponent>(L"TankUI");
}

void ACPP_Tank_M1A1::BeginPlay()
{
	Super::BeginPlay();
	//카메라
	Camera->SetActive(true);
	GunnerCam->SetActive(false);
	bUseControllerRotationYaw = false;
	//사운드
	IdleAudio->Play();
	IdleAudio->OnAudioFinished.AddDynamic(this,&ACPP_Tank_M1A1::IdleSoundPlay);
	EngineAudio->OnAudioFinished.AddDynamic(this,&ACPP_Tank_M1A1::EngineSoundStop);
	GunSystemAudio->OnAudioFinished.AddDynamic(this,&ACPP_Tank_M1A1::GunSystemSoundStop);
	//Damage
	HP = MAX_HP;
}

void ACPP_Tank_M1A1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsLocallyControlled())
		CamPitchLimitSmooth();
}

void ACPP_Tank_M1A1::Dead()
{
	Super::Dead();
	UMaterialInstanceConstant* DamageMat =
		Cast<UMaterialInstanceConstant>(UAssetManager::GetStreamableManager().LoadSynchronous(FSoftObjectPath(L"MaterialInstanceConstant'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/Damaged/Materials/MI_West_Tank_M1A1Abrams_Damaged.MI_West_Tank_M1A1Abrams_Damaged'")));
	GetMesh()->SetMaterial(0,DamageMat);
}


void ACPP_Tank_M1A1::ParameterSet()
{
	//sight
	CamRange = 800;
	BasicCamTurnSpeed = 100;
	PitchLimitMax = 20;
	PitchLimitMin = -10;
	CamType = ECameraType::THIRD;
	//Damage
	MAX_HP = 100;
	HP = MAX_HP;
}

void ACPP_Tank_M1A1::RootSet()
{
	/*객체 초기화*/
	//capsule
	class UCapsuleComponent* root = dynamic_cast<UCapsuleComponent*>(RootComponent);
	root->SetSimulatePhysics(true);
	root->SetGenerateOverlapEvents(true);
	root->SetCollisionProfileName("PhysicsActor");
	//sk
	GetMesh()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(L"SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/SK_West_Tank_M1A1Abrams.SK_West_Tank_M1A1Abrams'").Object);
	//anim
	ConstructorHelpers::FClassFinder<UAnimInstance> animInst
	(L"AnimBlueprint'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/ABP_West_Tank_M1A1Abrams.ABP_West_Tank_M1A1Abrams_C'");
	GetMesh()->SetAnimInstanceClass(animInst.Class);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -100));
}

void ACPP_Tank_M1A1::CollisionSet()
{
	//collider
	//차체
	FrontUpper = CreateDefaultSubobject<UBoxComponent>(L"FrontUpper");
	FrontUpper->SetupAttachment(RootComponent);
	Engine = CreateDefaultSubobject<UBoxComponent>(L"Engine");
	Engine->SetupAttachment(RootComponent);
	Bottom = CreateDefaultSubobject<UBoxComponent>(L"Bottom");
	Bottom->SetupAttachment(RootComponent);
	FrontUnder = CreateDefaultSubobject<UBoxComponent>(L"FrontUnder");
	FrontUnder->SetupAttachment(RootComponent);
	LSide = CreateDefaultSubobject<UBoxComponent>(L"LSide");
	LSide->SetupAttachment(RootComponent);
	RSide = CreateDefaultSubobject<UBoxComponent>(L"RSide");
	RSide->SetupAttachment(RootComponent);
	//포탑
	Turret = CreateDefaultSubobject<UBoxComponent>(L"Turret");
	Turret->SetupAttachment(RootComponent);
	/*객체 초기화*/
	//collider
	//bp에서 처리해야하나?
	FrontUpper->SetRelativeLocation(FVector(40,0,40));
	FrontUpper->SetRelativeRotation(FRotator(-5,0,0));
	FrontUpper->SetBoxExtent(FVector(350,170,10));
	FrontUpper->SetCollisionProfileName("TankCollider");
	Engine->SetRelativeLocation(FVector(-310, 0, 15));
	Engine->SetBoxExtent(FVector(32, 170, 60));
	Engine->SetCollisionProfileName("TankCollider");
	Bottom->SetRelativeLocation(FVector(-10, 0, -60));
	Bottom->SetBoxExtent(FVector(270, 170, 35));
	Bottom->SetCollisionProfileName("TankCollider");
	FrontUnder->SetRelativeLocation(FVector(300, 0, -35));
	FrontUnder->SetRelativeRotation(FRotator(25, 0, 0));
	FrontUnder->SetBoxExtent(FVector(60, 170, 25));
	FrontUnder->SetCollisionProfileName("TankCollider");
	LSide->SetRelativeLocation(FVector(-20, -140, 0));
	LSide->SetRelativeRotation(FRotator(-3, 0, 0));
	LSide->SetBoxExtent(FVector(330, 40, 60));
	LSide->SetCollisionProfileName("TankCollider");
	RSide->SetRelativeLocation(FVector(-20, 140, 0));
	RSide->SetRelativeRotation(FRotator(-3, 0, 0));
	RSide->SetBoxExtent(FVector(330, 40, 60));
	RSide->SetCollisionProfileName("TankCollider");
	
	Turret->SetRelativeLocation(FVector(0,0,110));
	Turret->SetBoxExtent(FVector(200,160,50));
	Turret->SetCollisionProfileName("TankCollider");
}

void ACPP_Tank_M1A1::CameraSet()
{
	//camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(L"SpringArm");
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->SetIsReplicated(true);
	Camera = CreateDefaultSubobject<UCameraComponent>(L"Camera");
	Camera->SetupAttachment(SpringArm);
	Camera->SetIsReplicated(true);
	GunnerSpringArm = CreateDefaultSubobject<USpringArmComponent>("GunnerSpringArm");
	GunnerSpringArm->SetupAttachment(GetMesh());
	GunnerSpringArm->SetIsReplicated(true);
	GunnerCam = CreateDefaultSubobject<UCameraComponent>(L"GunnerCam");
	GunnerCam->SetupAttachment(GunnerSpringArm);
	GunnerCam->SetIsReplicated(true);
	
	/*객체 초기화*/
	//camera
	SpringArm->SetRelativeLocation(FVector(0, 0, 260));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = CamRange;
	Camera->SetRelativeLocation(FVector(0,0,200));
	GunnerSpringArm->SetRelativeLocation(FVector(0, 0, 0));
	GunnerSpringArm->bUsePawnControlRotation = true;
	GunnerSpringArm->TargetArmLength = 0;
	GunnerSpringArm->SetupAttachment(GetMesh(),"GunnerCamPos");
	GunnerCam->SetRelativeLocation(FVector(0,0,20));
	GunnerCam->SetupAttachment(GunnerSpringArm);
}

void ACPP_Tank_M1A1::ParticleSet()
{
	//particle
	MuzzleFlashEffect = CreateDefaultSubobject<UParticleSystemComponent>(L"MuzzleFlash");
	MuzzleFlashEffect->SetupAttachment(GetMesh());
	ShockWaveEffect = CreateDefaultSubobject<UParticleSystemComponent>(L"ShockWave");
	ShockWaveEffect->SetupAttachment(GetMesh());
	WheelsEffect.SetNum(8);
	for(int i =0;i<WheelsEffect.Num();i++)
	{
		FString name = FString::Printf(TEXT("Wheel%d"),i);
		WheelsEffect[i] = CreateDefaultSubobject<UParticleSystemComponent>(FName(name));
		WheelsEffect[i]->bAutoDestroy=false;
	}
	/*객체 초기화*/
	//particle
	ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleParticle
	(L"ParticleSystem'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_MuzzleFire_01_M1A1Abrams.PS_MuzzleFire_01_M1A1Abrams'");
	MuzzleFlashEffect->Template = MuzzleParticle.Object;
	MuzzleFlashEffect->SetupAttachment(GetMesh(),"gun_1_jntSocket");
	MuzzleFlashEffect->bAutoActivate = false;
	MuzzleFlashEffect->bAutoDestroy = false;
	ConstructorHelpers::FObjectFinder<UParticleSystem> ShockWaveParticle
	(L"ParticleSystem'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_ShockWave_M1A1Abrams.PS_ShockWave_M1A1Abrams'");
	ShockWaveEffect->Template = ShockWaveParticle.Object;
	ShockWaveEffect->SetupAttachment(GetMesh(),"root_jnt");
	ShockWaveEffect->bAutoActivate = false;
	ShockWaveEffect->bAutoDestroy = false;
	ConstructorHelpers::FObjectFinder<UParticleSystem> WheelParticle
	(L"ParticleSystem'/Game/VigilanteContent/Shared/Particles/ParticleSystems/PS_Dust_WheelTrack_03.PS_Dust_WheelTrack_03'");
	for(int i =0;i<WheelsEffect.Num();i++)
	{
		FString name = FString::Printf(TEXT("Wheel%d"),i);
		WheelsEffect[i]->SetupAttachment(GetMesh(),FName(name));
		WheelsEffect[i]->Template = WheelParticle.Object;
		WheelsEffect[i]->bAutoActivate = false;
		WheelsEffect[i]->bAutoDestroy = false;
	}
}

void ACPP_Tank_M1A1::SoundSet()
{
	EngineAudio = CreateDefaultSubobject<UAudioComponent>(L"EngineAudio");
	EngineAudio->SetupAttachment(Engine);
	IdleAudio = CreateDefaultSubobject<UAudioComponent>(L"IdleAudio");
	IdleAudio->SetupAttachment(GetMesh());
	GunSystemAudio = CreateDefaultSubobject<UAudioComponent>(L"GunSystemAudio");
	GunSystemAudio->SetupAttachment(GunnerCam);
	TurretSystemAudio = CreateDefaultSubobject<UAudioComponent>(L"TurretSystemAudio");
	TurretSystemAudio->SetupAttachment(GunnerCam);
	HitAudio = CreateDefaultSubobject<UAudioComponent>(L"HitAudio");
	HitAudio->SetupAttachment(GetMesh());
	
	ConstructorHelpers::FObjectFinder<USoundAttenuation> EngineAttenuation
	(L"SoundAttenuation'/Game/BP/Sound/Attenuation/EngineSoundAttenuation.EngineSoundAttenuation'");
	EngineSoundAttenuation = EngineAttenuation.Object;
	IdleAudio->AttenuationSettings = EngineAttenuation.Object;
	EngineAudio->AttenuationSettings = EngineAttenuation.Object;
	ConstructorHelpers::FObjectFinder<USoundAttenuation> MainGunAttenuation
	(L"SoundAttenuation'/Game/BP/Sound/Attenuation/MainGunSoundAttenuation.MainGunSoundAttenuation'");
	GunSystemAudio->AttenuationSettings = MainGunAttenuation.Object;
	MainGunSoundAttenuation = MainGunAttenuation.Object;
	ConstructorHelpers::FObjectFinder<USoundAttenuation> TurretAttenuation
	(L"SoundAttenuation'/Game/BP/Sound/Attenuation/TurretSoundAttenuation.TurretSoundAttenuation'");
	TurretSoundAttenuation = TurretAttenuation.Object;
	TurretSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	HitAudio->AttenuationSettings = MainGunAttenuation.Object;
	
	/*객체 초기화*/
	MainGunFireSound.SetNum(2);
	//Sound
	ConstructorHelpers::FObjectFinder<USoundCue> IdleStartCue
	(L"SoundCue'/Game/Sound/Tank/Engine/M1A2_Start_Idle_Stop_Wave_0_0_0_Cue.M1A2_Start_Idle_Stop_Wave_0_0_0_Cue'");
	IdleStartSound=IdleStartCue.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> IdleLoopCue
	(L"SoundCue'/Game/Sound/Tank/Engine/M1A2_Start_Idle_Stop_Wave_0_0_1_Cue.M1A2_Start_Idle_Stop_Wave_0_0_1_Cue'");
	IdleLoopSound=IdleLoopCue.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> IdleEndCue
	(L"SoundCue'/Game/Sound/Tank/Engine/M1A2_Start_Idle_Stop_Wave_0_0_2_Cue.M1A2_Start_Idle_Stop_Wave_0_0_2_Cue'");
	IdleEndSound=IdleEndCue.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> EngineStartCue
	(L"SoundCue'/Game/Sound/Tank/Engine/M1A2_SuperTurbine_Wave_0_0_0_Cue.M1A2_SuperTurbine_Wave_0_0_0_Cue'");
	EngineStartSound=EngineStartCue.Object;
	ConstructorHelpers::FObjectFinder<USoundWave> EngineLoopCue
	(L"SoundWave'/Game/Sound/Tank/Engine/M1A2_SuperTurbine_Wave_0_0_1.M1A2_SuperTurbine_Wave_0_0_1'");
	EngineLoopSound=EngineLoopCue.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> EngineEndCue
	(L"SoundCue'/Game/Sound/Tank/Engine/M1A2_SuperTurbine_Wave_0_0_2_Cue.M1A2_SuperTurbine_Wave_0_0_2_Cue'");
	EngineEndSound=EngineEndCue.Object;
	
	
	MainGunFireSound[0]=ConstructorHelpers::FObjectFinder<USoundCue>(L"SoundCue'/Game/Sound/Tank/Fire/3DFire.3DFire'").Object;
	MainGunFireSound[1]=ConstructorHelpers::FObjectFinder<USoundCue>(L"SoundCue'/Game/Sound/Tank/Fire/FPFire.FPFire'").Object;
	
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunReloadCue
	(L"SoundCue'/Game/Sound/Tank/Reload/120mm_Cannon_Reload_M1A2_Wave_0_0_0_Cue.120mm_Cannon_Reload_M1A2_Wave_0_0_0_Cue'");
	MainGunReloadSound=MainGunReloadCue.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunReloadDoneCue
	(L"SoundCue'/Game/Sound/Tank/Reload/ReloadDone.ReloadDone'");
	MainGunReloadDoneSound=MainGunReloadDoneCue.Object;

	ConstructorHelpers::FObjectFinder<USoundCue> TurretLoop
	(L"SoundCue'/Game/Sound/Tank/Turret/Turret_Loop_01_Wave_0_0_0_Cue.Turret_Loop_01_Wave_0_0_0_Cue'");
	TurretLoopSound = TurretLoop.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> TurretEnd
	(L"SoundCue'/Game/Sound/Tank/Turret/Turret_Start_Stop_Wave_0_3_0_Cue.Turret_Start_Stop_Wave_0_3_0_Cue'");
	TurretEndSound = TurretEnd.Object;

	ConstructorHelpers::FObjectFinder<USoundCue> Hit
	(L"SoundCue'/Game/Sound/Tank/Hit/Tank_turret_impact_heavy_Cue.Tank_turret_impact_heavy_Cue'");
	TankHitSound = Hit.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> RicochetHit
	(L"SoundCue'/Game/Sound/Tank/Hit/Tank_turret_small_shockricos_sheetmetal_Cue.Tank_turret_small_shockricos_sheetmetal_Cue'");
	TankRicochetHitSound = RicochetHit.Object;
	
	IdleAudio->Sound = IdleStartSound;
	IdleAudio->VolumeMultiplier = 0.2f;
	EngineAudio->Sound = EngineStartSound;
	EngineAudio->VolumeMultiplier = 0.3f;
	GunSystemAudio->VolumeMultiplier=0.5f;
	TurretSystemAudio->VolumeMultiplier=0.3f;
	HitAudio->VolumeMultiplier=1.0f;
}
