#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UECPP_GetOutMyWayGameModeBase.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API AUECPP_GetOutMyWayGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUECPP_GetOutMyWayGameModeBase();

	virtual void BeginPlay() override;
	
	UFUNCTION(Server,Reliable)
	void Spawn(const FString& Team,class ACPP_Tank_PC* PC);
	void Spawn_Implementation(const FString& Team,class ACPP_Tank_PC* PC);
private:
	//GameInstance
	class UCPP_MultiplayGameInstance* GI;
	//pawn
	TSubclassOf<APawn> Player;
};
