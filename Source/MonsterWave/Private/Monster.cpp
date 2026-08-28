
#include "Monster.h"
#include "MonsterAIController.h"



AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMonsterAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void AMonster::BeginPlay()
{
	Super::BeginPlay();

}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}