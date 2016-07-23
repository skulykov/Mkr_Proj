/* sk_main.h - The interface for the Sk_ main window module.
 *

 */

#ifndef INCLUDE__SK__MAIN__H
#define INCLUDE__SK__MAIN__H

#include "mkrnav.h"
//#include "sk_gui.h"

#include "sk_io.h"
#include "sk_spawn.h"

#define SK__CURSOR_NORMAL 1
#define SK__CURSOR_WAIT   2

struct Sk_FactoryKeyMap;

typedef void (* Sk_KeyInput) (char *key);
typedef void (* Sk_Input)    (Sk_IO *io);
#ifdef IPHONE
typedef void (* Sk_MainIoCb) (Sk_Socket socket);
#endif

typedef void *Sk_Menu;

void sk_main_new (const char *title, int width, int height);

void sk_main_set_keyboard (struct Sk_FactoryKeyMap *bindings,
                                Sk_KeyInput callback);

Sk_Menu sk_main_new_menu (void);
void sk_main_free_menu       (Sk_Menu menu);
void sk_main_add_menu        (Sk_Menu menu, const char *label);
void sk_main_add_menu_item   (Sk_Menu menu,
                                   const char *label,
                                   const char *tip,
                                   Sk_Callback callback);
void sk_main_add_separator   (Sk_Menu menu);
void sk_main_popup_menu      (Sk_Menu menu, int x, int y);

void sk_main_add_tool       (const char *label,
                                  const char *icon,
                                  const char *tip,
                                  Sk_Callback callback);
void sk_main_add_tool_space (void);

void sk_main_add_canvas     (void);
void sk_main_add_status     (void);

void sk_main_show (void);

void sk_main_set_input    (Sk_IO *io, Sk_Input callback);
void sk_main_set_output   (Sk_IO *io, Sk_Input callback, BOOL is_connect);
void sk_main_remove_input (Sk_IO *io);

Sk_IO *sk_main_output_timedout(time_t timeout);

void sk_main_set_periodic (int interval, Sk_Callback callback);
void sk_main_remove_periodic (Sk_Callback callback);

void sk_main_set_status (const char *text);

void sk_main_toggle_full_screen (void);

void sk_main_flush (void);

void sk_main_exit (void);

int sk_main_is_widget_mode( void );

void sk_main_set_app_mode( int mode );

void sk_main_set_cursor (int cursor);

void sk_gui_minimize();
void sk_gui_maximize();
void sk_main_minimize (void);
BOOL sk_horizontal_screen_orientation();
void sk_main_open_url (const char* url);
void sk_main_post_resolver_result( int entry_id );

#ifdef IPHONE

int sk_main_should_mute ();
int sk_main_will_suspend ();
const char* sk_main_get_proxy (const char* url);
#include <sys/socket.h>
int sk_main_async_connect(Sk_IO *io, struct sockaddr *addr, Sk_Input callback) ;

int sk_main_get_platform ();
int sk_main_get_os_ver ();
int sk_main_should_save_nav ();

char const *sk_main_home_path (void);
char const *sk_main_bundle_path (void);
char const *sk_main_cache_path (void);
void sk_main_show_root (int animated);
int sk_main_is_root (void);
int sk_main_get_mainbox_height (void);
void sk_main_pop_view(int animated);
void sk_main_set_backlight(int isAlwaysOn);
void sk_main_refresh_backlight (void);
void sk_main_play_movie (const char* url);

typedef void *Sk_NativeImage;
Sk_NativeImage sk_main_load_image (const char *path, const char* file_name);
void sk_main_free_image (Sk_NativeImage image);

#define SK__MAIN_PLATFORM_NA       0
#define SK__MAIN_PLATFORM_IPOD     1
#define SK__MAIN_PLATFORM_IPHONE   2
#define SK__MAIN_PLATFORM_IPAD     3

#define SK__MAIN_OS_NA             0
#define SK__MAIN_OS_30             1
#define SK__MAIN_OS_31             2
#define SK__MAIN_OS_4              3
#define SK__MAIN_OS_5              4

BOOL sk_horizontal_screen_orientation();

#endif //IPHONE


int LogResult( int aVal, const char *aStrPrefix, int level, char *source, int line );

// map download
void sk_map_download_region (const char *region_code, int fips);
void sk_main_start_init();
void sk_main_message_dispatcher( int aMsg );
#endif /* INCLUDE__SK__MAIN__H */

