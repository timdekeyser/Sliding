// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Player/S_PlayerMovementComponent.h"


void AS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	for (UInputMappingContext* Context : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(Context, 0);
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::Crouch);
	//EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ThisClass::StopCrouching);

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::Sprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::StopSprinting);
}

void AS_PlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return ;

	GetCharacter()->Jump();
}

void AS_PlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return ;

	GetCharacter()->StopJumping();
}

void AS_PlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);
}

void AS_PlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AS_PlayerController::Crouch()
{
	US_PlayerMovementComponent* PlayerMovementComponent = Cast<US_PlayerMovementComponent>(GetCharacter()->GetMovementComponent());

	if (!IsValid(PlayerMovementComponent)) return ;

	PlayerMovementComponent->CrouchPressed();
}

void AS_PlayerController::StopCrouching()
{
	if (!IsValid(GetCharacter())) return ;

	GetCharacter()->UnCrouch();
}

void AS_PlayerController::Sprint()
{
	US_PlayerMovementComponent* PlayerMovementComponent = Cast<US_PlayerMovementComponent>(GetCharacter()->GetMovementComponent());

	if (!IsValid(PlayerMovementComponent)) return ;

	PlayerMovementComponent->SprintPressed();
}

void AS_PlayerController::StopSprinting()
{
	US_PlayerMovementComponent* PlayerMovementComponent = Cast<US_PlayerMovementComponent>(GetCharacter()->GetMovementComponent());

	if (!IsValid(PlayerMovementComponent)) return ;

	PlayerMovementComponent->SprintReleased();
}


