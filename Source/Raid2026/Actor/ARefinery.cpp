#include "ARefinery.h"

bool AARefinery::IsNeutral() const
{
	return ControllerPlayerID == -1;
}

bool AARefinery::IsControlledBy(int32 PlayerID) const
{
	return ControllerPlayerID == PlayerID;
}

void AARefinery::Capture(int32 NewControllerID)
{
	ControllerPlayerID = NewControllerID;
	PlayCaptureEffect(NewControllerID);
	UpdateControlIndicator(NewControllerID);
	OnCaptured.Broadcast(this, NewControllerID);
}

void AARefinery::Neutralize()
{
	ControllerPlayerID = -1;
}
