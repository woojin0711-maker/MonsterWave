
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MonsterWaveGameStateBase.generated.h"


UCLASS()
class MONSTERWAVE_API AMonsterWaveGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	

public:
	AMonsterWaveGameStateBase();

	//현재 웨이브
	void SetCurrentWave(int32 NewWave);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetCurrentWave() const;



	//남은시간
	void SetRemainingTime(float NewTime);

	UFUNCTION(BlueprintPure, Category = "Game State")
	float GetRemainingTime() const;

	//점수
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Game State")
	int32 GetScore() const;



protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWave, BlueprintReadOnly, Category = "Game State")
	int32 CurrentWave;
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "Game State")
	float RemainingTime;
	UPROPERTY(ReplicatedUsing = OnRep_Score, BlueprintReadOnly, Category = "Game State")
	int32 Score;



	UFUNCTION()
	void OnRep_CurrentWave();
	UFUNCTION()
	void OnRep_RemainingTime();
	UFUNCTION()
	void OnRep_Score();



};
