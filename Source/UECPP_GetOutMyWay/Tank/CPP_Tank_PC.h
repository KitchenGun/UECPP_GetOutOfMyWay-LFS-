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
	void Server_TeamSelect(const FString& NewTeam,class ACPP_Tank_PC* PC);
	void Server_TeamSelect_Implementation(const FString& NewTeam,class ACPP_Tank_PC* NewPC);
	UFUNCTION()
	void SelectRedTeam();
	UFUNCTION()
	void SelectBlueTeam();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
private:
	class AUECPP_GetOutMyWayGameModeBase* GM;
	
	FString Team;

	TSubclassOf<UUserWidget> BP_SelectWidget;

	class UCPP_UserWidget_TeamSelect* SelectWidget;
};
