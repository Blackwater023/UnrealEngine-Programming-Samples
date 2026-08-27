#include "Public/Attributes/Core/ZRB_CoreAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UZRB_CoreAttributeSet::UZRB_CoreAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitMana(100.f);
	InitMaxMana(100.f);
	InitResonance(0.f);
	InitMaxResonance(100.f);
	InitPendingDamage(0.f);
}

void UZRB_CoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UZRB_CoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZRB_CoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZRB_CoreAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZRB_CoreAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZRB_CoreAttributeSet, Resonance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZRB_CoreAttributeSet, MaxResonance, COND_None, REPNOTIFY_Always);
}

void UZRB_CoreAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	// Clamp current values to their maximums.
	// NOTE: This clamps the *modifier query* result, not the base value.
	// Always re-clamp in PostGameplayEffectExecute too.
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetResonanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxResonance());
	}
}

void UZRB_CoreAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetPendingDamageAttribute())
	{
		// PendingDamage is a meta attribute - grab the value, then zero it out.
		const float DamageAmount = GetPendingDamage();
		SetPendingDamage(0.f);
		
		if (DamageAmount > 0.f)
		{
			// Apply damage to Health.
			// NOTE: Check resistances here
			const float NewHealth = FMath::Clamp(GetHealth() - DamageAmount, 0.f, GetMaxHealth());
			SetHealth(NewHealth);
			
			// Extension Point: check for death (NewHealth <= 0) and broadcast a gameplay event or tag.
		}
	}
	
	// Final safety clamp - always keep attributes within bounds.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetResonanceAttribute())
	{
		SetResonance(FMath::Clamp(GetResonance(), 0.f, GetMaxResonance()));
	}
}

void UZRB_CoreAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_CoreAttributeSet, Health, OldHealth);
}

void UZRB_CoreAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_CoreAttributeSet, MaxHealth, OldMaxHealth);
}

void UZRB_CoreAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_CoreAttributeSet, Mana, OldMana);
}

void UZRB_CoreAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_CoreAttributeSet, MaxMana, OldMaxMana);
}

void UZRB_CoreAttributeSet::OnRep_Resonance(const FGameplayAttributeData& OldResonance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_CoreAttributeSet, Resonance, OldResonance);
}

void UZRB_CoreAttributeSet::OnRep_MaxResonance(const FGameplayAttributeData& OldMaxResonance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_CoreAttributeSet, MaxResonance, OldMaxResonance);
}
