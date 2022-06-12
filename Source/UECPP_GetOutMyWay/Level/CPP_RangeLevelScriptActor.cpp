#include "Level/CPP_RangeLevelScriptActor.h"
#include "Tank/CPP_Tank_PC.h"
#include "Tank/CPP_Tank_Character.h"
#include "Kismet/GameplayStatics.h"



void ACPP_RangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACPP_RangeLevelScriptActor::SpawnPlayer(ACPP_Tank_Character* Tank, ACPP_Tank_PC* PC)
{
	PC->OnPossess(Tank);
	Tank->SetPC(PC);
	Tank->SetUI();
}
