// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S_PlayerMovementComponent.h"
#include "GameFramework/Character.h"


bool US_PlayerMovementComponent::FSavedMove_Sliding::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Sliding* NewSlidingMove = static_cast<FSavedMove_Sliding*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewSlidingMove->Saved_bWantsToSprint)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void US_PlayerMovementComponent::FSavedMove_Sliding::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = 0;
}

uint8 US_PlayerMovementComponent::FSavedMove_Sliding::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
	
}

void US_PlayerMovementComponent::FSavedMove_Sliding::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	US_PlayerMovementComponent* CharacterMovement = Cast<US_PlayerMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
}

void US_PlayerMovementComponent::FSavedMove_Sliding::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	US_PlayerMovementComponent* CharacterMovement = Cast<US_PlayerMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
}

US_PlayerMovementComponent::FNetworkPredictionData_Client_Sliding::FNetworkPredictionData_Client_Sliding(const UCharacterMovementComponent& ClientMovement)
	:FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr US_PlayerMovementComponent::FNetworkPredictionData_Client_Sliding::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Sliding());
}

FNetworkPredictionData_Client* US_PlayerMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

	if (ClientPredictionData == nullptr)
	{
		US_PlayerMovementComponent* MutableThis = const_cast<US_PlayerMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Sliding(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void US_PlayerMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void US_PlayerMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void US_PlayerMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void US_PlayerMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToSprint)
		{
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = Walk_MaxWalkSpeed;
		}
	}
}

US_PlayerMovementComponent::US_PlayerMovementComponent()
{
	
}



