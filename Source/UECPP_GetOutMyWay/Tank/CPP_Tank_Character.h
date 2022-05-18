#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_Tank_Character.generated.h"

DECLARE_DELEGATE(FFire);
DECLARE_DELEGATE(FFPViewEffect);
DECLARE_DELEGATE_OneParam(FGunSightWidgetPosFunc,FVector2D)
DECLARE_DELEGATE(FZoomToggleFunc)
DECLARE_DELEGATE_OneParam(FSetRangeText,int32)

UENUM(BlueprintType)
enum class ECameraType : uint8
{
	THIRD		UMETA(DisplayName = "Third"),
	GUNNER		UMETA(DisplayName = "Gunner"),
	MAX
};


UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Tank_Character : public ACharacter
{
	GENERATED_BODY()

public:
	ACPP_Tank_Character();

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void GunDirPosWorldToScreen();
	
	//get&set
	FORCEINLINE UParticleSystemComponent* GetMuzzleFlashEffect() {return MuzzleFlashEffect;}
	FORCEINLINE UParticleSystemComponent* GetShockWaveEffect() {return ShockWaveEffect;}
	FORCEINLINE TArray<class UParticleSystemComponent*> GetWheelsEffect() {return WheelsEffect;}
	FORCEINLINE APlayerController* GetPlayerController() {return PC;}
	FORCEINLINE float GetGunAngleOffset() {return displacementAngle;}
	//Delegate
	FFire FireFunc;
	FFPViewEffect FpViewToggleFunc;
	FZoomToggleFunc FZoomToggleFunc;
	FSetRangeText FSetRangeTextFunc;
	FGunSightWidgetPosFunc FGunSightPosFunc;
protected:
	//sight
	void OnVerticalLook(float value);
	UFUNCTION(Server,Reliable)
	void Server_OnVerticalLook(float value);
	void Server_OnVerticalLook_Implementation(float value);
	void OnHorizontalLook(float value);
	UFUNCTION(Server,Reliable)
	void Server_OnHorizontalLook(float value);
	void Server_OnHorizontalLook_Implementation(float value);
	
	void CamPitchLimitSmooth();
	UFUNCTION(Server,Reliable)
	void Server_CamPitchLimitSmooth();
	void Server_CamPitchLimitSmooth_Implementation();
	void CamChange();
	UFUNCTION(Server,Reliable)
	void Server_CamChange();
	void Server_CamChange_Implementation();
	//move
	void OnMoveForward(float value);
	UFUNCTION(Server,Reliable)
	void Server_OnMoveForward(float value);
	void Server_OnMoveForward_Implementation(float value);
	
	void OnMoveTurn(float value);
	UFUNCTION(Server,Reliable)
	void Server_OnMoveTurn(float value);
	void Server_OnMoveTurn_Implementation(float value);
	
	void OnEngineBreak();
	UFUNCTION(Server,Reliable)
	void Server_OnEngineBreak();
	void Server_OnEngineBreak_Implementation();
	
	void OffEngineBreak();
	UFUNCTION(Server,Reliable)
	void Server_OffEngineBreak();
	void Server_OffEngineBreak_Implementation();

	//action
	void OnMainGunFire();
	void ZoomToggle();
	//particle
	UFUNCTION(Server,Reliable)
	void OnWheelParticle();
	void OnWheelParticle_Implementation();
	UFUNCTION()
	void OnFireParticle();
	//Sound
	UFUNCTION()
	void IdleSoundPlay();
	void EngineSoundPlay();
	UFUNCTION()
	void EngineSoundStop();
	void GunSystemSoundPlay();
	UFUNCTION()
	virtual void GunSystemSoundStop();
	UFUNCTION()
	virtual void GunSystemSoundReloadDone();
	UFUNCTION()
	virtual void TurretMoveLoop();
	UFUNCTION()
	virtual void TurretMoveEnd();
	//Damage
	UFUNCTION()
	virtual void Dead();
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
protected:
	//Camera
	UPROPERTY(EditDefaultsOnly,Replicated)
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly,Replicated)
	class UCameraComponent* GunnerCam;
	UPROPERTY(EditDefaultsOnly,Replicated)
	class USpringArmComponent* GunnerSpringArm;
	UPROPERTY(EditDefaultsOnly,Replicated)
	class USpringArmComponent* SpringArm;
	
	//Collision
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* FrontUpper;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* FrontUnder;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Engine;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Bottom;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* LSide;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* RSide;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Turret;

	//Particle
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* MuzzleFlashEffect;
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* ShockWaveEffect;
	UPROPERTY(EditDefaultsOnly)
	TArray<class UParticleSystemComponent*> WheelsEffect;

	//sound
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* EngineAudio;
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* IdleAudio;
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* GunSystemAudio;
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* TurretSystemAudio;
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* HitAudio;

	//ActorComp
	UPROPERTY(EditDefaultsOnly)
	class UCPP_TrackMovementComponent* TrackMovement;
	UPROPERTY(EditDefaultsOnly)
	class UCPP_TankPawnMovementComponent* TankMovement;
	UPROPERTY(EditDefaultsOnly)
	class UCPP_MainGunSystemComponent* GunSystem;
	UPROPERTY(EditDefaultsOnly)
	class UCPP_ParticleControlComponent* ParticleSystem;
	UPROPERTY(EditDefaultsOnly)
	class UCPP_TankUIComponent* TankUI;

	//APlayerController
	APlayerController* PC;

	//sight
	float CamRange = 800;
	float BasicCamTurnSpeed = 100;
	float PitchLimitMax = 20;
	float PitchLimitMin = -10;
	float displacementAngle =0.0f;
	ECameraType CamType = ECameraType::THIRD;
	bool IsZoom = false;

	//sound
	class USoundWave* EngineStartSound;
	class USoundWave* EngineLoopSound;
	class USoundWave* EngineEndSound;
	
	class USoundWave* IdleStartSound;
	class USoundWave* IdleLoopSound;
	class USoundWave* IdleEndSound;

	class USoundWave* TurretLoopSound;
	class USoundWave* TurretEndSound;

	TArray<class USoundCue*> MainGunFireSound;
	class USoundCue* MainGunReloadSound;

	class USoundCue* MainGunReloadDoneSound;
	class USoundCue* TankHitSound;
	class USoundCue* TankRicochetHitSound;
	
	bool IsMoveBefore=false;
	bool IsEngineEnd = true;
	bool IsGunReloadDone=true;

	class USoundAttenuation* MainGunSoundAttenuation;
	class USoundAttenuation* EngineSoundAttenuation;
	class USoundAttenuation* TurretSoundAttenuation;


	//Damage
	float MAX_HP = 100;
	UPROPERTY(Replicated)
	float HP = MAX_HP;
	bool IsDead=false;
};
