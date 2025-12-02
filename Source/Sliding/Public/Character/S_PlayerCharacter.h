// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S_BaseCharacter.h"
#include "S_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SLIDING_API AS_PlayerCharacter : public AS_BaseCharacter
{
	GENERATED_BODY()

	

public:
	AS_PlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement") class US_PlayerMovementComponent* PlayerMovementComponent;
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
};
