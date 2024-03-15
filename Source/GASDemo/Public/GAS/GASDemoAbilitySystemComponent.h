// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "GASDemoAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceiveDamageDelegate, UGASDemoAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);
/**
 * 
 */
UCLASS()
class GASDEMO_API UGASDemoAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool CharacterAbilitiesGiven = false;
	bool StartupEffectApplied = false;

	UPROPERTY(BlueprintAssignable)
	FReceiveDamageDelegate ReceviedDamage;
	
	virtual void ReceiveDamage(ThisClass* SourceASC, float UnmitigatedDamage, float MitigatedDamage);
};
