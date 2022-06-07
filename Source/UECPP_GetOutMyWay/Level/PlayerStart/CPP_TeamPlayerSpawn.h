
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CPP_TeamPlayerSpawn.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_TeamPlayerSpawn : public APlayerStart
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	
	
	FORCEINLINE FString GetTeamInfo() const
	{return TeamInfo;}
	
	
private:
	UPROPERTY(EditAnywhere,Category=TeamName)
	FString TeamInfo;

	
};
