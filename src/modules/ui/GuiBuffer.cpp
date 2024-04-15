#include "Gui.hpp"
#include "GuiInternal.hpp"



GUI_API void guiBufferInit(struct guiBuffer *b, const struct guiAllocator *a, guiSize initSize)
{
	GUI_ASSERT(b);
	GUI_ASSERT(a);
	GUI_ASSERT(initSize);

	if (!a || !b || !initSize) return;

	guiZero(b, sizeof(*b));

	b->type = GUI_BUFFER_DYNAMIC;
	b->memory.ptr = a->alloc(a->userdata, 0, initSize);
	b->memory.size = initSize;
	b->size = initSize;
	b->growFactor = 2.0f;
	b->pool = *a;
}


GUI_API void guiBufferInitFixed(struct guiBuffer *b, void *memory, guiSize size)
{	
	GUI_ASSERT(b);
	GUI_ASSERT(memory);
	GUI_ASSERT(size);

	if (!memory || !b || !size) return;

	guiZero(b, sizeof(*b));

	b->type = GUI_BUFFER_FIXED;
	b->memory.ptr = memory;
	b->memory.size = size;
	b->size = size;
}

GUI_API void guiBufferInfo(struct guiMemoryStatus *s, void *memory, struct guiBuffer *b)
{
	GUI_ASSERT(s);
	GUI_ASSERT(b);

	if (!s || !b) return;

	s->allocated = b->allocated;
	s->size = b->memory.size;
	s->needed = b->needed;
	s->memory = b->memory.ptr;
	s->calls = b->calls;
}

GUI_LIB void* nk_buffer_align(void *unaligned, guiSize align, guiSize *alignment, enum guiBufferAllocationType type)
{
	
}

GUI_LIB void* guiBufferAlloc(struct guiBuffer *b, enum guiBufferAllocationType type, guiSize size, guiSize align)
{
	int full;
	guiSize alignement;
	void *unaligned;
	void *memory;

	GUI_ASSERT(b);
	GUI_ASSERT(size);




}