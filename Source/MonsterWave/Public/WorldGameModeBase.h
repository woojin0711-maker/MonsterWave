// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WorldGameModeBase.generated.h"

class UUserWidget;
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

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ShowGameOver();

	void NotifyMonsterDied(AMonster* DeadMonster);

protected:

	virtual void BeginPlay() override;


	//설정 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveData> WaveDatas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<class AMonster> MonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	FVector SpawnCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	float SpawnRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	float SpawnInterval = 0.5f;

	UPROPERTY()
	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidgetInstance;

	TArray<TWeakObjectPtr<AMonster>> SpawnedMonsters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> GameOverWidgetInstance;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartCurrentLevel();


private:

	int32 CurrentWaveIndex;

	int32 RemainingMonsterSpawnCount = 0;

	FTimerHandle WaveTimerHandle;

	FTimerHandle MonsterSpawnTimerHandle;

	void StartWave();

	void UpdateWaveTimer();

	void EndWave();

	void CompleteLevel();

	void SpawnMonsters();

	void ClearSpawnedMonsters();

	void SpawnNextMonster();
	
};
