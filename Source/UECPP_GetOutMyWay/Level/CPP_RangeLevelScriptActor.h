#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CPP_RangeLevelScriptActor.generated.h"


UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_RangeLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
public:
	ACPP_RangeLevelScriptActor();
protected:
	
	virtual void BeginPlay() override;
private:
	TSubclassOf<class UCPP_UserWidget_TeamSelect> BP_SelectWidget;
};
