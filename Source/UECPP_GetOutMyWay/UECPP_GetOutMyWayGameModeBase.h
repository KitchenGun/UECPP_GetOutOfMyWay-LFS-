#pragma once

#include <d3d11.h>

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Tank/CPP_Tank_PC.h"
#include "UECPP_GetOutMyWayGameModeBase.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API AUECPP_GetOutMyWayGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUECPP_GetOutMyWayGameModeBase();


	UFUNCTION(Server,Reliable)
	void Spawn(const FString& Team,ACPP_Tank_PC* PC);
	void Spawn_Implementation(const FString& Team,ACPP_Tank_PC* PC);
};
