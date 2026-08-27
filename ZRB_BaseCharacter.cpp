#include "Public/BaseCharacter/ZRB_BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameplayAbility/ZRB_GameplayAbility.h"
#include "Public/Attributes/Core/ZRB_CoreAttributeSet.h"
#include "Public/Attributes/Stats/ZRB_StatAttributeSet.h"
#include "Public/Attributes/Combat/ZRB_CombatAttributeSet.h"

AZRB_BaseCharacter::AZRB_BaseCharacter()
{
	// Create the ASC and Attribute Set as default subobjects.
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	CoreAttributeSet = CreateDefaultSubobject<UZRB_CoreAttributeSet>(TEXT("CoreAttributeSet"));
	StatAttributeSet = CreateDefaultSubobject<UZRB_StatAttributeSet>(TEXT("StatAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UZRB_CombatAttributeSet>(TEXT("CombatAttributeSet"));
}

UAbilitySystemComponent* AZRB_BaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AZRB_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Test health initialization. In a real game, this would be handled by a GE.
	//InitializeTestHealth();
}

void AZRB_BaseCharacter::Move(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AZRB_BaseCharacter::Look(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AZRB_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when an input action is pressed. Notifies the ASC that the input was pressed and attempts to activate the ability.
void AZRB_BaseCharacter::OnAbilityInputPressed(FGameplayTag InputTag)
{
	if (!AbilitySystemComponent) return;
	
	UE_LOG(LogTemp, Warning, TEXT("INPUT PRESSED: %s"), *InputTag.ToString());
	
	// Iterate through all activatable abilities and check if their source tags match the input tag.
	for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("FOUND MATCHING ABILITY"));
			// Notify the spec that its input was pressed (critical for ability tasks)
			AbilitySystemComponent->AbilitySpecInputPressed(Spec);
			
			if (!Spec.IsActive())
			{
				UE_LOG(LogTemp, Warning, TEXT("TRYING TO ACTIVATE"));
				AbilitySystemComponent->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

// Called when an input action is released. Notifies the ASC that the input was released.
void AZRB_BaseCharacter::OnAbilityInputReleased(FGameplayTag InputTag)
{
	if (!AbilitySystemComponent) return;
	
	// Iterate through all activatable abilities and check if their source tags match the input tag.
	for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySystemComponent->AbilitySpecInputReleased(Spec);
		}
	}
}

// Bind input actions to the corresponding ability input tags.
void AZRB_BaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent CALLED"));
	
	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	// Bind each input action to the corresponding ability input tag.
	for (const FAbilityInputAction& Entry : AbilityInputActions)
	{
		if (Entry.InputAction)
		{
			EIC->BindAction(Entry.InputAction, ETriggerEvent::Started, this, &ThisClass::OnAbilityInputPressed, Entry.InputTag);
			
			EIC->BindAction(Entry.InputAction, ETriggerEvent::Completed, this, &ThisClass::OnAbilityInputReleased, Entry.InputTag);
		}
	}
}

// Server: initialize the ASC's actor info so it knows who owns it.
void AZRB_BaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// Server: initialize the ASC's actor info so it knows who owns it.
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAbilities();
	}
}

// Client: re-initialize the ASC's actor info so when the PlayerState replicates.
void AZRB_BaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// Client: re-initialize the ASC's actor info so when the PlayerState replicates.
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void AZRB_BaseCharacter::InitializeTestHealth()
{
	if (!StatAttributeSet || !CoreAttributeSet) return;

	constexpr float TestStrength = 10.f;
	constexpr float BaseHealth = 100.f;
	constexpr float HealthPerStrength = 10.f;

	// Temporary test value
	StatAttributeSet->SetStrength(TestStrength);

	// Calculate MaxHealth based on Strength and set it in the CoreAttributeSet.
	const float CalculatedMaxHealth = BaseHealth + (StatAttributeSet->GetStrength() * HealthPerStrength);

	CoreAttributeSet->SetMaxHealth(CalculatedMaxHealth);
	CoreAttributeSet->SetHealth(CalculatedMaxHealth);
}

void AZRB_BaseCharacter::InitializeAbilities()
{
	// Guard against double-initialization.
	if (bAbilitiesInitialized || !AbilitySystemComponent) return;
	
	// Only the server (or standalone) should grant abilities.
	if (!HasAuthority()) return;
	
	// Grant startup abilities.
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
			
			const UZRB_GameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<UZRB_GameplayAbility>();
			if (AbilityCDO && AbilityCDO->InputTag.IsValid())
			{
				Spec.GetDynamicSpecSourceTags().AddTag(AbilityCDO->InputTag);
			}
			AbilitySystemComponent->GiveAbility(Spec);
		}
	}
	
	// Apply startup effects (e.g., base stat initialization).
	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupEffects)
	{
		if (EffectClass)
		{
			FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
			Context.AddSourceObject(this);
			
			FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1, Context);
			if (Spec.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
	
	bAbilitiesInitialized = true;
}

bool AZRB_BaseCharacter::PerformMeleeTrace(FHitResult& OutHitResult)
{
	const FVector Start =
		GetActorLocation() + GetActorForwardVector() * TraceStartOffset;

	const FVector End =
		Start + GetActorForwardVector() * TraceDistance;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	return UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		TraceRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHitResult,
		true
	);
}
