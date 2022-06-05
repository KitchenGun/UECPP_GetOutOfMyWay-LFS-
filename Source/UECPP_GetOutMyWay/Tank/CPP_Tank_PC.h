#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_Tank_PC.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Tank_PC : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACPP_Tank_PC();
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void TeamSelect();

	UFUNCTION()
	void SelectRedTeam();
	UFUNCTION()
	void SelectBlueTeam();
		
private:
	class AUECPP_GetOutMyWayGameModeBase* GM;
	
	FString Team;
	
	TSubclassOf<UUserWidget> BP_SelectWidget;
};
