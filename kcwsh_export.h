#ifndef kcwsh_export
#define kcwsh_export

#ifdef kcwsh_EXPORTS
#define KCWSH_EXPORT __declspec(dllexport)
#else
#define KCWSH_EXPORT __declspec(dllimport)
#endif

#ifdef kcwshqt_EXPORTS
#define KCWSHQT_EXPORT __declspec(dllexport)
#else
#define KCWSHQT_EXPORT __declspec(dllimport)
#endif

#endif /* kcwsh_export */