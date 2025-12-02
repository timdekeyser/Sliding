// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S_PlayerCameraManager.h"
#include "Character/S_PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Player/S_PlayerMovementComponent.h"

AS_PlayerCameraManager::AS_PlayerCameraManager()
{
}

void AS_PlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (AS_PlayerCharacter* PlayerCharacter = Cast<AS_PlayerCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		US_PlayerMovementComponent* PMC = PlayerCharacter->GetPlayerMovementComponent();
		FVector TargetCrouchOffset = FVector(0, 0, PMC->GetCrouchedHalfHeight() - PlayerCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime/CrouchBlendDuration, 0.f, 1.f));

		if (PMC->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
		}
		if (PMC->IsMovingOnGround())
		{
			OutVT.POV.Location += Offset;
		}
	}
}
