/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_GFXBATCH_H
#define HPL_GFXBATCH_H

#include "math/MathTypes.h"
#include "graphics/Color.h"
#include "graphics/VertexBuffer.h"

namespace hpl {

	class iLowLevelGraphics;

	typedef uint32_t tGfxBatchAttrs;

	enum eGfxBatchAttr {
		eGfxBatchAttr_Position = 0x0001,
		eGfxBatchAttr_Color0 =   0x0002,
		eGfxBatchAttr_Texture0 = 0x0004
	};

	enum eBatchDrawMode
	{
		eBatchDrawMode_Tris,
		eBatchDrawMode_Quads
	};

	class cGfxBatch
	{
	public:
		cGfxBatch(iLowLevelGraphics *llGfx);
		cGfxBatch(int vertexCount, iLowLevelGraphics *llGfx);
		~cGfxBatch();

		void AddVertex(const cVector3f &position, const cColor &color, const cVector2f& tex);
		void Clear();
		bool HasContent() { return indexCount > 0; }

		iVertexBuffer *vertexBuffer;
		STLBufferIterator<cVector3f> posView;
		STLBufferIterator<cColor> colorView;
		STLBufferIterator<cVector2f> uvView;
		STLBufferIterator<int> indexView;
		int indexCount;
	};
}

#endif /* HPL_GFXBATCH_H */
