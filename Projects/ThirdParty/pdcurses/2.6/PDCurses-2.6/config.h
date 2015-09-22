/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if your compiler supports ANSI prototypes  */
#define HAVE_PROTO 1

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define as the system defined limit for number of signals */
#define PDC_MAX_SIGNALS NSIG

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the poll function.  */
/* #undef HAVE_POLL */

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the usleep function.  */
#define HAVE_USLEEP 1

/* Define if you have the vsscanf function.  */
#define HAVE_VSSCANF 1

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/select.h> header file.  */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* following manually added as autoheader can't recognise them */

/* Define if you have the <DECkeySym.h> header file.  */
/* #undef HAVE_DECKEYSYM_H */

/* Define if you have the <Sunkeysym.h> header file.  */
/* #undef HAVE_SUNKEYSYM_H */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_DELETE */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_INSERT */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_END */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_DOWN */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_NEXT */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_LEFT */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_RIGHT */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_HOME */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_UP */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_PRIOR */

/* Define if you have this defined in <keysym.h>.  */
/* #undef HAVE_XK_KP_BEGIN */

/* Define if you want to use Xaw3d library  */
/* #undef USE_XAW3D */

/* Define if you want to use neXtaw library  */
/* #undef USE_NEXTAW */

/* Define if you want to include LATIN1 key support */
#define INCLUDE_LATIN1 1

/* Define if you want to build XCurses with threads */
/* #undef USE_THREADS */

/* Define if you want to build XCurses with processes */
#define USE_PROCESSES 1

/* Define XPointer is typedefed in X11/Xlib.h */
#define XPOINTER_TYPEDEFED 1
