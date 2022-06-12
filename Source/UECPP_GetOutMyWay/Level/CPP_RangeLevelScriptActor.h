#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CPP_RangeLevelScriptActor.generated.h"


UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_RangeLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void SpawnPlayer(class ACPP_Tank_Character* Tank,class ACPP_Tank_PC* PC);
	
};
