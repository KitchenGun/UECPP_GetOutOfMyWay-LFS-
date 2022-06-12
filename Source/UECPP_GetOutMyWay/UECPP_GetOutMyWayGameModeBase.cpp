#include "UECPP_GetOutMyWayGameModeBase.h"
#include "Tank/CPP_Tank_PC.h"
#include "Blueprint/UserWidget.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Level/CPP_RangeLevelScriptActor.h"
#include "Tank/CPP_Tank_Character.h"

void AUECPP_GetOutMyWayGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UCPP_MultiplayGameInstance>(GetGameInstance());
	GI->FindSpawnPoint();
}

AUECPP_GetOutMyWayGameModeBase::AUECPP_GetOutMyWayGameModeBase()
{
	PlayerControllerClass = ConstructorHelpers::FClassFinder<ACPP_Tank_PC>(L"Blueprint'/Game/BP/BP_Tank_PC.BP_Tank_PC_C'").Class;
	Player = ConstructorHelpers::FClassFinder<APawn>(L"Blueprint'/Game/BP/Tank/BP_M1A1_Character.BP_M1A1_Character_C'").Class;
}
void AUECPP_GetOutMyWayGameModeBase::Spawn_Implementation(const FString& Team, ACPP_Tank_PC* PC)
{
	ACPP_Tank_Character* myTank = nullptr;
	FTransform SpawnTrans;
	PC->OnUnPossess();
	GL =Cast<ACPP_RangeLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if(Team.Equals("Red"))
	{
		//»¡°£ÆÀ
		SpawnTrans = GI->GetRedTeamSpawnPoints()[0]->GetTransform();
		if(SpawnTrans.IsValid())
		{
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(Player,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			myTank->SetPC(PC);
			GL->SpawnPlayer(myTank,PC);
		}
	}
	else
	{//ÆÄ¶õÆÀ
		SpawnTrans = GI->GetBlueTeamSpawnPoints()[0]->GetTransform();
		if(SpawnTrans.IsValid())
		{
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(Player,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			myTank->SetPC(PC);
			GL->SpawnPlayer(myTank,PC);
		}
	}
}
