// LXPlayerCharacter.cpp


#include "LXPlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"

ALXPlayerCharacter::ALXPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	checkf(CapsuleComp, TEXT("CapsuleComp is invalid."));
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);

	USkeletalMeshComponent* MeshComp = GetMesh();
	checkf(MeshComp, TEXT("MeshComp is invalid."));
	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->GravityScale = 1.0f;
	MoveComp->MaxWalkSpeed = 550.f;	
	MoveComp->MinAnalogWalkSpeed = 20.f;	
	MoveComp->MaxAcceleration = 1000.0f;
	MoveComp->BrakingFrictionFactor = 1.0f;
	MoveComp->BrakingFriction = 6.0f;
	MoveComp->GroundFriction = 8.0f;
	MoveComp->BrakingDecelerationWalking = 1000.0f;
	MoveComp->bUseControllerDesiredRotation = false;
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	MoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->SetCrouchedHalfHeight(65.0f);
	WalkSpeed = MoveComp->MaxWalkSpeed;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 275.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = 20.f;
	SpringArmComponent->CameraLagMaxDistance = 30.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ALXPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));
	
		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));
	
		EILPS->AddMappingContext(InputMappingContext, 0);
	}
}

void ALXPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	FVector CurrentAcceleration = MoveComp->GetCurrentAcceleration();
	
	FRotator CurrentAccelerationRotator = UKismetMathLibrary::MakeRotFromX(CurrentAcceleration);
	FRotator ControlRotation = GetControlRotation();
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAccelerationRotator, ControlRotation);

	FRotator DeltaRotationYaw = FRotator(0.0f, ControlRotation.Yaw, 0.0f);
	FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), DeltaRotationYaw, DeltaSeconds, 10.0f);
	SetActorRotation(InterpolatedRotation);
}

void ALXPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);
	
	EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	
	EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::HandleCrouchInput);
	
	EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::HandleSprintInputStarted);
	EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::HandleSprintInputCompleted);
}

void ALXPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{	
	if (Controller)
	{
		const FVector2D Value = InValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			//const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			//const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void ALXPlayerCharacter::HandleLookInput(const FInputActionValue& InValue)
{	
	const FVector2D Value = InValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		AddControllerPitchInput(Value.Y);
	}
}

void ALXPlayerCharacter::HandleCrouchInput(const FInputActionValue& InValue)
{
	const UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void ALXPlayerCharacter::HandleSprintInputStarted(const FInputActionValue& InValue)
{
	UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->MaxWalkSpeed = SprintSpeed;
}

void ALXPlayerCharacter::HandleSprintInputCompleted(const FInputActionValue& InValue)
{
	UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->MaxWalkSpeed = WalkSpeed;
}
