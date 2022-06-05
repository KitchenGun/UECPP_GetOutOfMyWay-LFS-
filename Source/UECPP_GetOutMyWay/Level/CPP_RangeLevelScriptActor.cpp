#include "Level/CPP_RangeLevelScriptActor.h"

#include "Blueprint/UserWidget.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"


void ACPP_RangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UCPP_MultiplayGameInstance>(GetGameInstance());
	GI->FindSpawnPoint();
}
