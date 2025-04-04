/*************************************************
 *
 * serial port related funcs
 *
 *************************************************/
void establishContact()
{
	while (Serial.available() <= 0)
	{
		Serial.println("BIG_ROBOT>"); // send string
		delay(1000);
	}
}

int readCmdLine()
{
	int recLen = Serial.available(); // check for available bytes in the serial buffer
	if (recLen > bufLen) // if more than the buffer can hold, limit to bufLen
	{
		recLen = bufLen; // limit to the buffer size to avoid overflow
	}
	else if (recLen <= 0)
	{
		return 0; // no data to process
	}
	
	Serial.readBytes(inputString, recLen); // clear out the excess bytes in the buffer
	inputString[recLen] = '\0';			  // ensure null termination of string
	Serial.println(inputString);
	return recLen;
}