#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UECPP_GetOutMyWayGameModeBase.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API AUECPP_GetOutMyWayGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	AUECPP_GetOutMyWayGameModeBase();
	//void Spawn(const FString& Team,APlayerController* PC);
	UFUNCTION(Server,Reliable)
	void Spawn(const FString& Team,class ACPP_Tank_PC* PC);
	void Spawn_Implementation(const FString& Team,ACPP_Tank_PC* PC);

private:
	
	//GameInstance
	class UCPP_MultiplayGameInstance* GI;
	class ACPP_RangeLevelScriptActor* GL;
	//pawn
	TSubclassOf<APawn> Player;
};
