/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 AES256 -  Encryption and Decryption Example
 *
 *  Description: This demo shows a simple example of encryption and decryption
 *  using the AES356 module.
 *
 *               MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Key: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
 * Plaintext: 00112233445566778899aabbccddeeff
 * Ciphertext: 8ea2b7ca516745bfeafc49904b496089
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Statics */
static uint8_t CipherKey[32] =
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
        0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
static uint8_t DataAESencrypted[1600];       // Encrypted data
static uint8_t DataAESdecrypted[1600];       // Decrypted data

/* Function Declarations */
void encrypt_message(char* str, uint8_t* encrypted, uint8_t* key);
void decrypt_message(uint8_t* data, int data_length, uint8_t* decrypted, uint8_t* key);

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    char txt[1600];
    int str_length;

    /* Get user input text message */
    while(1)
    {
        printf("Enter secret message: ");   //User prompt
        gets(txt);
        str_length = strlen(txt);           //Gets the length of the string

        /* Encrypt and decrypt the message */
        encrypt_message(txt, DataAESencrypted, CipherKey);
        decrypt_message(DataAESencrypted, str_length, DataAESdecrypted, CipherKey);

        printf("Encrypted Data (Hex): ");   //Prints encrypted data in hex format
        int ii;
        for (ii = 0; ii < str_length; ii++)
               {
                   printf("%02X", DataAESencrypted[ii]);
               }
        printf("\nDecrypted Data (Str): %s\n\n", DataAESdecrypted); //Prints the decrypted string message
    }
    
    /* Array DataAESdecrypted should now contain the same data as array Data */
    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}

void encrypt_message(char* str, uint8_t* encrypted, uint8_t* key)
{
    /* Load a cipher key to module */
    MAP_AES256_setCipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT);

    int ii;
    int loop_count = (strlen(str)/16) + (strlen(str)%16 != 0);                  //Gets the number of loop iterations
    for (ii = 0; ii < loop_count; ii++)                                         //Encrypts 16-Byte Blocks of data
        MAP_AES256_encryptData(AES256_BASE, str + ii*16, encrypted + ii*16);    //Encrypts data
}

void decrypt_message(uint8_t* data, int data_length, uint8_t* decrypted, uint8_t* key)
{
    /* Load a decipher key to module */
    MAP_AES256_setDecipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT);

    int ii;
    int loop_count = (data_length/16) + (data_length%16 != 0);                      //Gets the number of loop iterations
    for (ii = 0; ii < loop_count; ii++)                                             //Decrypts 16-Byte blocks of data
        MAP_AES256_decryptData(AES256_BASE, data + (ii*16), decrypted + (ii*16));   //Decrypts data
}
