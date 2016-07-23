/* sk_base64.h - Base 64 encoding/decoding
 *
 */

#ifndef INCLUDE__SK_BASE64__H
#define INCLUDE__SK_BASE64__H

int sk_base64_get_buffer_size (int binary_length);
int sk_base64_encode (const void* inData, int inLength, char** outText, int bufferSize);
int sk_base64_decode (char* inText, void** outData);

#endif // INCLUDE__SK_BASE64__H
