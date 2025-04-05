/*************************************************
 *
 * servo related funcs
 *
 *************************************************/

// Left servo is a real servo - stops at requested position
void wave()
{
	for (int j = 0; j < 2; j++)
	{
		for (int pos = 0; pos < 180; pos += 2)
		{						  // goes from 0 degrees to 180 degrees in steps of 2 deg
			waveServo.write(pos); // tell servo to go to position in variable 'pos'
			delay(15);			  // waits 15ms for the servo to reach the position
		}
		for (int pos = 180; pos >= 1; pos -= 2)
		{						  // goes from 180 degrees to 0 degrees
			waveServo.write(pos); // tell servo to go to position in variable 'pos'
			delay(15);			  // waits 15ms for the servo to reach the position
		}
	}
}

// Right wrist servo is a continuous rotation servo, so has to be parked with a neutral position
void wave_right()
{
	for (int j = 0; j < 2; j++)
	{
		for (int pos = 0; pos < 180; pos += 2)
		{						  // goes from 0 degrees to 180 degrees in steps of 2 deg
			waveServo_right.write(pos); // tell servo to go to position in variable 'pos'
			delay(15);			  // waits 15ms for the servo to reach the position
		}
		for (int pos = 180; pos >= 1; pos -= 2)
		{						  // goes from 180 degrees to 0 degrees
			waveServo_right.write(pos); // tell servo to go to position in variable 'pos'
			delay(15);			  // waits 15ms for the servo to reach the position
		}
	}
	waveServo_right.write(93); // ensure servo stops
}