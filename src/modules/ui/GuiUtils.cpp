#include "Gui.hpp"
#include "GuiInternal.hpp"

GUI_LIB void guiMemset(void *ptr, int c0, guiSize size)
{

	#define guiWord unsigned
	#define guiWsize sizeof(guiWord)
	#define guiWmask (guiWsize - 1)

	guiByte *dst = (guiByte*) ptr;
	unsigned c = 0;
	guiSize t = 0;

	/// Expanding c size
	if ((c = (guiByte)c0) != 0) {
		c = (c << 8) | c;
		if (sizeof(unsigned int) > 2)
			c = (c << 16) | c;
	}

	dst = (guiByte*)ptr;
	/// More efficient to go bite by bite in this case
	if (size < 3 * guiWsize){
		while (size--) *dst++ = (guiByte)c0;
		return;
	}

	/// Align dest
	if ((t = GUI_PTR_TO_UINT(dst) & guiWmask) != 0) {
		t = guiWsize - t;
		size -= t;
		do {
			*dst++ = (guiByte)c0;
		} while (--t != 0);
	}

	/// Fill words
	t = size / guiWsize;
	do {
		*(guiWord*)((void*)dst) = c;
		dst += guiWsize;
	} while (--t != 0);

	/// Fill trailing bytes
	t = (size & guiWmask);
	if (t != 0) {
		do {
			*dst++ = (guiByte)c0;
		} while (--t != 0);
	}

	#undef guiWord
	#undef guiWsize
	#undef guiWmask
}



GUI_LIB void guiZero(void *ptr, guiSize size)
{
	GUI_ASSERT(ptr);

}


