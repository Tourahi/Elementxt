
namespace Gui
{

/* ==============================================================
 *
 *                          BASICS
 *
 * ===============================================================*/

#ifdef GUI_INCLUDE_FIXED_TYPES
  #include <cstdint>
  #define GUI_INT8 int8_t
  #define GUI_UINT8 uint8_t
  #define GUI_INT16 int16_t
  #define GUI_UINT16 uint16_t
  #define GUI_INT32 int32_t
  #define GUI_UINT32 uint32_t
  #define GUI_SIZE_TYPE uintptr_t
  #define GUI_POINTER_TYPE uint32_t
#else
  #ifndef GUI_INT8
    #define GUI_INT8 signed char
  #endif
  #ifndef GUI_UINT8
    #define GUI_UINT8 unsigned char
  #endif
  #ifndef GUI_INT16
    #define GUI_INT16 signed short
  #endif
  #ifndef GUI_UINT16
    #define GUI_UINT16 unsigned short
  #endif

  #ifndef GUI_INT32
    #if defined(_MSC_VER)
      #define GUI_INT32 __int32
    #else
      #define GUI_INT32 signed int
    #endif
  #endif
   #ifndef GUI_UINT32
    #if defined(_MSC_VER)
      #define GUI_UINT32 unsigned __int32
    #else
      #define GUI_UINT32 unsigned int
    #endif
  #endif

  #ifndef GUI_SIZE_TYPE
    #if defined(_WIN64) && defined(_MSC_VER)
      #define GUI_SIZE_TYPE unsigned __int64
    #elif (defined(_WIN32) || defined(WIN32)) && defined(_MSC_VER)
      #define GUI_SIZE_TYPE unsigned __int32
    #elif defined(__GNUC__) || defined(__clang__)
      #if defined(__x86_64__) || defined(__ppc64__) || defined(__PPC64__) || defined(__aarch64__)
        #define GUI_SIZE_TYPE unsigned long
      #else
        #define GUI_SIZE_TYPE unsigned int
      #endif
    #else
      #define GUI_SIZE_TYPE unsigned long
    #endif
  #endif
  #ifndef GUI_POINTER_TYPE
    #if defined(_WIN64) && defined(_MSC_VER)
      #define GUI_POINTER_TYPE unsigned __int64
    #elif (defined(_WIN32) || defined(WIN32)) && defined(_MSC_VER)
      #define GUI_POINTER_TYPE unsigned __int32
    #elif defined(__GNUC__) || defined(__clang__)
      #if defined(__x86_64__) || defined(__ppc64__) || defined(__PPC64__) || defined(__aarch64__)
        #define GUI_POINTER_TYPE unsigned long
      #else
        #define GUI_POINTER_TYPE unsigned int
      #endif
    #else
      #define GUI_POINTER_TYPE unsigned long
    #endif
  #endif
#endif

#ifndef GUI_BOOL
  #ifdef GUI_INCLUDE_STANDARD_BOOL
    #include <cstdbool>
    #define GUI_BOOL bool
  #else
    #define GUI_BOOL int
  #endif
#endif


}
