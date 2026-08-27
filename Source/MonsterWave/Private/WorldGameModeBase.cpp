#include "WorldGameModeBase.h"

#include "MonsterWaveGameStateBase.h"
#include "Monster.h"

#include "TimerManager.h"
#include "Engine/Engine.h"



AWorldGameModeBase::AWorldGameModeBase()
{
	// 사용 gamestate 지정

	GameStateClass = AMonsterWaveGameStateBase::StaticClass();
	CurrentWaveIndex = 0;


	//기본 웨이브 설정

	WaveDatas.Add(FWaveData(30.0f, 10));
	WaveDatas.Add(FWaveData(45.0f, 15));
	WaveDatas.Add(FWaveData(60.0f, 20));
}

void AWorldGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	//웨이브 시작

	CurrentWaveIndex = 0;

	StartWave();
}

void AWorldGameModeBase::StartWave()
{
	if (!WaveDatas.IsValidIndex(CurrentWaveIndex))
	{
		CompleteLevel();
		return;
	}
	

	//gamerstate 가져오기
	AMonsterWaveGameStateBase* MWGameState = GetGameState<AMonsterWaveGameStateBase>();

	if (!MWGameState)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("GameState is not valid.")
		);

		return;
	}

	//현재 웨이브 설정
	const FWaveData& CurrentWaveData = WaveDatas[CurrentWaveIndex];




	//웨이브 정보 기록
	MWGameState->SetCurrentWave(CurrentWaveIndex + 1); //웨이브는 1부터 시작
	MWGameState->SetRemainingTime(CurrentWaveData.Duration);




	for (int32 i = 0; i < CurrentWaveData.SpawnCount; ++i)
	{
		SpawnMonsters();
	}


	// 로그
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("SWave %d Start | Time: %.0f | Spawn: %d"),
		CurrentWaveIndex + 1,
		CurrentWaveData.Duration,
		CurrentWaveData.SpawnCount
	);


	//화면 표시
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Green,
			FString::Printf(TEXT("Wave %d Start | Time: %.0f | Spawn: %d"),
				CurrentWaveIndex + 1,
				CurrentWaveData.Duration,
				CurrentWaveData.SpawnCount)
		);
	}

	//타이머
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&AWorldGameModeBase::UpdateWaveTimer,
		1.0f,
		true
	);
}


void AWorldGameModeBase::UpdateWaveTimer()
{
	AMonsterWaveGameStateBase* MWGameState = GetGameState<AMonsterWaveGameStateBase>();
	if (!MWGameState)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("GameState is not valid.")
		);
		return;
	}

	float NewTime = MWGameState->GetRemainingTime() - 1.0f;

	MWGameState->SetRemainingTime(NewTime);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Wave %d | Remaining Time: %.0f"),
		CurrentWaveIndex + 1,
		NewTime
	);

	

	//timeup
	if (NewTime <= 0.0f)
	{
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);
		EndWave();
	}
}


void AWorldGameModeBase::EndWave()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Wave %d End"),
		CurrentWaveIndex + 1
	);

	//다음 웨이브로 이동
	CurrentWaveIndex++;

	if (WaveDatas.IsValidIndex(CurrentWaveIndex))
	{
		StartWave();
	}
	else
	{
		CompleteLevel();
	}
}



void AWorldGameModeBase::CompleteLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Level Complete!")
	);


	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			TEXT("Level Complete!")
		);
	}
}


void AWorldGameModeBase::SpawnMonsters()
{
	if (!MonsterClass)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("MonsterClass is not set.")
		);
		return;
	}

	FVector SpawnLocation(0.0f, 0.0f, 100.0f); // 예시 위치, 필요에 따라 조정
	FRotator SpawnRotation = FRotator::ZeroRotator;


	GetWorld()->SpawnActor<AMonster>(MonsterClass, SpawnLocation, SpawnRotation);
}