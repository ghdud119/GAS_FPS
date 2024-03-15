// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASDemoCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GASDemoAbilitySystemComponent.h"
#include "GAS/Attribute/GASDemoAttributeSetBase.h"
#include "GAS/Abilities/GASDemoGameplayAbility.h"


// Sets default values
AGASDemoCharacterBase::AGASDemoCharacterBase(const class FObjectInitializer& ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//충돌체 설정
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);

	//ASC생성 및 초기화, 레플리케이트 설정
	AbilitySystemComponent = CreateDefaultSubobject<UGASDemoAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	//AttributeSet 생성
	AttributeSetBase = CreateDefaultSubobject<UGASDemoAttributeSetBase>(TEXT("AttributeSetBase"));

	//네트워크 가시성 설정(항상 네트워크에서 관리되게 설정)
	bAlwaysRelevant = true;

	//기본 Tag 초기화
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));
}

// Called when the game starts or when spawned
void AGASDemoCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	
	if(AbilitySystemComponent)
	{
		//정의한 어트리뷰트 변경 델리게이트를 ASC델리게이트에 부착
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetLevelAttribute()).AddUObject(this, &ThisClass::CharacterLevelChanged);

		//정의한 태그 변경 델리게이트 이벤트를 ASC이벤트에 부착
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::StunTagChanged);
	}
}

// Called every frame
void AGASDemoCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGASDemoCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AGASDemoCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool AGASDemoCharacterBase::IsAlive() const
{
	return GetHealth() > 0.f;
}

int32 AGASDemoCharacterBase::GetAbilityLevel(GASDemoAbilityID AbilityID) const
{
	return 1;
}

void AGASDemoCharacterBase::RemoveCharacterAbilities()
{
	//권한 및 유효성 확인
	if(GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	//삭제 목록 생성
	TArray<FGameplayAbilitySpecHandle> AbilitiesTORemove;

	//캐릭터 내부에 활성화가능한 어빌리티들 수집
	for(const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesTORemove.Add(Spec.Handle);
		}
	}

	//수집한 어빌리티들 삭제
	for(int32 i = 0; i < AbilitiesTORemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility((AbilitiesTORemove[i]));
	}

	//어빌리티 보유여부 최신화
	AbilitySystemComponent->CharacterAbilitiesGiven = false;
}

void AGASDemoCharacterBase::Die()
{
	RemoveCharacterAbilities();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->Velocity = FVector(0.f);

	//캐릭터 사망 방송
	OnCharacterDied.Broadcast((this));

	//모든 태그 삭제 및 사망태그 부착
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities();
		FGameplayTagContainer EffectsTagsToRemove;
		EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);
		AbilitySystemComponent->AddLooseGameplayTag((DeadTag));
	}
	if(DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDiying();
	}
}

void AGASDemoCharacterBase::FinishDiying()
{
	Destroy();
}

int32 AGASDemoCharacterBase::GetCharacterLevel() const
{
	if(AttributeSetBase)
	{
		return AttributeSetBase->GetLevel();
	}
	return 0;
}

float AGASDemoCharacterBase::GetHealth() const
{
	if(AttributeSetBase)
	{
		return AttributeSetBase->GetHealth();
	}
	return 0.f;
}

float AGASDemoCharacterBase::GetMaxHealth() const
{
	if(AttributeSetBase)
	{
		return AttributeSetBase->GetMaxHealth();
	}
	return 0.f;
}

float AGASDemoCharacterBase::GetMana() const
{
	if(AttributeSetBase)
	{
		return AttributeSetBase->GetMana();
	}
	return 0.f;
}

float AGASDemoCharacterBase::GetMaxMana() const
{
	if(AttributeSetBase)
	{
		return AttributeSetBase->GetMaxMana();
	}
	return 0.f;
}

void AGASDemoCharacterBase::AddCharacterAbilities()
{
	//권한 및 유효성 확인
	if(GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	for(TSubclassOf<UGASDemoGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}
	
	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void AGASDemoCharacterBase::InitializeAttributes()
{
	//어빌리티시스템 유효성 검사
	if(AbilitySystemComponent)
		return;

	//초기화용 어트리뷰트 유효성 검사
	if(!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Pleaase fill in the character's blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	//이펙트 컨텍스트 생성 및 적용대상 설정
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	//게임플레이 이펙트 스펙(Specificatioin) 핸들 생성
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);

	//이펙트 스펙을 대상에게 적용
	if(SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AGASDemoCharacterBase::AddStartupEffects()
{
	//권한 및 유효성 확인
	if(GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->StartupEffectApplied)
	{
		return;
	}

	//이펙트 컨텍스트 생성 및 적용대상 설정
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for(TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
		
		if(SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectApplied = true;
}

void AGASDemoCharacterBase::SetHealth(float Health)
{
	if(AttributeSetBase)
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void AGASDemoCharacterBase::SetMana(float Mana)
{
	if(AttributeSetBase)
	{
		AttributeSetBase->SetHealth(Mana);
	}
}

void AGASDemoCharacterBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("Health Changed"));
}

void AGASDemoCharacterBase::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("MaxHealth Changed"));
}

void AGASDemoCharacterBase::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("Mana Changed"));
}

void AGASDemoCharacterBase::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("MaxMana Changed"));
}

void AGASDemoCharacterBase::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("CharacterLeve Changed"));
}

void AGASDemoCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount > 0)
	{
		FGameplayTagContainer AbilityTagToCancel;
		AbilityTagToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagToIgnore;
		AbilityTagToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagToCancel, &AbilityTagToIgnore);
	}
}
