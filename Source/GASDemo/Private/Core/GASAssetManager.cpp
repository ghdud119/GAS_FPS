// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GASAssetManager.h"
#include "AbilitySystemGlobals.h"

void UGASAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
