
#pragma once

#include "CoreMinimal.h"
#include "Tank/CPP_Tank_Character.h"
#include "CPP_Tank_M1A1.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Tank_M1A1 : public ACPP_Tank_Character
{
	GENERATED_BODY()

public:
	ACPP_Tank_M1A1();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Dead() override;
	
private:
	//Setup
	void ParameterSet();
	void RootSet();
	void CollisionSet();
	void CameraSet();
	void ParticleSet();
	void SoundSet();
};
