#include "Public/Attributes/Stats/ZRB_StatAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UZRB_StatAttributeSet::UZRB_StatAttributeSet()
{
	InitStrength(1.f);
	InitMaxStrength(99.f);
	InitIntelligence(1.f);
	InitMaxIntelligence(99.f);
	InitAgility(1.f);
	InitMaxAgility(99.f);
	InitLuck(1.f);
	InitMaxLuck(99.f);
	InitResolve(1.f);
	InitMaxResolve(99.f);
}

void UZRB_StatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UZRB_StatAttributeSet, Strength);
	DOREPLIFETIME(UZRB_StatAttributeSet, MaxStrength);
	DOREPLIFETIME(UZRB_StatAttributeSet, Intelligence);
	DOREPLIFETIME(UZRB_StatAttributeSet, MaxIntelligence);
	DOREPLIFETIME(UZRB_StatAttributeSet, Agility);
	DOREPLIFETIME(UZRB_StatAttributeSet, MaxAgility);
	DOREPLIFETIME(UZRB_StatAttributeSet, Luck);
	DOREPLIFETIME(UZRB_StatAttributeSet, MaxLuck);
	DOREPLIFETIME(UZRB_StatAttributeSet, Resolve);
	DOREPLIFETIME(UZRB_StatAttributeSet, MaxResolve);
}

void UZRB_StatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	// Clamp current values to their maximums.
	// NOTE: This clamps the *modifier query* result, not the base value.
	// Always re-clamp in PostGameplayEffectExecute too.
	if (Attribute == GetStrengthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, GetMaxStrength());
	}
	else if (Attribute == GetIntelligenceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, GetMaxIntelligence());
	}
	else if (Attribute == GetAgilityAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, GetMaxAgility());	
	}
	else if (Attribute == GetLuckAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, GetMaxLuck());
	}
	else if (Attribute == GetResolveAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, GetMaxResolve());
	}
}

void UZRB_StatAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Final safety clamp - always keep attributes within bounds.
	if (Data.EvaluatedData.Attribute == GetStrengthAttribute())
	{
		SetStrength(FMath::Clamp(GetStrength(), 1.f, GetMaxStrength()));
	}
	else if (Data.EvaluatedData.Attribute == GetIntelligenceAttribute())
	{
		SetIntelligence(FMath::Clamp(GetIntelligence(), 1.f, GetMaxIntelligence()));
	}
	else if (Data.EvaluatedData.Attribute == GetAgilityAttribute())
	{
		SetAgility(FMath::Clamp(GetAgility(), 1.f, GetMaxAgility()));
	}
	else if (Data.EvaluatedData.Attribute == GetLuckAttribute())
	{
		SetLuck(FMath::Clamp(GetLuck(), 1.f, GetMaxLuck()));
	}
	else if (Data.EvaluatedData.Attribute == GetResolveAttribute())
	{
		SetResolve(FMath::Clamp(GetResolve(), 1.f, GetMaxResolve()));
	}
}

void UZRB_StatAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, Strength, OldStrength);
}

void UZRB_StatAttributeSet::OnRep_MaxStrength(const FGameplayAttributeData& OldMaxStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, MaxStrength, OldMaxStrength);
}

void UZRB_StatAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, Intelligence, OldIntelligence);
}

void UZRB_StatAttributeSet::OnRep_MaxIntelligence(const FGameplayAttributeData& OldMaxIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, MaxIntelligence, OldMaxIntelligence);
}

void UZRB_StatAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldAgility)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, Agility, OldAgility);
}

void UZRB_StatAttributeSet::OnRep_MaxAgility(const FGameplayAttributeData& OldMaxAgility)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, MaxAgility, OldMaxAgility);
}

void UZRB_StatAttributeSet::OnRep_Luck(const FGameplayAttributeData& OldLuck)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, Luck, OldLuck);
}

void UZRB_StatAttributeSet::OnRep_MaxLuck(const FGameplayAttributeData& OldMaxLuck)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, MaxLuck, OldMaxLuck);
}

void UZRB_StatAttributeSet::OnRep_Resolve(const FGameplayAttributeData& OldResolve)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, Resolve, OldResolve);
}

void UZRB_StatAttributeSet::OnRep_MaxResolve(const FGameplayAttributeData& OldMaxResolve)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZRB_StatAttributeSet, MaxResolve, OldMaxResolve);
}