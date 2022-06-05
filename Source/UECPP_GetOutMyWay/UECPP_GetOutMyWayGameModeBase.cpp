#include "UECPP_GetOutMyWayGameModeBase.h"
#include "Tank/CPP_Tank_PC.h"
#include "Blueprint/UserWidget.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Character.h"
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"

AUECPP_GetOutMyWayGameModeBase::AUECPP_GetOutMyWayGameModeBase()
{
	Player = ConstructorHelpers::FClassFinder<APawn>(L"Blueprint'/Game/BP/Tank/BP_M1A1_Character.BP_M1A1_Character_C'").Class;
	PlayerControllerClass = ConstructorHelpers::FClassFinder<ACPP_Tank_PC>(L"Blueprint'/Game/BP/BP_Tank_PC.BP_Tank_PC_C'").Class;
}

void AUECPP_GetOutMyWayGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UCPP_MultiplayGameInstance>(GetGameInstance());
}

void AUECPP_GetOutMyWayGameModeBase::Spawn_Implementation(const FString& Team, ACPP_Tank_PC* PC)
{
	class ACPP_Tank_Character* myTank;
	DefaultPawnClass = Player;
	FTransform SpawnTrans;
	if(Team.Equals("Red"))
	{
		//»¡°£ÆÀ
		SpawnTrans = GI->GetRedTeamSpawnPoints()[0]->GetTransform();
		if(SpawnTrans.IsValid())
		{
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			PC->Possess(myTank);
			
		}
		else
		{
			while (SpawnTrans.IsValid())
			{
				GI->FindSpawnPoint();
				SpawnTrans = GI->GetRedTeamSpawnPoints()[0]->GetTransform();
			}
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			PC->Possess(myTank);
		}
	}
	else
	{//ÆÄ¶õÆÀ
		SpawnTrans = GI->GetBlueTeamSpawnPoints()[0]->GetTransform();
		if(SpawnTrans.IsValid())
		{
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			PC->Possess(myTank);
		}
		else
		{
			while (SpawnTrans.IsValid())
			{
				GI->FindSpawnPoint();
				SpawnTrans = GI->GetBlueTeamSpawnPoints()[0]->GetTransform();
			}
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			PC->Possess(myTank);
		}
	}
	//°ÔÀÓ ÀÎÇ² Àü¿ë
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = false;
}
