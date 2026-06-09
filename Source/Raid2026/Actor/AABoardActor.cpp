#include "AABoardActor.h"
#include "Net/UnrealNetwork.h"

void AABoardActor::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AABoardActor, gridPosition);
}

AABoardActor::AABoardActor()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AABoardActor::TakeDamage(int32 amount)
{
	currentHealthPoint -= amount;
	if (currentHealthPoint <= 0)
	{
		currentHealthPoint = 0;
		onDestroyeds.Broadcast(this);
		Die();
	}
	else
	{
		onDamaged.Broadcast(this, amount);
		PlayDamageEffect(amount);
	}
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

void AABoardActor::OnRep_GridPosition()
{
}
