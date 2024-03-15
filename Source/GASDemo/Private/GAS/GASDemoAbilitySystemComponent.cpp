// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GASDemoAbilitySystemComponent.h"

void UGASDemoAbilitySystemComponent::ReceiveDamage(ThisClass* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceviedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
