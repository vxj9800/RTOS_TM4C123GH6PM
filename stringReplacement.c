/*
 * stringReplacement.c
 *
 *  Created on: Mar 6, 2020
 *      Author: JaYniL~LM10
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "stringReplacement.h"



void string_copy( const char source_string[] ,  char destination_string[])
{
    int b=0;
    for(;source_string[b] != '\0';b++)
      destination_string[b] = source_string[b];
    destination_string[b] = '\0';
}
void roder(char* string, int length )
{
    int i = 0;
    int  j = length - 1;
    int lc;
    for(;i<j;i++)
    {
        lc = string[i];
        string[i] = string[j];
        string[j] = lc;
        j--;
    }
}
int itoc(int z, char string[])
{
    int i = 0;
    while (z)
    {
        string[i++] = (z % 10) + '0';
        z /= 10;
    }


    roder(string, i);
    string[i] = '\0';
    return i;
}
void name_display()
{
    putsUart0("***************************************\r\n");
    putsUart0("EE6314 Advanced Embedded Microcontroller Systems \r\n");
    putsUart0("Name: JAYNIL PATEL\r\n");
    putsUart0("ID: 1001678013\r\n");
    putsUart0("Project 1: RTOS (SPRING 2020)\r\n");
    putsUart0("***************************************\r\n");
    putsUart0("\r\n");
    putsUart0("Type sc for list of supported commands with format\r\n");
}
void sc()
{
    putsUart0("\r\n");
    putsUart0("ps                                   \t\t//For Process Status\r\n");
    putsUart0("ipcs                         \t\t//for Inter process Communication Status\r\n");
    putsUart0("pid <Thread Name>                    \t\t //to get the pid of any thread \r\n");
    putsUart0("kill <pidno>                         \t\t//to kill any specific thread\r\n");
    putsUart0("<thread name> &                      \t\t // to restart any thread\r\n");
    putsUart0("reboot                               \t\t //to reset the board\r\n");
    putsUart0("preemption <ON/OFF>                  \t\t//turn on/off preemption \r\n");
    putsUart0("scheduler <priority/rr>     \t //to select priority scheduler or Round Robin\r\n");
    putsUart0("setpr <Thread Name> <New Priority> \t //to change any thread's priority \r\n");
    putsUart0("pi <ON/OFF>                       \t//Turn on/off priority Inheritance \r\n");

}

void defaultmode()
{
    putsUart0("Default Modes are as below:-\r\n");
    putsUart0("PI ON \tPREEMPTION ON \t SCHEDULER   PRIORITY\r\n");
    putsUart0("\r\n");
}

void dectohex(uint32_t decnumber)
{

    uint8_t i=1,ncountDigits=0,r=0;
    h2d hex2dec;
    char hexadecimalNumber[100];
    hex2dec.quotient = decnumber;
    while(hex2dec.quotient!=0)
    {
        hex2dec.temp = hex2dec.quotient % 16;
        if( hex2dec.temp < 10)
            hex2dec.temp = hex2dec.temp + 48;
        else
            hex2dec.temp = hex2dec.temp + 55;
        hexadecimalNumber[i++]= hex2dec.temp;
        hex2dec.quotient = hex2dec.quotient / 16;
        ncountDigits++;
    }
    if(ncountDigits == 0)
    {
        putsUart0("00000000");
    }
    else if(ncountDigits<=8 & ncountDigits >0)
    {
        for(r = 1 ; r <=8 ; r++)
            hexadecimalNumber[ncountDigits + r] = '0';
        r =0;
        while(r<=7)
        {
            putcUart0(hexadecimalNumber[8-r]);
            r++;
        }
    }

    else
    {
        for (hex2dec.j = i -1 ;hex2dec.j> 0;hex2dec.j--)
          putcUart0(hexadecimalNumber[hex2dec.j]);
    }
}

uint8_t nstrlength(char a[])
{
        uint8_t i;
        for(i=0; a[i]!='\0'; ++i);
        return i;
}
