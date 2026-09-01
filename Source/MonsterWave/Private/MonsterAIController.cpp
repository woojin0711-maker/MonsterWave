

#include "MonsterAIController.h"

#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// Possess된 Pawn이 유효한지 확인
	if (!IsValid(InPawn))
	{
		return;
	}
	// Pawn이 Character인지 확인
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (IsValid(PlayerCharacter))
	{
		// PlayerCharacter를 타겟으로 설정
		SetFocus(PlayerCharacter);
		MoveToActor(PlayerCharacter, 100.0f);
	}
}

void AMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* ControlledPawn = GetPawn();

	if (!ControlledPawn)
	{
		return;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (!IsValid(ControlledPawn) || !IsValid(PlayerCharacter))
	{
		return;
	}

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());

	if (Distance > 120.0f && GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		MoveToActor(PlayerCharacter, 100.0f);
	}
	else if (Distance <= 120.0f && GetMoveStatus() == EPathFollowingStatus::Moving)
	{
		StopMovement();
	}
}