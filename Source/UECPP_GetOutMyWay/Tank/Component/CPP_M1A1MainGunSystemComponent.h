#pragma once

#include "CoreMinimal.h"
#include "Tank/Component/CPP_MainGunSystemComponent.h"
#include "CPP_M1A1MainGunSystemComponent.generated.h"


UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_M1A1MainGunSystemComponent : public UCPP_MainGunSystemComponent
{
	GENERATED_BODY()
	
public:
	UCPP_M1A1MainGunSystemComponent();

	virtual void MainGunFire() override;
	UFUNCTION(Server,Reliable)
	void Server_MainGunFire(FVector SpawnPos,FRotator Direction);
	void Server_MainGunFire_Implementation(FVector SpawnPos,FRotator Direction);
	
protected:
	virtual void BeginPlay() override;
	
private:
	float M1A1ReloadTime =5.0f;

	class UCPP_ObjectPoolManager* ObjPoolManager;
	class ACPP_Projectile* tempProjectile;
};
