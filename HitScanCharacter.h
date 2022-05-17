#include "AmmoType.h"
#include "HitScanCharacter.generated.h"

UCLASS()
class HITSCANROJECT7_API AHitScanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHitScanCharacter();
  
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float ADSTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float ADSLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float HipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float HipLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0",UIMax = "1.0"))
		float MouseADSTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseADSLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseHipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseHipLookUpRate;
    
  void LMouseButtonDwn();
	void LMouseButtonUp();
	bool bLMouseButtonDwn;

	void RMouseButtonDwn();
	void RMouseButtonUp();
protected:
	virtual void BeginPlay() override;
  
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
  
  void SendBullet();

	void GunfireAnimMontage();
  
  void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);
	//Line Trace for items under the crosshairs 
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHit);
  
  void StartAutoFireTimer();
	UFUNCTION()
	void AutoFireReset();
	FTimerHandle AutoFireTimer;
	float RateOfFire;

	void ReloadButtonPressed();
	void ReloadWeapon();
	bool bReloading;
  
  //Checks to see if i have the proper ammo for the weapon
	bool ReserveAmmo();

	class AHitScanWeapon* SpawnDefaultWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 AmmoReserves;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHitScanWeapon> DefaultWeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AHitScanWeapon* ActiveOverlappingItem;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FiringSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//bullet trails
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	//Does your gun have ammo
	bool GunHasAmmo();
  
  void CalculateCrosshairSpread(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	float CameraDefualtFOV;
	float CameraZoomedFOV;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraInterpSpeed;
	void CameraInterpFOV(float DeltaTime);
	//for inputs That require rates
	void SetAimSensitivityRates(float DeltaTime);
	void MouseTurnSensitivity(float rate);
	void MouseLookUpSensitivity(float rate);

	void StartCrosshairWeaponFire();
	UFUNCTION()
	void StopCrosshairWeaponFire();

	//Creates reverve ammo for weapons
	void InitializeAmmoMap();
  
  void PickUpAmmo(class AAmmoPickUps* Ammo);
  
private:
  //Determines the Crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	//How fast the crosshairs move
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairADSFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float WeaponFireDuration;
	bool bFiringWeapon;
	FTimerHandle CrosshairFiringTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipfireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	//keeps track of ammo
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 ReserveAmmo_Rifle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 ReserveAmmo_SMG;
  
public:
  //Call every frame
  	virtual void  Tick(float DeltaTime) override;
  
  	void EquipWeapon(AHitScanWeapon* Weapon);
	
  	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AHitScanWeapon* EquippedWeapon;
  
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
  
  	FORCEINLINE bool GetAiming()const { return bAiming; }
  };
