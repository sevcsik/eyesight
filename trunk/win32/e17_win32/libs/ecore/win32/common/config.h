/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Build Ecore_Con Module */
/* #undef BUILD_ECORE_CON */

/* Build Ecore_Config Module */
/* #undef BUILD_ECORE_CONFIG */

/* Build Ecore_Desktop Module */
/* #undef BUILD_ECORE_DESKTOP */

/* Build Ecore_DirectFB Module */
/* #undef BUILD_ECORE_DIRECTFB */

/* Build Ecore_Evas Module */
#define BUILD_ECORE_EVAS 1

/* Support for Buffers in Ecore_Evas */
/* #define BUILD_ECORE_EVAS_BUFFER 1 */

/* Support for Direct3D Engine in Ecore_Evas */
#define BUILD_ECORE_EVAS_DIRECT3D 1

/* Support for DirectDraw Engine in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_DIRECTDRAW */

/* Support for 16 bpp DirectDraw Engine in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_DIRECTDRAW_16 */

/* Support for DirectFB in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_DIRECTFB */

/* Support for Linux FB in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_FB */

/* Support for SDL in Ecore_Evas */
/* #define BUILD_ECORE_EVAS_SDL 1 */

/* Support for SVN SDL */
/* #define BUILD_ECORE_EVAS_SDL_130 1 */

/* Support for X11 Engine in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_X11 */

/* Support for X11 16bpp Engine in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_X11_16 */

/* Support for GL Engine in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_X11_GL */

/* Support for XRender Engine in Ecore_Evas */
/* #undef BUILD_ECORE_EVAS_XRENDER */

/* Build Ecore_FB Module */
/* #undef BUILD_ECORE_FB */

/* Build Ecore_File Module */
#define BUILD_ECORE_FILE 1

/* Build Ecore_IMF Module */
/* #define BUILD_ECORE_IMF 1 */

/* Build Ecore_IMF_EVAS Module */
/* #define BUILD_ECORE_IMF_EVAS 1 */

/* Build Ecore_Ipc Module */
/* #undef BUILD_ECORE_IPC */

/* Build Ecore_Job Module */
#define BUILD_ECORE_JOB 1

/* Build Ecore_SDL Module */
/* #define BUILD_ECORE_SDL 1 */

/* Build Ecore_Txt Module */
#define BUILD_ECORE_TXT 1

/* Build Ecore_Win32 Module */
#define BUILD_ECORE_WIN32 1

/* Build Ecore_X Module */
/* #undef BUILD_ECORE_X */

/* Build support for XCB composite */
/* #undef ECORE_XCB_COMPOSITE */

/* Build support for XCB damage */
/* #undef ECORE_XCB_DAMAGE */

/* Build support for XCB dpms */
/* #undef ECORE_XCB_DPMS */

/* Build support for XCB xfixes */
/* #undef ECORE_XCB_FIXES */

/* Build support for XCB randr */
/* #undef ECORE_XCB_RANDR */

/* Build support for XCB render */
/* #undef ECORE_XCB_RENDER */

/* Build support for XCB screensaver */
/* #undef ECORE_XCB_SCREENSAVER */

/* Build support for XCB shape */
/* #undef ECORE_XCB_SHAPE */

/* Build support for XCB sync */
/* #undef ECORE_XCB_SYNC */

/* Build support for XCB xinerama */
/* #undef ECORE_XCB_XINERAMA */

/* Build support for XCB xprint */
/* #undef ECORE_XCB_XPRINT */

/* Build support for XCB xtest */
/* #undef ECORE_XCB_XTEST */

/* Build support for Xcomposite */
/* #undef ECORE_XCOMPOSITE */

/* Build support for Xcursor */
/* #undef ECORE_XCURSOR */

/* Build support for Xdamage */
/* #undef ECORE_XDAMAGE */

/* Build support for Xdpms */
/* #undef ECORE_XDPMS */

/* Build support for Xfixes */
/* #undef ECORE_XFIXES */

/* Build support for Xinerama */
/* #undef ECORE_XINERAMA */

/* Build support for Xprint */
/* #undef ECORE_XPRINT */

/* Build support for Xrandr */
/* #undef ECORE_XRANDR */

/* Build support for Xrender */
/* #undef ECORE_XRENDER */

/* Build support for Xss */
/* #undef ECORE_XSS */

/* Build support for Xtest */
/* #undef ECORE_XTEST */

/* Have abstract sockets namespace */
#define HAVE_ABSTRACT_SOCKETS 1

/* Downloading with CURL */
/* #undef HAVE_CURL */

/* Build Ecore_Win32 with Direct3D Code */
#define HAVE_DIRECT3D 1

/* Build Ecore_Win32 with DirectDraw Code */
#define HAVE_DIRECTDRAW 1

/* Build Ecore_Win32 with 16 bpp DirectDraw Code */
/* #undef HAVE_DIRECTDRAW_16 */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Defined to 1 if XCB backend is enabled. */
/* #undef HAVE_ECORE_X_XCB */

/* Define to 1 if you have the <features.h> header file. */
/* #undef HAVE_FEATURES_H */

/* Define to 1 if you have the `gettimeofday' function. */
/* #define HAVE_GETTIMEOFDAY 1 */

/* File monitoring with Inotify */
/* #undef HAVE_INOTIFY */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <langinfo.h> header file. */
/* #undef HAVE_LANGINFO_H */

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
/* #undef HAVE_NETINET_IN_H */

/* Build Ecore_Win32 with OpenGL Glew Code */
/* #define HAVE_OPENGL_GLEW 1 */

/* File monitoring with polling */
#define HAVE_POLL 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */

/* File monitoring with Inotify - sys/inotify.h */
/* #undef HAVE_SYS_INOTIFY */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
/* #undef HAVE_SYS_WAIT_H */

/* Build Ecore_FB Touchscreen Code */
/* #undef HAVE_TSLIB */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <windows.h> header file. */
#define HAVE_WINDOWS_H 1

/* Define to 1 if you have the <winsock2.h> header file. */
#define HAVE_WINSOCK2_H 1

/* Define to 1 if your compiler has __attribute__ */
/* #define HAVE___ATTRIBUTE__ 1 */

/* Name of package */
#define PACKAGE "ecore"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "enlightenment-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "ecore"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ecore 0.9.9.042"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ecore"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.9.9.042"

/* Suffix for shared objects */
#define SHARED_LIB_SUFFIX ".dll"

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Use OpenSSL */
/* #undef USE_OPENSSL */

/* Version number of package */
#define VERSION "0.9.9.042"

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to 1 if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
