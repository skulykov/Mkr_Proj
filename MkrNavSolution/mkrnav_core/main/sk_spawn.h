/* sk_spawn.h - Process control interface for the MkrNav application.
 *

 */

#ifndef INCLUDE__SK_SPAWN__H
#define INCLUDE__SK_SPAWN__H

#include "sk_list.h"


typedef int Sk_Pipe; /* UNIX style. */
#define SK_SPAWN_INVALID_PIPE   -1
#define SK_SPAWN_IS_VALID_PIPE(x) (x != SK_SPAWN_INVALID_PIPE)


typedef void (*Sk_FeedbackHandler) (void *data);

struct sk_spawn_feedback {
    
    Sk_ListItem link;
    int child;

    Sk_FeedbackHandler handler;
    void *data;
};

typedef struct sk_spawn_feedback Sk_Feedback;
    

void sk_spawn_initialize (const char *argv0);

int  sk_spawn (const char *name, const char *command_line);

int  sk_spawn_with_feedback
         (const char *name,
          const char *command_line,
          Sk_Feedback *feedback);

int  sk_spawn_with_pipe
         (const char *name,
          const char *command_line,
          Sk_Pipe pipes[2],      /* pipes[0] -> read, pipes[1] -> write */
          Sk_Feedback *feedback);

int  sk_spawn_write_pipe (Sk_Pipe pipe, const void *data, int length);
int  sk_spawn_read_pipe  (Sk_Pipe pipe, void *data, int size);
void sk_spawn_close_pipe (Sk_Pipe pipe);

void sk_spawn_check (void);

void sk_spawn_command (const char *command);

#endif // INCLUDE__SK_SPAWN__H

