#include "UECPP_GetOutMyWayGameModeBase.h"

#include "Blueprint/UserWidget.h"

AUECPP_GetOutMyWayGameModeBase::AUECPP_GetOutMyWayGameModeBase()
{
	//ConstructorHelpers::FClassFinder<ACPP_M1A1> player(L"Blueprint'/Game/BP/Tank/BP_M1A1.BP_M1A1_C'");//class´Â_CºÙ¿©¾ßÇÔ
	ConstructorHelpers::FClassFinder<APawn> player(L"Blueprint'/Game/BP/Tank/BP_M1A1_Character.BP_M1A1_Character_C'");
	if (player.Succeeded())
	{
		DefaultPawnClass = player.Class;
	}
}

void AUECPP_GetOutMyWayGameModeBase::Spawn_Implementation(const FString& Team, ACPP_Tank_PC* PC)
{
	if(Team.Equals("Red"))
	{//»¡°£ÆÀ
		
	}
	else
	{//ÆÄ¶õÆÀ
		
	}
	
	
}
