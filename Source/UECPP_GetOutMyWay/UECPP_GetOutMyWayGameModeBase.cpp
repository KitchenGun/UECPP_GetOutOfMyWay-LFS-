#include "UECPP_GetOutMyWayGameModeBase.h"

#include "Blueprint/UserWidget.h"

AUECPP_GetOutMyWayGameModeBase::AUECPP_GetOutMyWayGameModeBase()
{
	//ConstructorHelpers::FClassFinder<ACPP_M1A1> player(L"Blueprint'/Game/BP/Tank/BP_M1A1.BP_M1A1_C'");//class��_C�ٿ�����
	ConstructorHelpers::FClassFinder<APawn> player(L"Blueprint'/Game/BP/Tank/BP_M1A1Pawn.BP_M1A1Pawn_C'");//testpawn ���� ������
	if (player.Succeeded())
	{
		DefaultPawnClass = player.Class;
	}
}
