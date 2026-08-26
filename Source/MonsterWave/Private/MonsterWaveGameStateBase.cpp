

#include "MonsterWaveGameStateBase.h"

AMonsterWaveGameStateBase::AMonsterWaveGameStateBase()
{
	CurrentWave = 0;
	RemainingTime = 0.0f;
	Score = 0;
}

void AMonsterWaveGameStateBase::SetCurrentWave(int32 NewWave)
{
	CurrentWave = NewWave;
}

int32 AMonsterWaveGameStateBase::GetCurrentWave() const
{
	return CurrentWave;
}


void AMonsterWaveGameStateBase::SetRemainingTime(float NewTime)
{
	RemainingTime = NewTime;
}


float AMonsterWaveGameStateBase::GetRemainingTime() const
{
	return RemainingTime;
}


void AMonsterWaveGameStateBase::AddScore(int32 Amount)
{
	Score += Amount;
}


int32 AMonsterWaveGameStateBase::GetScore() const
{
	return Score;
}