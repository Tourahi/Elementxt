#pragma once


/* ==============================================================
 *
 *                          HELPERS
 *
 * ===============================================================*/

// delay to ensure proper token pasting while using ##
#define GUI_STRING_JOIN_IMMEDIATE(arg1, arg2) arg1 ## arg2
#define GUI_STRING_JOIN_DELAY(arg1, arg2) GUI_STRING_JOIN_IMMEDIATE(arg1, arg2)
#define GUI_STRING_JOIN(arg1, arg2) GUI_STRING_JOIN_DELAY(arg1, arg2)

#ifdef _MSC_VER
  #define GUI_UNIQUE_NAME(name) GUI_STRING_JOIN(name, __COUNTER__)
#else
  #define GUI_UNIQUE_NAME(name) GUI_STRING_JOIN(name, __LINE__)
#endif

#ifndef GUI_STATIC_ASSERT
  // The compiler will go creazy if the dummy array got a negative size.
  #define GUI_STATIC_ASSERT(exp) typedef char GUI_UNIQUE_NAME(_dummy_arr)[(exp)? 1:-1]
#endif


/* ==============================================================
 *
 *                          BASICS
 *
 * ===============================================================*/

#ifndef GUI_API
  #ifdef GUI_PRIVATE
    #if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199409L))
      #define GUI_API static inline
    #elif defined(__cplusplus)
      #define GUI_API static inline
    #else
      #define GUI_API static
    #endif
  #else
    #define GUI_API extern
  #endif
#endif

#ifndef GUI_LIB
  #ifdef GUI_SINGLE_FILE
    #define GUI_LIB static
  #else
    #define GUI_LIB extern
  #endif
#endif



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

typedef GUI_INT8    guiChar;
typedef GUI_UINT8   guiuChar;
typedef GUI_UINT8   guiByte;
typedef GUI_INT16   guiShort;
typedef GUI_UINT16  guiuShort;
typedef GUI_INT32   guiInt;
typedef GUI_UINT32  guiuInt;

typedef GUI_SIZE_TYPE    guiSize;
typedef GUI_POINTER_TYPE guiPtr;
typedef GUI_BOOL         guiBool;

typedef guiuInt guiHash;
typedef guiuInt guiFlags;
typedef guiuInt guiRune; // unicode

/* Asserts  */
GUI_STATIC_ASSERT(sizeof(guiShort) == 2);
GUI_STATIC_ASSERT(sizeof(guiuShort) == 2);
GUI_STATIC_ASSERT(sizeof(guiuInt) == 4);
GUI_STATIC_ASSERT(sizeof(guiInt) == 4);
GUI_STATIC_ASSERT(sizeof(guiByte) == 1);
GUI_STATIC_ASSERT(sizeof(guiFlags) == 4);
GUI_STATIC_ASSERT(sizeof(guiRune) == 4);
GUI_STATIC_ASSERT(sizeof(guiSize) >= sizeof(void*));
GUI_STATIC_ASSERT(sizeof(guiPtr) >= sizeof(void*));

#ifdef GUI_INCLUDE_STANDARD_BOOL
GUI_STATIC_ASSERT(sizeof(guiBool) == sizeof(bool));
#else
GUI_STATIC_ASSERT(sizeof(guiBool) >= 2);
#endif



/* ==============================================================
 *
 *                          API
 *
 * ===============================================================*/

typedef union { void *ptr; int id; } guiHandle;
typedef void*(*guiPluginAlloc)(guiHandle, void *old, guiSize);
typedef void(*guiPluginFree)(guiHandle, void *old);


struct guiAllocator {
  guiHandle userdata;
  guiPluginAlloc alloc;
  guiPluginFree free;
};



/* ==============================================================
 *
 *                          MEM BUFFER
 *
 * ===============================================================*/

struct guiMemoryStatus {
  void *memory;
  unsigned int type;
  guiSize size;
  guiSize allocated;
  guiSize needed;
  guiSize calls;
};

// trNote : make them clas enums??
enum guiAllocationType {
  GUI_BUFFER_FIXED, // fixed size buffer
  GUI_BUFFER_DYNAMIC // dynamic size buffer
};

enum guiBufferAllocationType {
  GUI_BUFFER_FRONT,
  GUI_BUFFER_BACK,
  GUI_BUFFER_MAX
};

struct guiBufferMaker {
  guiBool active;
  guiSize offset;
};

struct guiMemory
{
  void *ptr;
  guiSize size; // trNote : 32bits in my case
};

struct guiBuffer {
  
  struct guiBufferMaker maker[GUI_BUFFER_MAX]; // To free a buffer to a certain offset
  struct guiAllocator pool; // allocator callback for dynmc buffers
  enum guiAllocationType type; // mem management type
  struct guiMemory memory; // mem and size of the current mem block
  double growFactor; // for dynamic mem management
  guiSize allocated; // total currently allocated
  guiSize needed; // totally consumed memory given that enough memory is present
  guiSize calls; // nbr of alloc calls
  guiSize size; // current size of the buffer
  
};


GUI_API void guiBufferInit(struct guiBuffer*, const struct guiAllocator*, guiSize size);
GUI_API void guiBufferInitFixed(struct guiBuffer*, void *memory, guiSize size);
GUI_API void guiBufferInfo(struct guiMemoryStatus*, void *memory, struct guiBuffer*);





/* ==============================================================
 *                          ALIGNMENT
 * =============================================================== */

#if defined(__PTRDIFF_TYPE__)
# define GUI_PTR_TO_UINT(x) ((guiSize)(__PTRDIFF_TYPE__)(x))
# define GUI_UINT_TO_PTR(x) ((void*)(__PTRDIFF_TYPE__)(x))
#elif !defined(__GNUC__)
# define GUI_UINT_TO_PTR(x) ((void*)&((char*)0)[x])
# define GUI_PTR_TO_UINT(x) ((guiSize)(((char*)x)-(char*)0))
#elif !defined(GUI_INCLUDE_FIXED_TYPES)
# define GUI_UINT_TO_PTR(x) ((void*)(uintptr_t)(x))
# define GUI_PTR_TO_UINT(x) ((uintptr_t)(x))
#else
# define GUI_UINT_TO_PTR(x) ((void*)(x))
# define GUI_PTR_TO_UINT(x) ((guiSize)(x))
#endif

#define GUI_ALIGN_PTR(x, mask)\
  (GUI_UINT_TO_PTR((GUI_PTR_TO_UINT((guiByte*)(x) + (mask-1)) & ~(mask-1))))

#define GUI_ALIGN_PTR_BACK(x, mask)\
  (GUI_UINT_TO_PTR((GUI_PTR_TO_UINT((guiByte*)(x)) & ~(mask-1))))

#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
#define GUI_OFFSETOF(st, m) (__builtin_offsetof(st, m))
#else
#define GUI_OFFSETOF(st, m) ((guiPtr)&(((st*)0)->m))
#endif

#ifdef __cplusplus

template<typename T> struct guiAlignOf;

template<typename T, int sizeDiff>
struct guiHelperAlign {
  enum {value = sizeDiff};
};

// specialized guiHelperAlign in case of sizeDif == 0
template<typename T>
struct guiHelperAlign<T, 0> {
  enum {value = guiAlignOf<T>::value};
};


template<typename T> struct guiAlignOf {
  struct Big {T x; char c;}; 
  enum {
    diff = sizeof(Big) - sizeof(T), value = guiHelperAlign<Big, diff>::value
  };
};

#define GUI_ALIGNOF(t) (guiAlignOf<t>::value)
#else
#define GUI_ALIGNOF(t) GUI_OFFSETOF(struct {char c; t _h;}, _h)
#endif


/* ==============================================================
 *                          MATH
 * =============================================================== */

#define guiPtrAdd(t, p, i) ((t*)((void*)((guiByte*)(p) + (i))))