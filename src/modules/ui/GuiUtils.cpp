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


}



GUI_LIB void guiZero(void *ptr, guiSize size)
{
	GUI_ASSERT(ptr);

}


