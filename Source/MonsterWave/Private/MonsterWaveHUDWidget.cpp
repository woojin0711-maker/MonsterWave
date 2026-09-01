// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterWaveHUDWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "MonsterWaveGameStateBase.h"
#include "Warrior.h"

void UMonsterWaveHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	AMonsterWaveGameStateBase* MWGameState = GetWorld()->GetGameState<AMonsterWaveGameStateBase>();

	if (MWGameState)
	{
		// Update wave text
		if (WaveText)
		{
			WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), MWGameState->GetCurrentWave())));
		}
		// Update timer text
		if (TimerText)
		{
			TimerText->SetText(FText::FromString(FString::Printf(TEXT("Time: %d"), FMath::CeilToInt(MWGameState->GetRemainingTime()))));
		}
		// Update score text
		if (ScoreText)
		{
			ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), MWGameState->GetScore())));
		}
	}

	// Update health bar and text
	AWarrior* PlayerCharacter = Cast<AWarrior>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		const float CurrentHealth = PlayerCharacter->GetCurrentHealth();
		const float MaxHealth = PlayerCharacter->GetMaxHealth();

		if (HealthText)
		{
			HealthText->SetText(FText::FromString(FString::Printf(TEXT("HP %.0f / %.0f"), CurrentHealth, MaxHealth)));

			const float HealthPercent = MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.0f;

			if (HealthBar)
			{
				HealthBar->SetPercent(HealthPercent);
			}
		}
	}
}

