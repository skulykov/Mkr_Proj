/* sk_android_msg.h - Messages definitions
 
 */

#ifndef INCLUDE__SK_ANDROID_MSG__H
#define INCLUDE__SK_ANDROID_MSG__H

//!!!! The category masks are two MS bytes
//!!!! Two LS bytes are assigned to the message ID
#define ANDROID_MSG_CATEGORY_IO_CALLBACK	0x010000
#define ANDROID_MSG_CATEGORY_IO_CLOSE		0x020000
#define ANDROID_MSG_CATEGORY_TIMER			0x040000
#define ANDROID_MSG_CATEGORY_MENU			0x080000
#define ANDROID_MSG_CATEGORY_RENDER			0x100000
#define ANDROID_MSG_CATEGORY_RESOLVER     0x200000

#define ANDROID_MSG_ID_MASK			0xFFFF



#endif // INCLUDE__SK_ANDROID_MSG__H

