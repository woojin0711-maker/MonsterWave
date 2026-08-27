// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WorldGameModeBase.generated.h"


class AMonster;

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

public:

	FWaveData() : Duration(30.0f), SpawnCount(10) {}

	FWaveData(float InDuration, int32 InSpawnCount) : Duration(InDuration), SpawnCount(InSpawnCount) {}


	//제한시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float Duration;

	// 생성할 아이템 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 SpawnCount;
};


UCLASS()
class MONSTERWAVE_API AWorldGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:

	AWorldGameModeBase();


protected:

	virtual void BeginPlay() override;


	//설정 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveData> WaveDatas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<class AMonster> MonsterClass;



private:

	int32 CurrentWaveIndex;

	FTimerHandle WaveTimerHandle;

	void StartWave();


	void UpdateWaveTimer();


	void EndWave();


	void CompleteLevel();

	void SpawnMonsters();

	
};
