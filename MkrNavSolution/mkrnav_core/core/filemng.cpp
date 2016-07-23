#include "filemng.h"

using namespace std;
#if !SK_BUILD_FOR_ANDROID

int main_fsinit()
{
/*
  ifstream fin;
  string filepath;
  int num;
  DIR *dp;
  struct dirent *dirp;
  struct stat filestat;

  //cout << "dir to get files of: " << flush;
  //getline( cin, dir );  // gets everything the user ENTERs
  string  dir="/mnt/sdcard/fcnavgl";
  dp = opendir( dir.c_str() );
  if (dp == NULL)
    {
    //cout << "Error(" << errno << ") opening " << dir << endl;
		LOGE(">>>>Could not open /mnt/sdcard/fcnavgl\n");
		 LOGE(">>>>making /mnt/sdcard/fcnavgl\n");
		int rv =	mkdir("/mnt/sdcard/fcnavgl",0777);
		LOGE(">>>>making /mnt/sdcard/fcnavgl mode 0777 rv=%d\n",rv);
		//return 0;
    }

 

  while ((dirp = readdir( dp )))
    {
		filepath = dir + "/" + dirp->d_name;

		//LOGI("filepath %s \n",filepath.c_str());
		// If the file is a directory (or is in some way invalid) we'll skip it 
		if (stat( filepath.c_str(), &filestat )) continue;
		if (S_ISDIR( filestat.st_mode ))         continue;

		//LOGI("+***ONLY FILES*** filepath %s \n",filepath.c_str());
		// Endeavor to read a single number from the file and display it
		fin.open( filepath.c_str() );
		if (fin >> num)
		{
		  //LOGI("filepath %s \n",filepath.c_str());//cout << filepath << ": " << num << endl;
		}
		fin.close();
    }
  LOGI("+***FILES*** filepath %s \n",filepath.c_str());

  closedir( dp );
*/
  return 0;
  }

#else


#pragma region SK_FILE_PATH

typedef struct Sk_FilePathRecord *Sk_FilePathList;

struct Sk_FilePathRecord {

   Sk_FilePathList next;

   char  *name;
   int    count;
   char **items;
   char  *preferred;
};

static Sk_FilePathList Sk_FilePaths = NULL;


/* This list exist because the user context is a special case
 * that we want to handle in a standard way.
 */
static const char *Sk_FilePathUser[] = {
   "~/"HOME_PREFIX,
#ifdef IPHONE
	"+/"HOME_PREFIX,
#endif //IPHONE
   NULL
};
static const char *Sk_FilePathUserPreferred = "~/"HOME_PREFIX;


/* Skins directories */
static const char *Sk_FilePathSkin[] = {
   "~/"HOME_PREFIX"/skins/default/day",
   "~/"HOME_PREFIX"/skins/default",
#ifdef IPHONE
	"+/"HOME_PREFIX"/skins/default/day",
	"+/"HOME_PREFIX"/skins/default",
#endif //IPHONE
   NULL
};
#ifndef IPHONE
static const char *Sk_FilePathSkinPreferred = "~/"HOME_PREFIX"/skins";
#else
static const char *Sk_FilePathSkinPreferred = "+/"HOME_PREFIX"/skins";
#endif //IPHONE

/* The hardcoded path for configuration files (the "config" path).
 * (Note that the user directory (~/.waze) does not appear here
 * as it is implicitely used by the callers--see above.)
 */
static const char *Sk_FilePathConfig[] = {
#ifdef QWS
   /* This is for the Sharp Zaurus PDA.. */
   "/opt/QtPalmtop/share/mkrnav",
   "/mnt/cf/QtPalmtop/share/mkrnav",
   "/mnt/card/QtPalmtop/share/mkrnav",
#else
   /* This is for standard Unix configurations. */
   "~/"HOME_PREFIX,
#ifdef IPHONE
	"+/"HOME_PREFIX,
#endif //IPHONE
   "/etc/mkrnav",
   "/usr/local/share/mkrnav",
   "/usr/share/mkrnav",
#endif
   NULL
};
static const char *Sk_FilePathConfigPreferred =
#ifdef QWS
                      "/mnt/cf/QtPalmtop/share/mkrnav";
#else
                      "~/"HOME_PREFIX;
#endif

static const char *Sk_FilePathGpsSuffix = "gps/";

/* The default path for the map files (the "maps" path): */
static const char *Sk_FilePathMaps[] = {
#ifdef QWS
   /* This is for the Sharp Zaurus PDA.. */
   "/opt/QtPalmtop/share/mkrnav",
   "/mnt/cf/QtPalmtop/share/mkrnav",
   "/mnt/card/QtPalmtop/share/mkrnav",
#else
   /* This is for standard Unix configurations. */
#ifndef IPHONE
   "~/"HOME_PREFIX"/maps",
#else
   "#/"HOME_PREFIX"/maps",
	"+/"HOME_PREFIX"/maps",
#endif //IPHONE
   "/var/lib/mkrnav",
   "/usr/lib/mkrnav",

   /* These paths are not really compliant with the FHS, but we
    * keep them for compatibility with older versions of Sk_.
    */
   "/usr/local/share/mkrnav",
   "/usr/share/mkrnav",
#endif
   NULL
};
static const char *Sk_FilePathMapsPreferred =
#ifdef QWS
                      "/mnt/cf/QtPalmtop/share/mkrnav";
#else
#ifdef IPHONE
					"#/"HOME_PREFIX"/maps";
#else
                      "/var/lib/mkrnav";
#endif //IPHONE
#endif

static char *sk_path_expand (const char *item, size_t length);

void sk_path_initialize( const char* aCfgPath )
{

}

static void sk_path_list_create(const char *name,
                                     const char *items[],
                                     const char *preferred) {


   size_t i;
   size_t count;
   Sk_FilePathList new_path;

   for (count = 0; items[count] != NULL; ++count) ;

   new_path = (Sk_FilePathList)malloc (sizeof(struct Sk_FilePathRecord));
   sk_check_allocated(new_path);

   new_path->next  = Sk_FilePaths;
   new_path->name  = strdup(name);
   new_path->count = (int)count;

   new_path->items = (char**)calloc (count, sizeof(char *));
   sk_check_allocated(new_path->items);

   for (i = 0; i < count; ++i) {
      new_path->items[i] = sk_path_expand (items[i], strlen(items[i]));
   }
   new_path->preferred  = sk_path_expand (preferred, strlen(preferred));

   Sk_FilePaths = new_path;
}

static Sk_FilePathList sk_path_find (const char *name) {

   Sk_FilePathList cursor;

   if (Sk_FilePaths == NULL) {

      /* Add the hardcoded configuration. */

      sk_path_list_create ("user",   Sk_FilePathUser,
                                          Sk_FilePathUserPreferred);

      sk_path_list_create ("config", Sk_FilePathConfig,
                                          Sk_FilePathConfigPreferred);

      sk_path_list_create ("skin",   Sk_FilePathSkin,
                                          Sk_FilePathSkinPreferred);

      sk_path_list_create ("maps",   Sk_FilePathMaps,
                                          Sk_FilePathMapsPreferred);
   }

   for (cursor = Sk_FilePaths; cursor != NULL; cursor = cursor->next) {
      if (strcasecmp(cursor->name, name) == 0) break;
   }
   return cursor;
}


/* Directory path strings operations. -------------------------------------- */

static char *sk_path_cat (const char *s1, const char *s2) {

    char *result =(char*) malloc (strlen(s1) + strlen(s2) + 4);

    sk_check_allocated (result);

    strcpy (result, s1);
    strcat (result, "/");
    strcat (result, s2);

    return result;
}


char *sk_path_join (const char *path, const char *name) {

   if (path == NULL) {
      return strdup (name);
   }
   if ( name == NULL ) {
	  return strdup( path );
   }
   return sk_path_cat (path, name);
}


char *sk_path_parent (const char *path, const char *name) {

   char *separator;
   char *full_name = sk_path_join (path, name);

   separator = strrchr (full_name, '/');
   if (separator == NULL) {
      return ".";
   }

   *separator = 0;

   return full_name;
}


void sk_path_format (char *buffer, int buffer_size, const char *path, const char *name) {

	int len1 = path ? strlen (path) + 1 : 0;
	int len2 = name ? strlen (name) : 0;

	if (len1 >= buffer_size) {
		len1 = buffer_size - 1;
	}
	if (len1 + len2 >= buffer_size) {
		len2 = buffer_size - 1 - len1;
	}

	// first copy file name, for the case where buffer and name are the same pointer
	if (len2) {
		memmove (buffer + len1, name, len2);
	}
	if (len1) {
		memmove (buffer, path, len1 - 1);
		buffer[len1 - 1] = '/';
	}
	buffer[len1 + len2] = '\0';
}


char *sk_path_skip_directories (const char *name) {

   char *result = strrchr (name, '/');

   if (result == NULL) return (char *)name;

   return result + 1;
}


char *sk_path_remove_extension (const char *name) {

   char *result;
   char *p;


   result = strdup(name);
   sk_check_allocated(result);

   p = sk_path_skip_directories (result);
   p = strrchr (p, '.');
   if (p != NULL) *p = 0;

   return result;
}


/* The standard directory paths: ------------------------------------------- */

static const char *sk_path_home (void) {

   static char *Sk_FilePathHome = NULL;

   if (Sk_FilePathHome == NULL) {

#ifdef IPHONE
      Sk_FilePathHome = strdup((char *) sk_main_home_path());
#else
      Sk_FilePathHome = getenv("HOME");
#endif

      if (Sk_FilePathHome == NULL) {
         Sk_FilePathHome = "";
      }
   }

   return Sk_FilePathHome;
}

#ifdef IPHONE
const char *sk_path_bundle (void) {
	
	static char Sk_FilePathBundle[256];
   static int initialized = 0;
   const char *bundle_path;
	
	if (!initialized) {
		
		bundle_path = sk_main_bundle_path();
		
		if (bundle_path == NULL) {
			Sk_FilePathBundle[0] = 0;
      } else {
         snprintf(Sk_FilePathBundle, sizeof(Sk_FilePathBundle), bundle_path);
		}
	}
	
	return Sk_FilePathBundle;
}

const char *sk_path_cache (void) {
	
	static char Sk_FilePathCache[256];
   static int initialized = 0;
   const char *cache_path;
	
	if (!initialized) {
		
		cache_path = sk_main_cache_path();
		
		if (cache_path == NULL) {
			Sk_FilePathCache[0] = 0;
      } else {
         snprintf(Sk_FilePathCache, sizeof(Sk_FilePathCache), cache_path);
		}
	}
	
	return Sk_FilePathCache;
}
#endif //IPHONE


const char *sk_path_user (void) {

    static char *Sk_User = NULL;

    if (Sk_User == NULL) {
        Sk_User = sk_path_cat (sk_path_home(), HOME_PREFIX);
        mkdir (Sk_User, 0770);
    }
    return Sk_User;
}


const char *sk_path_trips (void) {

    static char  Sk_DefaultTrips[] = HOME_PREFIX"/trips";
    static char *Sk_Trips = NULL;

    if (Sk_Trips == NULL) {

        Sk_Trips = getenv("SK_TRIPS");

        if (Sk_Trips == NULL) {
            Sk_Trips =
               sk_path_cat (sk_path_home(), Sk_DefaultTrips);
        }

        mkdir (Sk_Trips, 0770);
    }
    return Sk_Trips;
}


static char *sk_path_expand (const char *item, size_t length) {

   const char *expansion;
   size_t expansion_length;
   char *expanded;

   switch (item[0]) {
      case '~': expansion = sk_path_home(); item++; length--; break;
      case '&': expansion = sk_path_user(); item++; length--; break;
#ifdef IPHONE
      case '+': expansion = sk_path_bundle(); item++; length--; break;
      case '#': expansion = sk_path_cache(); item++; length--; break;
#endif //IPHONE
      default:  expansion = "";
   }
   expansion_length = strlen(expansion);

   expanded =(char*) malloc (length + expansion_length + 1);
   sk_check_allocated(expanded);

   //strcpy (expanded, expansion);
   //strncat (expanded, item, length);
   snprintf(expanded, length + expansion_length + 1, "%s%s", expansion, item);

   //expanded[length+expansion_length] = 0;

   return expanded;
}


/* Path lists operations. -------------------------------------------------- */

void sk_path_set (const char *name, const char *path) {

   int i;
   size_t count;
   const char *item;
   const char *next_item;

   Sk_FilePathList path_list = sk_path_find (name);


   if (path_list == NULL) {
      sk_log(SK_FATAL, "unknown path set '%s'", name);
   }

   while (*path == ',') path += 1;
   if (*path == 0) return; /* Ignore empty path: current is better. */


   if (path_list->items != NULL) {

      /* This replaces a path that was already set. */

      for (i = path_list->count-1; i >= 0; --i) {
         free (path_list->items[i]);
      }
      free (path_list->items);
   }


   /* Count the number of items in this path string. */

   count = 0;
   for (item = path-1; item != NULL; item = strchr (item+1, ',')) {
      count += 1;
   }

   path_list->items = (char**)calloc (count, sizeof(char *));
   sk_check_allocated(path_list->items);


   /* Extract and expand each item of the path.
    * Ignore directories that do not exist yet.
    */
   for (i = 0, item = path-1; item != NULL; item = next_item) {

      item += 1;
      next_item = strchr (item, ',');

      if (next_item == NULL) {
         path_list->items[i] = sk_path_expand (item, strlen(item));
      } else {
         path_list->items[i] =
            sk_path_expand (item, (size_t)(next_item - item));
      }

      //AviR patch - do not ignore directories that do not exist
      if (true /*sk_file_exists(NULL, path_list->items[i])*/) {
         ++i;
      } else {
         free (path_list->items[i]);
         path_list->items[i] = NULL;
      }
   }
   path_list->count = i;
}


const char *sk_path_first (const char *name) {

   Sk_FilePathList path_list = sk_path_find (name);

   if (path_list == NULL) {
      sk_log (SK_FATAL, "invalid path set '%s'", name);
   }

   if (path_list->count > 0) {
      return path_list->items[0];
   }

   return NULL;
}


const char *sk_path_next  (const char *name, const char *current) {

   int i;
   Sk_FilePathList path_list = sk_path_find (name);


   for (i = 0; i < path_list->count-1; ++i) {

      if (path_list->items[i] == current) {
         return path_list->items[i+1];
      }
   }

   return NULL;
}


const char *sk_path_last (const char *name) {

   Sk_FilePathList path_list = sk_path_find (name);

   if (path_list == NULL) {
      sk_log (SK_FATAL, "invalid path set '%s'", name);
   }

   if (path_list->count > 0) {
      return path_list->items[path_list->count-1];
   }
   return NULL;
}


const char *sk_path_previous (const char *name, const char *current) {

   int i;
   Sk_FilePathList path_list = sk_path_find (name);

   for (i = path_list->count-1; i > 0; --i) {

      if (path_list->items[i] == current) {
         return path_list->items[i-1];
      }
   }
   return NULL;
}


/* This function always return a hardcoded default location,
 * which is the recommended location for these objects.
 */
const char *sk_path_preferred (const char *name) {

   Sk_FilePathList path_list = sk_path_find (name);

   if (path_list == NULL) {
      sk_log (SK_FATAL, "invalid path set '%s'", name);
   }

   return path_list->preferred;
}


void sk_path_create ( const char *path )
{
	int res, status, stopFlag = 0;
	struct stat stat_buffer;
	char parent_path[512] = {0};
	char *pNext = parent_path;
	char delim = '/';
   
	strncpy( parent_path, path, 512 );
   
	while( !stopFlag )
	{
		pNext = strchr( pNext+1, delim );
		if ( pNext )
			*pNext = 0;
		else
			stopFlag = 1;
      
		// Check if exists and make dir if necessary
		status = stat ( parent_path, &stat_buffer );
		if ( status )
		{
			res = mkdir( parent_path, 0777 );
			if ( res < 0 && errno != EEXIST )	// Path exists in this case is not an error
			{
				sk_log( SK_ERROR, "Error creating path: %s, Error: %d, %s", path, errno, strerror( errno ) );
				stopFlag = 1;
			}
		}
		if ( pNext )
			*pNext = delim;
	}
}


static char *Sk_FilePathEmptyList = NULL;

char **sk_path_list (const char *path, const char *extension) {

   char  *match;
   int    length;
   size_t count;
   char  **result;
   char  **cursor;

   DIR *directory;
   struct dirent *entry;


   directory = opendir (path);
   if (directory == NULL) return &Sk_FilePathEmptyList;

   count = 0;
   while ((entry = readdir(directory)) != NULL) ++count;

   cursor = result =(char**) calloc (count+1, sizeof(char *));
   sk_check_allocated (result);

   rewinddir (directory);
   if (extension != NULL) {
      length = strlen(extension);
   } else {
      length = 0;
   }

   while ((entry = readdir(directory)) != NULL) {

      if (entry->d_name[0] == '.') continue;

      if (length > 0) {

         match = entry->d_name + strlen(entry->d_name) - length;

         if (! strcmp (match, extension)) {
            *(cursor++) = strdup (entry->d_name);
         }
      } else {
         *(cursor++) = strdup (entry->d_name);
      }
   }
   *cursor = NULL;
   closedir(directory);

   return result;
}


void   sk_path_list_free (char **list) {

   char **cursor;

   if ((list == NULL) || (list == &Sk_FilePathEmptyList)) return;

   for (cursor = list; *cursor != NULL; ++cursor) {
      free (*cursor);
   }
   free (list);
}


void sk_path_free (const char *path) {
   free ((char *) path);
}


const char *sk_path_search_icon (const char *name) {

   static char result[256];

   sprintf (result, "%s/pixmaps/rm_%s.png", sk_path_home(), name);
   if (sk_file_exists(NULL, result)) return result;

   sprintf (result, "/usr/local/share/pixmaps/rm_%s.png", name);
   if (sk_file_exists(NULL, result)) return result;

   sprintf (result, "/usr/share/pixmaps/rm_%s.png", name);
   if (sk_file_exists(NULL, result)) return result;

   return NULL; /* Not found. */
}


int sk_path_is_full_path (const char *name) {
   return name[0] == '/';
}


int sk_path_is_directory (const char *name) {

   struct stat file_attributes;

   if (stat (name, &file_attributes) != 0) {
      return 0;
   }

   return S_ISDIR(file_attributes.st_mode);
}


const char *sk_path_temporary (void) {

   return "/var/tmp";
}

const char *sk_path_gps( void )
{
   static char *Sk_FilePathGps = NULL;

   if (Sk_FilePathGps == NULL)
   {
	  Sk_FilePathGps = sk_path_join( sk_path_user(), Sk_FilePathGpsSuffix );
	  sk_path_create( Sk_FilePathGps );
   }
   return Sk_FilePathGps;
}

const char *sk_path_images( void )
{
   static char *Sk_FilePathImages = NULL;

   if ( Sk_FilePathImages == NULL )
   {
	  Sk_FilePathImages = sk_path_join( sk_path_user(), "images" );
	  sk_path_create( Sk_FilePathImages );
   }
   return Sk_FilePathImages;
}

const char *sk_path_voices( void )
{
   static char *Sk_FilePathVoices = NULL;
   
   if ( Sk_FilePathVoices == NULL )
   {
      Sk_FilePathVoices = sk_path_join( sk_path_user(), "voices" );
      sk_path_create( Sk_FilePathVoices );
   }
   return Sk_FilePathVoices;
}

const char *sk_path_tts( void )
{
   static char *Sk_FilePathTts = NULL;

   if ( Sk_FilePathTts == NULL )
   {
      Sk_FilePathTts = sk_path_join( sk_path_user(), "tts" );
      sk_path_create( Sk_FilePathTts );
   }
   return Sk_FilePathTts;
}

const char *sk_path_downloads( void )
{
   return sk_path_user();
}

const char *sk_path_debug( void )
{
   static char *Sk_FilePathDebug = NULL;

   if ( Sk_FilePathDebug == NULL )
   {
      Sk_FilePathDebug = sk_path_join( sk_path_user(), "debug" );
      sk_path_create( Sk_FilePathDebug );
   }
   return Sk_FilePathDebug;
}

const char *sk_path_config( void )
{
	return sk_path_user();
}

#pragma endregion

#pragma region Sk_File

FILE *sk_file_fopen (const char *path,
                          const char *name,
                          const char *mode) {

   int   silent;
   FILE *file;
   const char *full_name = sk_path_join (path, name);

   if (mode[0] == 's') {
      /* This special mode is a "lenient" read: do not complain
       * if the file does not exist.
       */
      silent = 1;
      ++mode;
   } else {
      silent = 0;
   }

   file = fopen (full_name, mode);

   if ((file == NULL) && (! silent)) {
      sk_log (SK_ERROR, "cannot open file %s", full_name);
   }

   sk_path_free (full_name);
   return file;
}


void sk_file_remove (const char *path, const char *name) {

   const char *full_name = sk_path_join (path, name);

   remove(full_name);

   sk_path_free (full_name);
}

/*
 * Removing non empty directory
 */
void sk_file_rmdir( const char *path, const char *name )
{

   const char *full_name = sk_path_join (path, name);

   char cmd[1024];
   snprintf( cmd, 1024, "rm -r %s", full_name );
   system( cmd );

   sk_path_free (full_name);
}

int sk_file_exists (const char *path, const char *name) {

   int   status;
   const char *full_name = sk_path_join (path, name);
   struct stat stat_buffer;

   status = stat (full_name, &stat_buffer);

   sk_path_free (full_name);

   return (status == 0);
}


int sk_file_length (const char *path, const char *name) {

   int   status;
   const char *full_name = sk_path_join (path, name);
   struct stat stat_buffer;

   status = stat (full_name, &stat_buffer);
   sk_path_free (full_name);

   if (status == 0) {
      return stat_buffer.st_size;
   }
   return -1;
}


void sk_file_save (const char *path, const char *name,
                        void *data, int length) {

   int   fd;
   const char *full_name = sk_path_join (path, name);

   fd = open (full_name, O_CREAT+O_WRONLY+O_TRUNC, 0666);
   sk_path_free (full_name);

   if (fd >= 0) {

      write (fd, data, length);
      close(fd);
   }
}


int sk_file_truncate (const char *path, const char *name,
                           int length) {

   int   res;
   const char *full_name = sk_path_join (path, name);

   res = truncate (full_name, length);
   sk_path_free (full_name);

   return res;
}

int sk_file_rename (const char *old_name, const char *new_name) {

   return rename (old_name, new_name);
}

void sk_file_append (const char *path, const char *name,
                          void *data, int length) {

   int   fd;
   const char *full_name = sk_path_join (path, name);

   fd = open (full_name, O_CREAT+O_WRONLY+O_APPEND, 0666);
   sk_path_free (full_name);

   if (fd >= 0) {

      write (fd, data, length);
      close(fd);
   }
}


const char *sk_file_unique (const char *base) {

    static int   UniqueNameCounter = 0;
    static char *UniqueNameBuffer = NULL;
    static int   UniqueNameBufferLength = 0;

    int length;
    
    length = strlen(base + 16);
    
    if (length > UniqueNameBufferLength) {

        if (UniqueNameBuffer != NULL) {
            free(UniqueNameBuffer);
        }
        UniqueNameBuffer = (char*)malloc (length);
        
        sk_check_allocated(UniqueNameBuffer);
        
        UniqueNameBufferLength = length;
    }
    
    sprintf (UniqueNameBuffer,
             "%s%d_%d", base, getpid(), UniqueNameCounter);

    UniqueNameCounter += 1;

    return UniqueNameBuffer;
}


const char *sk_file_map (const char *set,
                              const char *name,
                              const char *sequence,
                              const char *mode,
                              Sk_FileContext *file) {

   Sk_FileContext context;

   struct stat state_result;
   int open_mode;
   int map_mode;


   context = (Sk_FileContext)malloc (sizeof(*context));
   sk_check_allocated(context);

   context->fd = -1;
   context->base = NULL;
   context->size = 0;

   if (strcmp(mode, "r") == 0) {
      open_mode = O_RDONLY;
      map_mode = PROT_READ;
   } else if (strchr (mode, 'w') != NULL) {
      open_mode = O_RDWR;
      map_mode = PROT_READ|PROT_WRITE;
   } else {
      sk_log (SK_ERROR,
                   "%s: invalid file access mode %s", name, mode);
      free (context);
      return NULL;
   }

   if (name[0] == '/') {

      context->fd = open (name, open_mode, 0666);
      sequence = ""; /* Whatever, but NULL. */

   } else {

      char *full_name;

      int full_name_size;
      int name_size = strlen(name);
      int size;


      if (sequence == NULL) {
         sequence = sk_path_first(set);
      } else {
         sequence = sk_path_next(set, sequence);
      }
      if (sequence == NULL) {
         free (context);
         return NULL;
      }

      full_name_size = 512;
      full_name =(char*) malloc (full_name_size);
      sk_check_allocated(full_name);

      do {
         size = strlen(sequence) + name_size + 2;

         if (size >= full_name_size) {
            full_name = (char*)realloc (full_name, size);
            sk_check_allocated(full_name);
            full_name_size = size;
         }

         strcpy (full_name, sequence);
         strcat (full_name, "/");
         strcat (full_name, name);

         context->fd = open (full_name, open_mode, 0666);

         if (context->fd >= 0) break;

         sequence = sk_path_next(set, sequence);

      } while (sequence != NULL);

      free (full_name);
   }

   if (context->fd < 0) {
      if (sequence == 0) {
         sk_log (SK_INFO, "cannot open file %s", name);
      }
      sk_file_unmap (&context);
      return NULL;
   }

   if (fstat (context->fd, &state_result) != 0) {
      if (sequence == 0) {
         sk_log (SK_ERROR, "cannot stat file %s", name);
      }
      sk_file_unmap (&context);
      return NULL;
   }
   context->size = state_result.st_size;

   context->base =
      mmap (NULL, state_result.st_size, map_mode, MAP_SHARED, context->fd, 0);

   if (context->base == NULL) {
      sk_log (SK_ERROR, "cannot map file %s", name);
      sk_file_unmap (&context);
      return NULL;
   }

   *file = context;

   return sequence; /* Indicate the next directory in the path. */
}


void *sk_file_base (Sk_FileContext file){

   if (file == NULL) {
      return NULL;
   }
   return file->base;
}


int sk_file_size (Sk_FileContext file){

   if (file == NULL) {
      return 0;
   }
   return file->size;
}


int sk_file_sync (Sk_FileContext file) {

   if (file->base != NULL) {
      return msync (file->base, file->size, MS_SYNC);
   }

   return -1;
}


void sk_file_unmap (Sk_FileContext *file) {

   Sk_FileContext context = *file;

   if (context->base != NULL) {
      munmap (context->base, context->size);
   }

   if (context->fd >= 0) {
      close (context->fd);
   }
   free(context);
   *file = NULL;
}


SK_FILE sk_file_open  (const char *name, const char *mode) {

   int unix_mode = 0;

   if (strcmp(mode, "r") == 0) {
      unix_mode = O_RDONLY;
   } else if (strcmp (mode, "rw") == 0) {
      unix_mode = O_RDWR|O_CREAT;
   } else if (strchr (mode, 'w') != NULL) {
      unix_mode = O_RDWR|O_CREAT|O_TRUNC;
   } else if (strchr (mode, 'a') != NULL) {
      unix_mode = O_RDWR|O_CREAT|O_APPEND;
   } else {
      sk_log (SK_ERROR,
                   "%s: invalid file access mode %s", name, mode);
      return -1;
   }

   return (SK_FILE) open (name, unix_mode, 0644);
}


int sk_file_read  (SK_FILE file, void *data, int size) {
   return read ((int)file, data, size);
}

int sk_file_write (SK_FILE file, const void *data, int length) {
   return write ((int)file, data, length);
}

int sk_file_seek (SK_FILE file, int offset, Sk_SeekWhence whence) {

	int unix_whence;
	
	switch (whence) {
		case SK_SEEK_START:
			unix_whence = SEEK_SET;
			break;
		case SK_SEEK_CURR:
			unix_whence = SEEK_CUR;
			break;
		case SK_SEEK_END:
			unix_whence = SEEK_END;
			break;
		default:
	      sk_log (SK_ERROR,
	                   "invalid file seek whence %d", (int)whence);
	      return -1;
	}
	
	return lseek ((int)file, offset, unix_whence);
} 

void  sk_file_close (SK_FILE file) {
   close ((int)file);
}

int sk_file_free_space (const char *path) {
   return -1;
}


#pragma endregion


#endif