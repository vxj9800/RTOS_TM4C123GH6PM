
#include "termInput.h"

void parseString(struct termInput* userInput)
{
	bool prevCharState = 0;								// Previous Character state. 0 = Character not useful, 1 = Character is useful
	userInput->argCount = 0;
	uint8_t length = strlen(userInput->str);
	for (uint8_t i = 0; i<length; ++i)
	{
		char c = userInput->str[i];
		if ((c==45 || c==46 || c==38) || (c>47 && c<58) || (c>64 && c<91) || (c>96 && c<123))
		{
			if (prevCharState==0)
			{
				if (userInput->argCount == MAX_FIELDS)
				{
					putsUart0("\r\n> Number of arguments entered is more than maximum limit.");
					putsUart0("\r\n> All the arguments after maximum limit will be ignored.");
					break;
				}
				userInput->pos[userInput->argCount++] = i;
				prevCharState = 1;
			}
			// if (c>64 && c<91)
			// {
			// 	userInput->str[i] += 32;
			// }
		}
		else
		{
			prevCharState = 0;
			userInput->str[i] = '\0';
		}
	}
}

char* getArgString(struct termInput* userInput, uint8_t argNumber)
{
	if (argNumber <= userInput->argCount)
		return userInput->str+userInput->pos[argNumber];
	else
		return NULL;
}
