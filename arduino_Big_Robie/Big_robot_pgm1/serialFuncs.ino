/*************************************************
 *
 * serial port related funcs
 *
 *************************************************/
void establishContact()
{
	while (Serial.available() <= 0)
	{
		Serial.println("BIG_ROBOT"); // send string
		delay(1000);
	}
}

int readCmdLine()
{
	int recLen = 0;
	if (Serial.available() > 0)
	{
		// if there's any serial available, read it:
		while (Serial.available() > 0)
		{
			recLen = Serial.readBytes(inputString, bufLen);
		}
	}
	return recLen;
}