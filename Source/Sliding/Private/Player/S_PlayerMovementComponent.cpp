// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S_PlayerMovementComponent.h"

#include "Character/S_PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


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

	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

void US_PlayerMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	PlayerCharacterOwner = Cast<AS_PlayerCharacter>(GetOwner());
	
}

void US_PlayerMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (MovementMode == MOVE_Walking && Safe_bPrevWantsToCrouch )
	{
		ACharacter* Character = Cast<ACharacter>(PawnOwner);
		if (!Character)
			return;

		
		FHitResult PotentialSlideSurface;
		if (Velocity.SizeSquared() > pow(Slide_MinSpeed,2) && GetSlideSurface(PotentialSlideSurface))
		{
			EnterSlide();
		}
	}

	if (IsCustomMovementMode(SMOVE_Slide) && !bWantsToCrouch )
	{
		EnterSlide();
	}

	if (MovementMode == MOVE_Custom && CustomMovementMode == SMOVE_Slide)
	{
		ACharacter* Character = Cast<ACharacter>(PawnOwner);
		if (!Character)
			return;
		
		if (Character->bPressedJump)
		{
			ExitSlide();
			SetMovementMode(MOVE_Walking);
			Character->Jump();

			return; 
		}
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void US_PlayerMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case SMOVE_Slide:
		PhysSlide(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

bool US_PlayerMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(SMOVE_Slide);
}

bool US_PlayerMovementComponent::CanCrouchInCurrentState() const
{
	
	if (IsFalling())
	{
		return true;
	}
	
	
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

void US_PlayerMovementComponent::EnterSlide()
{
	UE_LOG(LogTemp, Warning, TEXT("EnterSlide"));
	
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom, SMOVE_Slide);
}

void US_PlayerMovementComponent::ExitSlide()
{
	UE_LOG(LogTemp, Warning, TEXT("ExitSlide"));
	
	bWantsToCrouch = false;

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
	if (CharacterOwner)
	{
		CharacterOwner->UnCrouch();
	}
}

void US_PlayerMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME) return;

	RestorePreAdditiveRootMotionVelocity();

	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		ExitSlide();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	Velocity += Slide_GravityForce * FVector::DownVector * DeltaTime;

	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5f)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime, Slide_Friction, true, GetMaxBrakingDeceleration());
	}

	ApplyRootMotionToVelocity(DeltaTime);

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * DeltaTime * 1.01;
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, DeltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		ExitSlide();
	}

	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}
}

bool US_PlayerMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, PlayerCharacterOwner->GetIgnoreCharacterParams());
}

void US_PlayerMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void US_PlayerMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void US_PlayerMovementComponent::CrouchPressed()
{
	bWantsToCrouch = !bWantsToCrouch;
}

bool US_PlayerMovementComponent::IsSliding()
{
	return bIsSliding;
}

bool US_PlayerMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

US_PlayerMovementComponent::US_PlayerMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
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
void US_PlayerMovementComponent::FSavedMove_Sliding::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	US_PlayerMovementComponent* CharacterMovement = Cast<US_PlayerMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;
}

void US_PlayerMovementComponent::FSavedMove_Sliding::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	US_PlayerMovementComponent* CharacterMovement = Cast<US_PlayerMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
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

void US_PlayerMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}



