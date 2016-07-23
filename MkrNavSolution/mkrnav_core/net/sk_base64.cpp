/* sk_base64.c - Base 64 encoding/decoding
 *
 */

#include <stdlib.h>
#include <string.h>
#include "mkrnav.h"
#include "sk_base64.h"


static char encodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char decodingTable[128];


int sk_base64_get_buffer_size (int binary_length) {
   int text_length = ((binary_length + 2) / 3) * 4 + 1;
   return text_length;
}

int sk_base64_encode (const void* inData, int inLength, char** outText, int bufferSize) {
   int i, j;
   const unsigned char* input = (const unsigned char*) inData;
   unsigned char* output;
   int text_length = sk_base64_get_buffer_size(inLength);

   if (bufferSize != text_length) {
      sk_log (SK_ERROR, "sk_base64_encode() - bad buffer size (%d), expected (%d)",
                   bufferSize, text_length);
      return FALSE;
   }
   
   output = (unsigned char*)*outText;
   
   for (i = 0; i < inLength; i += 3) {
      int value = 0;
      int index;
      for (j = i; j < (i + 3); j++) {
         value <<= 8;
         
         if (j < inLength) {
            value |= (0xFF & input[j]);
         }
      }
      
      index = (i / 3) * 4;
      output[index + 0] =                       encodingTable[(value >> 18) & 0x3F];
      output[index + 1] =                       encodingTable[(value >> 12) & 0x3F];
      output[index + 2] = (i + 1) < inLength ?  encodingTable[(value >> 6)  & 0x3F] : '=';
      output[index + 3] = (i + 2) < inLength ?  encodingTable[(value >> 0)  & 0x3F] : '=';
   }
   
   output[text_length - 1] = 0;
   
   return TRUE;
}

int sk_base64_decode (char* inText, void** outData) {
//AR: Decoding code untested
   int i;
   int inputLength = strlen(inText) -1;
   static int initialized = 0;
   
   if ((inText== NULL) || (inputLength % 4 != 0)) {
      return -1;
   }
   
   if (!initialized) {
      memset(decodingTable, 0, 128);
      for (i = 0; i < strlen(encodingTable); i++) {
         decodingTable[encodingTable[i]] = i;
      }
      initialized = 1;
   }
   
   while (inputLength > 0 && inText[inputLength - 1] == '=') {
      inputLength--;
   }
   
   int outputLength = inputLength * 3 / 4;
   unsigned char* output = (unsigned char*)malloc(outputLength);
   *outData = (void *)output;
   
   int inputPoint = 0;
   int outputPoint = 0;
   while (inputPoint < inputLength) {
      char i0 = inText[inputPoint++];
      char i1 = inText[inputPoint++];
      char i2 = inputPoint < inputLength ? inText[inputPoint++] : 'A'; /* 'A' will decode to \0 */
      char i3 = inputPoint < inputLength ? inText[inputPoint++] : 'A';
      
      output[outputPoint++] = (decodingTable[i0] << 2) | (decodingTable[i1] >> 4);
      if (outputPoint < outputLength) {
         output[outputPoint++] = ((decodingTable[i1] & 0xf) << 4) | (decodingTable[i2] >> 2);
      }
      if (outputPoint < outputLength) {
         output[outputPoint++] = ((decodingTable[i2] & 0x3) << 6) | decodingTable[i3];
      }
   }
   
   return outputLength;
}