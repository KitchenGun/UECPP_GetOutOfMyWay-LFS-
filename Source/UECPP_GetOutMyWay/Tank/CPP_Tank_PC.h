#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_Tank_PC.generated.h"

class ACPP_Tank_Character;

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Tank_PC : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACPP_Tank_PC();
	virtual void BeginPlay() override;
	void TeamSelect(FString TeamName);
	UFUNCTION(Server,Reliable)
	void Server_TeamSelect();
	void Server_TeamSelect_Implementation();
	UFUNCTION()
	void SelectRedTeam();
	UFUNCTION()
	void SelectBlueTeam();

	void SetOwnPawn(APawn* value);
	
	ACPP_Tank_Character* GetOwnPawn()
	{
		return OwnPawn;
	}
	
		
private:
	class AUECPP_GetOutMyWayGameModeBase* GM;
	
	FString Team;

	class ACPP_Tank_Character* OwnPawn;
	
	TSubclassOf<UUserWidget> BP_SelectWidget;

	class UCPP_UserWidget_TeamSelect* SelectWidget;
};
