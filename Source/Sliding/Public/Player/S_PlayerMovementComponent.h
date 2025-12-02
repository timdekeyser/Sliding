// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S_PlayerMovementComponent.generated.h"

class AS_PlayerCharacter;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	SMOVE_NONE		UMETA(Hiddent),
	SMOVE_Slide		UMETA(DisplayName =  "Slide"),
	SMOVE_MAX		UMETA(Hidden),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLIDING_API US_PlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_Sliding : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 Saved_bWantsToSprint:1;

		uint8 Saved_bPrevWantsToCrouch:1;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_Sliding : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Sliding(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Sliding Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

	UPROPERTY(EditDefaultsOnly) float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float Walk_MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly) float Slide_MinSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly) float Slide_EnterImpulse = 500.f;
	UPROPERTY(EditDefaultsOnly) float Slide_GravityForce = 5000.f;
	UPROPERTY(EditDefaultsOnly) float Slide_Friction = 1.3f;
	
	bool Safe_bWantsToSprint;
	bool Safe_bPrevWantsToCrouch;
	
	
	UPROPERTY(Transient) AS_PlayerCharacter* PlayerCharacterOwner;
	

public:
	US_PlayerMovementComponent();

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
	void SprintPressed();
	void SprintReleased();
	void CrouchPressed();
	UFUNCTION(BlueprintCallable) bool IsSliding();
	bool bIsSliding;

	bool IsCustomMovementMode (ECustomMovementMode InCustomMovementMode) const;

	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void InitializeComponent() override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	

private:
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float DeltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
	
};
