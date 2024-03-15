// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASDemo/GASDemo.h"
#include "GASDemoGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGASDemoGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGASDemoGameplayAbility();
	
	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	GASDemoAbilityID AbilityInputID = GASDemoAbilityID::None;
		// Value to associate an ability with an slot without tying it to an automatically activated input.
	// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	GASDemoAbilityID AbilityID = GASDemoAbilityID::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivaeAbilityOnGranted = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
