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
	FORCEINLINE FTransform GetTankTransform() {return TankTransform;}
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
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_OnVerticalLook(float value);
	void Multicast_OnVerticalLook_Implementation(float value);
	
	void OnHorizontalLook(float value);
	UFUNCTION(Server,Reliable)
	void Server_OnHorizontalLook(float value);
	void Server_OnHorizontalLook_Implementation(float value);
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_OnHorizontalLook(float value);
	void Multicast_OnHorizontalLook_Implementation(float value);
	
	void CamPitchLimitSmooth();
	void CamChange();
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
	void OnWheelParticle();
	UFUNCTION(Server,Reliable)
	void Server_OnWheelParticle();
	void Server_OnWheelParticle_Implementation();
	UFUNCTION()
	void OnFireParticle();
	UFUNCTION(Server,Reliable)
	void Server_OnFireParticle();
	void Server_OnFireParticle_Implementation();
	//Sound
	UFUNCTION()
	void IdleSoundPlay();
	UFUNCTION(Server,Reliable)
	void Server_IdleSoundPlay();
	void Server_IdleSoundPlay_Implementation();
	UFUNCTION()
	void EngineSoundPlay();
	UFUNCTION(Server,Reliable)
	void Server_EngineSoundPlay();
	void Server_EngineSoundPlay_Implementation();
	UFUNCTION()
	void EngineSoundStop();
	UFUNCTION(Server,Reliable)
	void Server_EngineSoundStop();
	void Server_EngineSoundStop_Implementation();
	UFUNCTION()
	void GunSystemSoundPlay();
	UFUNCTION(Server,Reliable)
	void Server_GunSystemSoundPlay();
	void Server_GunSystemSoundPlay_Implementation();
	UFUNCTION()
	virtual void GunSystemSoundStop();
	UFUNCTION(Server,Reliable)
	virtual void Server_GunSystemSoundStop();
	virtual void Server_GunSystemSoundStop_Implementation();
	UFUNCTION()
	virtual void GunSystemSoundReloadDone();
	UFUNCTION(Server,Reliable)
	virtual void Server_GunSystemSoundReloadDone();
	virtual void Server_GunSystemSoundReloadDone_Implementation();
	UFUNCTION()
	virtual void TurretMoveLoop();
	UFUNCTION(Server,Reliable)
	virtual void Server_TurretMoveLoop();
	virtual void Server_TurretMoveLoop_Implementation();
	UFUNCTION()
	virtual void TurretMoveEnd();
	UFUNCTION(Server,Reliable)
	virtual void Server_TurretMoveEnd();
	virtual void Server_TurretMoveEnd_Implementation();
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
	//mesh rotation
	UPROPERTY(ReplicatedUsing=OnRep_UpdateTankTransform)
	FTransform TankTransform = FTransform::Identity;
	UFUNCTION()
	void OnRep_UpdateTankTransform(FTransform value);
	//Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* GunnerCam;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* GunnerSpringArm;
	UPROPERTY(EditDefaultsOnly)
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
	UPROPERTY(VisibleDefaultsOnly,Replicated)
	class UAudioComponent* EngineAudio;
	UPROPERTY(VisibleDefaultsOnly,Replicated)
	class UAudioComponent* IdleAudio;
	UPROPERTY(VisibleDefaultsOnly,Replicated)
	class UAudioComponent* GunSystemAudio;
	UPROPERTY(VisibleDefaultsOnly,Replicated)
	class UAudioComponent* TurretSystemAudio;
	UPROPERTY(VisibleDefaultsOnly,Replicated)
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
	FRotator ControllerRotation = FRotator::ZeroRotator;

	//sound
	class USoundCue* EngineStartSound;
	class USoundCue* EngineLoopSound;
	class USoundCue* EngineEndSound;
	
	class USoundCue* IdleStartSound;
	class USoundCue* IdleLoopSound;
	class USoundCue* IdleEndSound;

	class USoundCue* TurretLoopSound;
	class USoundCue* TurretEndSound;

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
	float HP = MAX_HP;
	bool IsDead=false;
};
