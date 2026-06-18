#include "AABoardActor.h"
#include "Net/UnrealNetwork.h"

void AABoardActor::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AABoardActor, gridPosition);
	DOREPLIFETIME(AABoardActor, ownerPlayer);
}

AABoardActor::AABoardActor()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AABoardActor::TakeDamage(int32 amount)
{
	currentHealthPoint -= amount;
}

int32 AABoardActor::GetCurrentHP() const
{
	return currentHealthPoint;
}

int32 AABoardActor::GetMaxHP() const
{
	return maxHealthPoint;
}

bool AABoardActor::IsAlive() const
{
	return currentHealthPoint > 0;
}

void AABoardActor::SetHealthPoint(int32 health)
{
	maxHealthPoint = health;
	currentHealthPoint = health;
}

FIntPoint AABoardActor::GetGridPosition() const
{
	return gridPosition;
}

int32 AABoardActor::GetOwnerID() const
{
	return ownerPlayer;
}

void AABoardActor::Die_Implementation()
{

}

void AABoardActor::SetGridPosition(FIntPoint NewPos)
{
	gridPosition = NewPos;
}

void AABoardActor::Heal(int32 amount)
{
	if (currentHealthPoint == maxHealthPoint)
		return;

	currentHealthPoint += maxHealthPoint;

	if (currentHealthPoint > maxHealthPoint)
		currentHealthPoint = maxHealthPoint;
}

void AABoardActor::OnRep_GridPosition()
{
}
