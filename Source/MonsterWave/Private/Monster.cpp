
#include "Monster.h"
#include "MonsterAIController.h"
#include "MonsterWaveGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "WorldGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMonsterAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
}


void AMonster::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(
			this,
			&AMonster::HandleMontageNotifyBegin
		);
	}
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsDead)
	{
		TryAttackPlayer();
	}
}

float AMonster::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator, 
	AActor* DamageCauser
)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Monster took damage: %.2f, Current Health: %.2f, Max Health: %.2f"),
		ActualDamage,
		CurrentHealth,
		MaxHealth
	);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	return ActualDamage;
}

void AMonster::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	AMonsterWaveGameStateBase* MWGameState = GetWorld()->GetGameState<AMonsterWaveGameStateBase>();

	if (MWGameState)
	{
		MWGameState->AddScore(ScoreReward);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Monster died. Score rewarded: %d"),
			MWGameState->GetScore()
		);
	}

	if (AWorldGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AWorldGameModeBase>())
	{
		GameMode->NotifyMonsterDied(this);
	}

	GetCharacterMovement()->DisableMovement();
	DetachFromControllerPendingDestroy();

	// 캡슐 충돌만 제거
	GetCapsuleComponent()->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	// 스켈레탈 메시를 래그돌로 전환
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics
	);
	GetMesh()->SetSimulatePhysics(true);

	// 살짝 튕겨 나가게 함
	GetMesh()->AddImpulse(
		-GetActorForwardVector() * 300.0f
		+ FVector::UpVector * 150.0f,
		NAME_None,
		true
	);

	// 4초 후 제거
	SetLifeSpan(4.0f);
}

void AMonster::TryAttackPlayer()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	const float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

	if (DistanceToPlayer > AttackRange)
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastAttackTime < AttackInterval)
	{
		return;
	}

	LastAttackTime = CurrentTime;

	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
	else
	{
		ApplyAttackDamage();
	}
}

void AMonster::HandleMontageNotifyBegin(
	FName NotifyName,
	const FBranchingPointNotifyPayload& BranchingPointPayload
)
{
	if (NotifyName == TEXT("AttackHit"))
	{
		ApplyAttackDamage();
	}
}

void AMonster::ApplyAttackDamage()
{
	if (bIsDead)
	{
		return;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	const float DistanceToPlayer = FVector::Dist(
		GetActorLocation(),
		PlayerCharacter->GetActorLocation()
	);

	if (DistanceToPlayer > AttackRange)
	{
		return;
	}
	
	UGameplayStatics::ApplyDamage(
		PlayerCharacter,
		AttackDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
}
