#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "HitScanWeapon.h"
#include "AmmoPickUps.h"

AHitScanCharacter::AHitScanCharacter() : BaseTurnRate(45.f), BaseLookUpRate(45.f), ADSTurnRate(20.f), ADSLookUpRate(20.f), HipTurnRate(90.f), HipLookUpRate(90.f),MouseADSTurnRate(0.2f),
MouseADSLookUpRate(0.2f), MouseHipTurnRate(1.0f), MouseHipLookUpRate(1.0f), RateOfFire(0.175f), bAiming(false),CameraDefualtFOV(0.f),CameraZoomedFOV(60.f), CurrentFOV(0.f),CameraInterpSpeed(20.f),
  WeaponFireDuration(0.05f), ReserveAmmo_Rifle(75), ReserveAmmo_SMG(18)
  {  
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	  
	//Creates Spring Arm. Pulls towards player if collision
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 350.f;//Camera follow distance
	CameraArm->bUsePawnControlRotation = true;//Rotate arm based on controller
	CameraArm->SocketOffset = FVector(0.f, 50.f, 50.f);
	/*//Set size for collsion capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);*/

	//Creates Camera that follow Character
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	//camera will rotate based on the spring arm which rotates based on the character
	FollowCamera->bUsePawnControlRotation = true;

	//Keep character from rotating with camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
  }

void AHitScanCharacter::BeginPlay()
{
	Super::BeginPlay();
  
  //Spawns a default weapon and equips it 
	EquipWeapon(SpawnDefaultWeapon());
}
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpFOV(DeltaTime);
	SetAimSensitivityRates(DeltaTime);
	CalculateCrosshairSpread(DeltaTime);
}

void AHitScanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  PlayerInputComponent->BindAction("NormalAttack", IE_Pressed, this, &AMainCharacter::LMouseButtonDwn);
	PlayerInputComponent->BindAction("NormalAttack", IE_Released, this, &AMainCharacter::LMouseButtonUp);
  
  PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &AMainCharacter::RMouseButtonDwn);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &AMainCharacter::RMouseButtonUp);
 }

void AHitScanCharacter::MouseTurnSensitivity(float rate) {
	float TurnScaleFactor;
	if (bAiming) {
		TurnScaleFactor = MouseADSTurnRate;
	}
	else {
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(rate * TurnScaleFactor);
}

void AHitScanCharacter::MouseLookUpSensitivity(float rate) {
	float TurnScaleFactor;
	if (bAiming) {
		TurnScaleFactor = MouseADSLookUpRate;
	}
	else {
		TurnScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(rate * TurnScaleFactor);
}

void AHitScanCharacter::SetAimSensitivityRates(float DeltaTime) {
	if (bAiming) {
		BaseTurnRate = ADSTurnRate;
		BaseLookUpRate = ADSLookUpRate;
	}
	else {
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AHitScanCharacter::CameraInterpFOV(float DeltaTime) {
	//Setting the current Camera FOV
	if (bAiming) {//Interp to Zoomed FOV
		CurrentFOV = FMath::FInterpTo(CurrentFOV, CameraZoomedFOV, DeltaTime, CameraInterpSpeed);
	}
	else {//Interp to Defualt FOV
		CurrentFOV = FMath::FInterpTo(CurrentFOV, CameraDefualtFOV, DeltaTime, CameraInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CurrentFOV);

}
 
 void AHitScanCharacter::LMouseButtonDwn() {
	bLMouseButtonDwn = true;
	FireWeapon();
}

void AHitScanCharacter::LMouseButtonUp() {
	bLMouseButtonDwn = false;
	EquippedWeapon->ResetRecoil(); //Stops recoil after player stops shooting
}

void AHitScanCharacter::RMouseButtonDwn() {
	bAiming = true;
	if (bLMouseButtonDwn == false && bAiming == true) { // if player is ADS and no longer shooting
		EquippedWeapon->ResetRecoil();				          	// the recoil will reset 
	}
}

void AHitScanCharacter::RMouseButtonUp() {
	bAiming= false;
}

void AHitScanCharacter::EquipWeapon(AHitScanWeapon* Weapon) {
	if (Weapon) {
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("hand_rSocket_Gun"));
		if (HandSocket) {
			//Attch weapon to socket
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		EquippedWeapon = Weapon;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
		
	}

}

AHitScanWeapon* AHitScanCharacter::SpawnDefaultWeapon() {
	//Check the TSubclssOf variable
	if (DefaultWeaponType) {
		//Spawn the weapon
		return  GetWorld()->SpawnActor<AHitScanWeapon>(DefaultWeaponType);
	}
	return nullptr;
}

void AHitScanCharacter::CalculateCrosshairSpread(float spread) {
	FVector2D WalkSpeedRange{0.f,650.f};
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
	
	if (GetCharacterMovement()-> IsFalling()) {
		//Spreads the crosshair slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, spread, 2.25f);
	}
	else {
		//Shrink the crosshairs rapidly when character lands or on the ground
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, spread, 30.f);
	}
	if (bAiming) {
		CrosshairADSFactor = FMath::FInterpTo(CrosshairADSFactor, 0.2, spread, 30.f);
	}
	else {
		CrosshairADSFactor = FMath::FInterpTo(CrosshairADSFactor, 0.f, spread, 30.f);
	}
	if (bFiringWeapon) {
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, spread, 60.f);
	}
	else {
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, spread, 45.f);
	}

	CrosshairSpreadMultiplier = 0.25f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairADSFactor + CrosshairShootingFactor;
}

void AHitScanCharacter::StartCrosshairWeaponFire() {
	bFiringWeapon = true;

	GetWorldTimerManager().SetTimer(CrosshairFiringTimer, this, &AMainCharacter::StopCrosshairWeaponFire, WeaponFireDuration);
}

void AHitScanCharacter::StopCrosshairWeaponFire() {
	bFiringWeapon = false;
}

float AHitScanCharacter::GetCrosshairSpreadMultiplier() const {
	return CrosshairSpreadMultiplier;
}

void AHitScanCharacter::FireWeapon() {
	if (EquippedWeapon == nullptr)return;
	if (CombatState != ECombatState::ECS_Unoccupied)return;

	if (GunHasAmmo()) {
		PlayFireSound();
		GunfireAnimMontage();
		EquippedWeapon->DecrementAmmo();
		StartAutoFireTimer();
		SendBullet();
		EquippedWeapon->WeaponRecoilPattern(); //Starts recoil pattern from Blueprints
		ReloadWeapon();
	}

}

void AHitScanCharacter::InitializeAmmoMap() {
	AmmoMap.Add(EAmmoType::EAT_RifleAmmo, ReserveAmmo_Rifle);
	AmmoMap.Add(EAmmoType::EAT_SMG, ReserveAmmo_Shotgun);
}

bool AHitScanCharacter::GunHasAmmo() {
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

bool AHitScanCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation) {
	/// Check for crosshair trace hit
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);
	if (bCrosshairHit) {
		// Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else {
		// no crosshair trace hit
		// OutBeamLocation is the End location for the line trace
	}
	// Perform a second trace, this time from the gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - WeaponTraceStart };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	float BulletSpreadMultiplier = UKismetMathLibrary::RandomFloatInRange(0.1f, CrosshairSpreadMultiplier);
	FVector BulletSpread = UKismetMathLibrary::Add_VectorFloat(WeaponTraceEnd, BulletSpreadMultiplier);
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, BulletSpread, ECollisionChannel::ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit) // object between barrel and BeamEndPoint?
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}

	return false;
}

bool AHitScanCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation) {
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 25.f;//Option to Raise/Lower Crosshair 
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld) {
		//FHitResult ScreenTraceHit;
		// Trace from Crosshair world location outward
		const FVector Start{CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit) {
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AHitScanCharacter::SendBullet() {

	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetWeaponMesh()->GetSocketByName("Muzzle");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetWeaponMesh());

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

void AHitScanCharacter::PlayFireSound() {
	if (FiringSound) {
		UGameplayStatics::PlaySound2D(this, FiringSound);
	}
}

void AHitScanCharacter::GunfireAnimMontage() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipfireMontage) {
		AnimInstance->Montage_Play(HipfireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
	//Start weapon fire timer for crosshairs
	StartCrosshairWeaponFire();
}

void AHitScanCharacter::StartAutoFireTimer() {
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AMainCharacter::AutoFireReset, RateOfFire);
}

void AHitScanCharacter::ReloadWeapon() {
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;

	//Do you have the correct ammo?
	if (ReserveAmmo()) {

		CombatState = ECombatState::ECS_Reloading;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (ReloadMontage && AnimInstance) {
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool AHitScanCharacter::ReserveAmmo() {
	if (EquippedWeapon == nullptr)return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType)) {
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AHitScanCharacter::PickUpAmmo(AAmmoPickUps* Ammo) {
	//Check to see if AmmoMap has the ammo needed
	if (AmmoMap.Find(Ammo->GetAmmoType())) {
		//Get amount of ammo in our Ammo Map for the ammo type
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();
		//Set the amount of ammo in the Map of the desired ammo type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	//Auto Reloads weapon on pickup with no ammo
	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType()) {
		//check if the the gun is empty
		if (EquippedWeapon->GetAmmo() == 0) {
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

void AHitScanCharacter::FinishReloading() {
	
	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr)return;

	//Updates the ammo map
	const auto AmmoType{ EquippedWeapon->GetAmmoType() };
	if(AmmoMap.Contains(AmmoType)){
		//The reserve ammo the character holds in their invetory
		int32 ReserveAmmo = AmmoMap[AmmoType];

		//The amount of ammo being reloaded 
		const int32 ReloadedAmmo = EquippedWeapon->GetMagazineSize() - EquippedWeapon->GetAmmo();
		if (ReloadedAmmo > ReserveAmmo) {
			//Gives all reserve ammo to the Weapon's Magazine
			EquippedWeapon->ReloadAmmo(ReserveAmmo);
			ReserveAmmo = 0;
			AmmoMap.Add(AmmoType, ReserveAmmo);
		}
		else {
			//Fills the Magazine
			EquippedWeapon->ReloadAmmo(ReloadedAmmo);
			ReserveAmmo -= ReloadedAmmo;
			AmmoMap.Add(AmmoType, ReserveAmmo);
		}
	}
}

void AHitScanCharacter::AutoFireReset() {
	CombatState = ECombatState::ECS_Unoccupied;

	if (GunHasAmmo()) {
		if (bLMouseButtonDwn) {
			FireWeapon();
		}
	}
	else {
		ReloadWeapon();
	}
}
