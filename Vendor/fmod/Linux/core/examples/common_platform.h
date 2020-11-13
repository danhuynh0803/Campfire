/*==============================================================================
FMOD Example Framework
Copyright (c), Firelight Technologies Pty, Ltd 2014-2020.
==============================================================================*/
#include <pthread.h>
#include <assert.h>

#define COMMON_PLATFORM_SUPPORTS_FOPEN

#define FMOD_Main() main(int, char**)
#define Common_TTY(format, ...) fprintf(stderr, format, __VA_ARGS__)

typedef pthread_mutex_t Common_Mutex;
