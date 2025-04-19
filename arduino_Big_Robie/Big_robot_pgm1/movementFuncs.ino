// Removed obsolete tilt() and pan() functions as they called the removed setMotor()
// Commands 'U','D','L','R' now directly use non-blocking start functions.

void tiltWithFback(int targetY, int mtrZoneBoundries[], motor *mtr)
{ // assumes targetY goes to zero as you approach target
	if (targetY < mtrZoneBoundries[0])
	{	// max lift boundary
		// set spd to max Lift value (do not know yet if cw or ccw)
	}
	if ((targetY > mtrZoneBoundries[0]) && (targetY < mtrZoneBoundries[1]))
	{	// lower dead band boundary
		// set spd to proportial Lift value (do not know yet if cw or ccw)
	}
	if ((targetY > mtrZoneBoundries[1]) && (targetY < mtrZoneBoundries[2]))
	{	// upper dead band boundary
		// set spd to dead band value (do not know yet if cw or ccw)
	}
	if ((targetY > mtrZoneBoundries[2]) && (targetY < mtrZoneBoundries[3]))
	{	// max drop boundary
		// set spd to proportial Fall value (do not know yet if cw or ccw)
	}
	if (targetY > mtrZoneBoundries[3])
	{	// max lift boundary
		// set spd to max Fall value (do not know yet if cw or ccw)
	}
}
