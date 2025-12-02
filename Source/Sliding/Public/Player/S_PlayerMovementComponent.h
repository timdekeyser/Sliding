// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S_PlayerMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLIDING_API US_PlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	bool Safe_bWantsToSprint;

public:
	US_PlayerMovementComponent();
	
};
