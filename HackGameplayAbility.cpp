#include "HackGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"

UHackGameplayAbility::UHackGameplayAbility()
{
	// Define ability tags
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Active")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Finisher")));
}

// Get the cooldown rate from the owning Ability System Component
float UHackGameplayAbility::GetCooldownRate() const
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return 1.f;

	return ASC->GetNumericAttribute(UBasicAttributeSet::GetCooldownRateAttribute());
}

// Calculate the final cooldown duration after applying the cooldown rate
float UHackGameplayAbility::CalculateCooldown() const
{
	const float Rate = GetCooldownRate();
	
	if (Rate <= KINDA_SMALL_NUMBER) return BaseCooldown; // Avoid division by zero or negative rates
	
	return BaseCooldown / Rate;
}

// Apply the cooldown effect to the ability's owner
void UHackGameplayAbility::ApplyCooldown()
{
	if (!CooldownGameplayEffect) // No cooldown effect assigned, nothing to apply
	{
		UE_LOG(LogTemp, Warning, TEXT("Cooldown Gameplay Effect not set on '%s'."), *GetName());
		return;
	} 
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	
	// Calculate the final cooldown after attribute scaling
	const float FinalCooldown = CalculateCooldown();
	
	// Build a gameplay effect spec from the cooldown effect
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffect);
	
	if (!SpecHandle.IsValid()) return; // Stop if the spec could not be created
	
	// Store the cooldown duration in the spec for the effect to read
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		FGameplayTag::RequestGameplayTag(TEXT("Data.Cooldown.Duration")),
		FinalCooldown);
	
	// Apply the cooldown effect to this ability's owner
	(void)ApplyGameplayEffectSpecToOwner(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		SpecHandle);
}

// Check if the avatar actor has a player controller
bool UHackGameplayAbility::HasPC() const
{
	const APawn* PawnObject = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!PawnObject) return false;

	return PawnObject->GetController()->IsA<APlayerController>();
}