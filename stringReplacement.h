/*
 * stringReplacement.h
 *
 *  Created on: Mar 6, 2020
 *      Author: JaYniL~LM10
 */

#ifndef STRINGREPLACEMENT_H_
#define STRINGREPLACEMENT_H_

typedef struct H2D
{
    uint32_t remainder,quotient;
    uint8_t j,temp;
}h2d;

void string_copy( const char source_string[] ,  char destination_string[]);
void roder(char* string, int length );
int itoc(int z, char string[]);
uint8_t nstrlength(char a[]);
void name_display();
void sc();
void defaultmode();
void dectohex(uint32_t decnumber);


#endif /* STRINGREPLACEMENT_H_ */
