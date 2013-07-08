#ifndef kcwsh_hooks
#define kcwsh_hooks

#include <windows.h>

DWORD kcwshInputHook(void *val);

DWORD kcwshOutputHook(void *val);

#endif /* kcwsh_hooks */