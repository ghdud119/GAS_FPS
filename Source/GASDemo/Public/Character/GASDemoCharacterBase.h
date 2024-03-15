// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GASDemo/GASDemo.h"
#include "GASDemoCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGASDemoCharacterBase*, Character);

UCLASS()
class GASDEMO_API AGASDemoCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	//생성자
	AGASDemoCharacterBase(const class FObjectInitializer& ObjectInitializer);
	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character")
	virtual int32 GetAbilityLevel(GASDemoAbilityID AbilityID) const;
	
	//캐릭터 사망 함수들

	//캐릭터 사망시 델리게이트 함수
	UPROPERTY(BlueprintAssignable, Category = "GASDemo|Character")
	FCharacterDiedDelegate OnCharacterDied;
	//캐릭터가 보유한 모든 어빌리티 삭제 함수
	virtual void RemoveCharacterAbilities();
	//사망시 필요한 행동들
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character")
	virtual void FinishDiying();
	//캐릭터 사망 함수들 종료

	//ASC, AttributeSet Get함수들
	UFUNCTION(BlueprintCallable, Category = "GAS|Character|AbilityComponentSystem")
	class UGASDemoAbilitySystemComponent* GetAbilitySystemComponent()	{return AbilitySystemComponent;}

	UFUNCTION(BlueprintCallable, Category = "GAS|Character|AttributeSet")
	class UGASDemoAttributeSetBase* GetAttributeSetBase()	{return AttributeSetBase;}

	//어트리뷰트 Get
	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character|Attributes")
	int32 GetCharacterLevel() const;
	
	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "GASDemo|Character|Attributes")
	float GetMaxMana() const;
	//어트리뷰트 Get 종료

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void AddCharacterAbilities();
	
	virtual void InitializeAttributes();
	
	virtual void AddStartupEffects();

	//어트리뷰트 델리게이트들
	
	
	TObjectPtr<class UGASDemoAbilitySystemComponent> AbilitySystemComponent;
	TObjectPtr<class UGASDemoAttributeSetBase> AttributeSetBase;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Demo|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Abilities")
	TArray<TSubclassOf<class UGASDemoGameplayAbility>> CharacterAbilities;

	//캐릭터 생성시 어트리뷰트 초기화용
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	//캐릭터 생성 후 시작 어트리뷰트 적용용
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	//어트리뷰트 Set
	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);

	//어트리뷰트 변경 델리게이트
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	//태그 변경 함수
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
private:




};
