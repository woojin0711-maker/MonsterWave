#include "WorldGameModeBase.h"

#include "MonsterWaveGameStateBase.h"
#include "Monster.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

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

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController && HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(PlayerController, HUDWidgetClass);

		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
		else
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("Failed to create HUD widget instance.")
			);
		}
	}
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

	RemainingMonsterSpawnCount = CurrentWaveData.SpawnCount;

	SpawnNextMonster();

	if (RemainingMonsterSpawnCount > 0)
	{
		GetWorldTimerManager().SetTimer(
			MonsterSpawnTimerHandle,
			this,
			&AWorldGameModeBase::SpawnNextMonster,
			SpawnInterval,
			true
		);
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
	GetWorldTimerManager().ClearTimer(MonsterSpawnTimerHandle);
	RemainingMonsterSpawnCount = 0;

	ClearSpawnedMonsters();

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

void AWorldGameModeBase::SpawnNextMonster()
{
	if (RemainingMonsterSpawnCount <= 0)
	{
		GetWorldTimerManager().ClearTimer(MonsterSpawnTimerHandle);
		return;
	}
	SpawnMonsters();
	RemainingMonsterSpawnCount--;

	if (RemainingMonsterSpawnCount <= 0)
	{
		GetWorldTimerManager().ClearTimer(MonsterSpawnTimerHandle);
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


	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(),
		TEXT("MonsterSpawn"),
		SpawnPoints
	);

	if (SpawnPoints.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("No actor with the 'MonsterSpawn' tag was found.")
		);
		return;
	}

	AActor* SelectedSpawnPoint = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
	if (!IsValid(SelectedSpawnPoint))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector2D RandomOffset = FMath::RandPointInCircle(SpawnRadius);

	const FVector SpawnLocation = SelectedSpawnPoint->GetActorLocation() + FVector(RandomOffset.X, RandomOffset.Y, 0.0f);

	AMonster* SpawnedMonster = GetWorld()->SpawnActor<AMonster>(
		MonsterClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);



	if (IsValid(SpawnedMonster))
	{
		SpawnedMonsters.Emplace(SpawnedMonster);
	}
}


void AWorldGameModeBase::ClearSpawnedMonsters()
{
	for (TWeakObjectPtr<AMonster> MonsterPtr : SpawnedMonsters)
	{
		if (AMonster* Monster = MonsterPtr.Get())
		{
			Monster->Destroy();
		}
	}
	SpawnedMonsters.Empty();
}

void AWorldGameModeBase::ShowGameOver()
{
	if (GameOverWidgetInstance)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(MonsterSpawnTimerHandle);

	RemainingMonsterSpawnCount = 0;

	ClearSpawnedMonsters();

	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController || !GameOverWidgetClass)
	{
		return;
	}

	GameOverWidgetInstance = CreateWidget<UUserWidget>(PlayerController, GameOverWidgetClass);
	if (!GameOverWidgetInstance)
	{
		return;
	}

	GameOverWidgetInstance->AddToViewport(10);

	FInputModeUIOnly InputMode;

	InputMode.SetWidgetToFocus(GameOverWidgetInstance->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void AWorldGameModeBase::RestartCurrentLevel()
{
	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->RemoveFromParent();
		GameOverWidgetInstance = nullptr;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
	}

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
}

void AWorldGameModeBase::NotifyMonsterDied(AMonster* DeadMonster)
{
	SpawnedMonsters.RemoveAll(
		[DeadMonster](const TWeakObjectPtr<AMonster>& MonsterPtr)
		{
			return !MonsterPtr.IsValid() || MonsterPtr.Get() == DeadMonster;
		}
	);

	// 아직 생성될 몬스터가 남았으면 기다린다.
	if (RemainingMonsterSpawnCount > 0)
	{
		return;
	}

	// 살아 있는 몬스터가 남았으면 기다린다.
	if (!SpawnedMonsters.IsEmpty())
	{
		return;
	}

	AMonsterWaveGameStateBase* MWGameState =
		GetGameState<AMonsterWaveGameStateBase>();

	if (MWGameState && MWGameState->GetRemainingTime() > 5.0f)
	{
		MWGameState->SetRemainingTime(5.0f);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("All monsters defeated. Remaining time changed to 5 seconds.")
		);
	}
}