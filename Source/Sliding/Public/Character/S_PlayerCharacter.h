// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S_BaseCharacter.h"
#include "S_PlayerCharacter.generated.h"


class US_PlayerMovementComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SLIDING_API AS_PlayerCharacter : public AS_BaseCharacter
{
	GENERATED_BODY()

	

public:
	AS_PlayerCharacter(const FObjectInitializer& ObjectInitializer);

	US_PlayerMovementComponent* GetPlayerMovementComponent() const;
	
	FCollisionQueryParams GetIgnoreCharacterParams();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement") class US_PlayerMovementComponent* PlayerMovementComponent;
	
	virtual bool CanCrouch() const override;
	
	
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	
};
