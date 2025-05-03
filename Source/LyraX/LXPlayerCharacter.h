// LXPlayerCharacter.h

#pragma once

#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "LXPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class LYRAX_API ALXPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region ACharacter Override
	
public:
	ALXPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LXPlayerCharacter|Component")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LXPlayerCharacter|Component")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

#pragma endregion
	
#pragma region Input

private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);
	
	void HandleCrouchInput(const FInputActionValue& InValue);
	
	void HandleSprintInputStarted(const FInputActionValue& InValue);
	
	void HandleSprintInputCompleted(const FInputActionValue& InValue);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	TObjectPtr<UInputAction> CrouchAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	float WalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LXPlayerCharacter|Input")
	float SprintSpeed;
	
#pragma endregion

};
