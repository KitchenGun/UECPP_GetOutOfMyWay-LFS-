#include "Level/PlayerStart/CPP_TeamPlayerSpawn.h"

#include "Components/CapsuleComponent.h"


void ACPP_TeamPlayerSpawn::BeginPlay()
{
	GetCapsuleComponent()->SetActive(false);
	Super::BeginPlay();
}
