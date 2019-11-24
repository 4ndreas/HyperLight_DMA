/*
 * helper.h
 *
 *  Created on: 19.11.2019
 *      Author: ahoel
 */

#ifndef HELPER_H_
#define HELPER_H_

			  //setLED(universe+8, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(0, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(1, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(2, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(3, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(4, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(5, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(6, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(7, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//
//		  setLED(8, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(9, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(10, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(11, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(12, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(13, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(14, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  setLED(15, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);

    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}


#endif /* HELPER_H_ */
