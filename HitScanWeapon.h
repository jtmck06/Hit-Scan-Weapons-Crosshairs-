#include "AmmoType.h"
#include "HitScanWeapon.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	EIS_Holstered UMETA(DisplayName = "Holstered"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class HITSCANPROJECT7_API HitScanWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	HitScanWeapon();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

void SetItemProperties(EItemState State);

  UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
  
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item| Properties", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponMesh;
private:
  //Ammo Count for weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineSize;
  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item| Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;
  
  //Pointer to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item| Properties", meta = (AllowPrivateAccess = "true"))
	class AHitScanCharacter* CharacterPtr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item| Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item| Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;
  
protected:
virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
  
  void DecrementAmmo();
	void ReloadAmmo(int32 Amount);

	//Functions that will be implemented in blueprints to control the recoil pattern for the gun
	UFUNCTION(BlueprintImplementableEvent, Category = "Recoil")
	void WeaponRecoilPattern();
	UFUNCTION(BlueprintImplementableEvent, Category = "Recoil")
	void ResetRecoil();
  
  FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagazineSize() const { return MagazineSize; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
  FORCEINLINE USphereComponent* GetCollisionVolume() const { return CollisionVolume; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
};
