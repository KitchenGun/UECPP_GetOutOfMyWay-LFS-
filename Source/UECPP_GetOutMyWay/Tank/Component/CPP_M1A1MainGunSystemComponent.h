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
	
	void MainGunFire() override;

	UFUNCTION(Server,Reliable)	
	virtual void Server_MainGunFire(FVector SpawnPos,FRotator Direction);
	virtual void Server_MainGunFire_Implementation(FVector SpawnPos,FRotator Direction);
	
protected:
	virtual void BeginPlay() override;
private:
	float M1A1ReloadTime =5.0f;
};
