
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"


class UAnimMontage;
struct FBranchingPointNotifyPayload;




UCLASS()
class MONSTERWAVE_API AMonster : public ACharacter
{
	GENERATED_BODY()
	
public:	
	AMonster();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Score")
	int32 ScoreReward = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Health")
	bool bIsDead = false;

	void Die();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Attack")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Attack")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Attack")
	float AttackInterval = 1.0f;

	float LastAttackTime = -1.0f;

	void TryAttackPlayer();
	void ApplyAttackDamage();

	UFUNCTION()
	void HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Animation")
	TObjectPtr<UAnimMontage> DeathMontage;


public:
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
