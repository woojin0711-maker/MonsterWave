
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"

UCLASS()
class MONSTERWAVE_API AMonster : public ACharacter
{
	GENERATED_BODY()
	
public:	
	AMonster();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
