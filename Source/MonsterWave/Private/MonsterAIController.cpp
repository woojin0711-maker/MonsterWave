

#include "MonsterAIController.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	
	
	if (PlayerCharacter)
	{
		MoveToActor(PlayerCharacter, 100.0f);
	}
}