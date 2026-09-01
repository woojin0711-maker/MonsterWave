
#include "Warrior.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "WorldGameModeBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/DamageType.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Monster.h"
#include "AIController.h"

AWarrior::AWarrior()
{

	PrimaryActorTick.bCanEverTick = true;

	CurrentHealth = MaxHealth;
	bIsDead = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	TargetCameraDistance = CameraBoom->TargetArmLength;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}


void AWarrior::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	CurrentHealth = MaxHealth;
	bIsDead = false;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController && InputMappingContext)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem && InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(
			this,
			&AWarrior::HandleMontageNotifyBegin
		);
	}
}


void AWarrior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength, 
			TargetCameraDistance, 
			DeltaTime, 
			ZoomInterpSpeed
		);
	}
}

void AWarrior::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInput && MoveAction)
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(
				MoveAction, 
				ETriggerEvent::Triggered, 
				this, 
				&AWarrior::Move);
		}
	}

	if (EnhancedInput && AttackAction)
	{
		EnhancedInput->BindAction(
			AttackAction,
			ETriggerEvent::Started,
			this,
			&AWarrior::Attack);
	}

	if (EnhancedInput && LookAction)
	{
		EnhancedInput->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&AWarrior::Look);
	}

	if (EnhancedInput && JumpAction)
	{
		EnhancedInput->BindAction(
			JumpAction,
			ETriggerEvent::Started,
			this,
			&AWarrior::StartJump);

		EnhancedInput->BindAction(
			JumpAction,
			ETriggerEvent::Completed,
			this,
			&AWarrior::StopJump);

		EnhancedInput->BindAction(
			JumpAction,
			ETriggerEvent::Canceled,
			this,
			&AWarrior::StopJump);
	}

	if (EnhancedInput && ZoomAction)
	{
		EnhancedInput->BindAction(
			ZoomAction,
			ETriggerEvent::Triggered,
			this,
			&AWarrior::Zoom);
	}

	if (EnhancedInput && SprintAction)
	{
		EnhancedInput->BindAction(
			SprintAction,
			ETriggerEvent::Started,
			this,
			&AWarrior::StartSprint);

		EnhancedInput->BindAction(
			SprintAction,
			ETriggerEvent::Completed,
			this,
			&AWarrior::StopSprint);

		EnhancedInput->BindAction(
			SprintAction,
			ETriggerEvent::Canceled,
			this,
			&AWarrior::StopSprint);
	}
}


void AWarrior::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AWarrior::ApplyAttackHit()
{
	const FVector Start = GetActorLocation() + GetActorForwardVector() * 50.0f;
	const FVector End = Start + (GetActorForwardVector() * AttackRange);

	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const FCollisionShape AttackShape = FCollisionShape::MakeSphere(AttackRadius);
	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		AttackShape,
		QueryParams
	);

	const FColor DebugColor = bHit ? FColor::Red : FColor::Green;

	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		DebugColor,
		false,
		1.0f,
		0,
		3.0f
	);

	DrawDebugSphere(
		GetWorld(),
		End,
		AttackRadius,
		16,
		DebugColor,
		false,
		1.0f
	);

	if (bHit && AttackEffect)
	{
		const FVector EffectLocation = bHit ? HitResult.ImpactPoint : End;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			AttackEffect,
			EffectLocation,
			GetActorRotation()
		);
	}


	if (!bHit || !IsValid(HitResult.GetActor()))
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	if (AMonster* HitMonster = Cast<AMonster>(HitActor))
	{
		if (AAIController* MonsterController =
			Cast<AAIController>(HitMonster->GetController()))
		{
			MonsterController->StopMovement();
		}

		const FVector KnockbackDirection =
			(HitMonster->GetActorLocation() - GetActorLocation())
			.GetSafeNormal2D();

		const FVector LaunchVelocity =
			KnockbackDirection * KnockbackStrength
			+ FVector::UpVector * KnockbackUpwardForce;

		HitMonster->LaunchCharacter(
			LaunchVelocity,
			true,
			true
		);
	}

	UGameplayStatics::ApplyDamage(
		HitResult.GetActor(),
		AttackDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
}

void AWarrior::Attack()
{
	if (bIsDead)
	{
		return;
	}

	if (SlashEffect)
	{
		const FVector SlashLocation =
			GetActorLocation()
			+ GetActorForwardVector() * 140.0f
			+ FVector::UpVector * 70.0f;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			SlashEffect,
			SlashLocation,
			GetActorRotation()
		);
	}

	ApplyAttackHit();
}

float AWarrior::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
		TEXT("Warrior took damage: %.2f, Current Health: %.2f, Max Health: %.2f"),
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

void AWarrior::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	GetCharacterMovement()->DisableMovement();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Warrior died.")
	);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Red,
			TEXT("You Died!")
		);
	}
	
	AWorldGameModeBase* GameMode = Cast<AWorldGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		GameMode->ShowGameOver();
	}
}

void AWarrior::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
}

void AWarrior::Zoom(const FInputActionValue& Value)
{
	const float ZoomAxisValue = Value.Get<float>();
	TargetCameraDistance = FMath::Clamp(
		TargetCameraDistance - (ZoomAxisValue * ZoomStep),
		MinCameraDistance,
		MaxCameraDistance
	);
	const float CurrentArmLength = CameraBoom->TargetArmLength;
	const float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetCameraDistance, GetWorld()->GetDeltaSeconds(), ZoomInterpSpeed);
	CameraBoom->TargetArmLength = NewArmLength;
}

void AWarrior::StartJump()
{
	if (bIsDead)
	{
		return;
	}
	Jump();
}

void AWarrior::StopJump()
{
	if (bIsDead)
	{
		return;
	}
	StopJumping();
}

void AWarrior::StartSprint()
{
	if (bIsDead)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AWarrior::StopSprint()
{
	if (bIsDead)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AWarrior::HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "AttackHit")
	{
		ApplyAttackHit();
	}
}