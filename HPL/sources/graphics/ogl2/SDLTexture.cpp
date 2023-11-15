/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#if defined(__APPLE__)&&defined(__MACH__)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "graphics/Bitmap.h"
#include "graphics/ogl2/SDLTexture.h"

#include "math/Math.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSDLTexture::cSDLTexture(const tString &asName, iLowLevelGraphics* apLowLevelGraphics,
								bool abUseMipMaps, eTextureTarget aTarget)
	: iTexture(asName, apLowLevelGraphics, abUseMipMaps, aTarget)
	{
		mbContainsData = false;

		if(aTarget != eTextureTarget_2D) mbUseMipMaps = false;

		mpGfxSDL = static_cast<cLowLevelGraphicsSDL*>(mpLowLevelGraphics);

		mlTextureIndex = 0;
		mfTimeCount = 0;
		mfTimeDir = 1;

		mlBpp = 0;

	}

	cSDLTexture::~cSDLTexture()
	{
		for(size_t i=0; i<mvTextureHandles.size(); ++i)
		{
			glDeleteTextures(1,(GLuint *)&mvTextureHandles[i]);
		}
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromBitmap(const Bitmap &bmp)
	{
		//Generate handles
		if (mvTextureHandles.empty())
		{
			mvTextureHandles.resize(1);
			glGenTextures(1, mvTextureHandles.data());
		}
		else
		{
			//Log("Delete + Generate!\n");
			//glDeleteTextures(1,(GLuint *)&mvTextureHandles[0]);
			//glGenTextures(1,(GLuint *)&mvTextureHandles[0]);
		}

		return CreateFromBitmapToHandle(bmp, 0);
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateAnimFromBitmapVec(const std::vector<Bitmap>& bitmaps)
	{
		int count = static_cast<int>(bitmaps.size());
		mvTextureHandles.resize(count);
		glGenTextures(count, mvTextureHandles.data());

		for (int i=0; i<count; ++i)
		{
			if (CreateFromBitmapToHandle(bitmaps[i], i) == false)
			{
				return false;
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateCubeFromBitmapVec(const std::vector<Bitmap>& bitmaps)
	{
		if (mTarget != eTextureTarget_CubeMap)
		{
			return false;
		}

		if (bitmaps.size() < 6) {
			Error("Only %d bitmaps supplied for creation of cube map, 6 needed.", bitmaps.size());
			return false;
		}

		//Generate handles
		if(mvTextureHandles.empty())
		{
			mvTextureHandles.resize(1);
			glGenTextures(1,(GLuint *)&mvTextureHandles[0]);
		}
		else
		{
			glDeleteTextures(1,(GLuint *)&mvTextureHandles[0]);
			glGenTextures(1,(GLuint *)&mvTextureHandles[0]);
		}

		GLenum GLTarget = InitCreation(0);

		//Create the cube map sides
		for(int i=0; i< 6; i++)
		{
			const Bitmap &bmp = bitmaps[i];

			GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i;

			int lChannels;
			GLenum format;
			GetSettings(bmp, lChannels,format);

			glTexImage2D(target, 0, lChannels, bmp.GetWidth(), bmp.GetHeight(),
				0, format, GL_UNSIGNED_BYTE, bmp.GetRawData());

			mlWidth = bmp.GetWidth();
			mlHeight = bmp.GetHeight();
			mlBpp = lChannels * 8;

			if(!cMath::IsPow2(mlHeight) || !cMath::IsPow2(mlWidth))
			{
				Warning("Texture '%s' does not have a pow2 size!\n",msName.c_str());
			}
		}

		PostCreation(GLTarget);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromArray(unsigned char *apPixelData, int alChannels, const cVector3l &avSize)
	{
		if(mvTextureHandles.empty())
		{
			mvTextureHandles.resize(1);
			glGenTextures(1,(GLuint *)&mvTextureHandles[0]);
		}

		GLenum GLTarget = InitCreation(0);

		int lChannels = alChannels;
		GLenum format =0;
		switch(lChannels)
		{
		case 1: format = GL_LUMINANCE; break;
		case 2: format = GL_LUMINANCE_ALPHA; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		}

		mlWidth = avSize.x;
		mlHeight = avSize.y;
		mlDepth = avSize.z;
		mlBpp = lChannels * 8;

		if(!cMath::IsPow2(mlHeight) || !cMath::IsPow2(mlWidth) || !cMath::IsPow2(mlDepth))
		{
			Warning("Texture '%s' does not have a pow2 size!\n",msName.c_str());
		}

		if(mTarget == eTextureTarget_1D)
		{
			glTexImage1D(GLTarget, 0, lChannels, mlWidth,0,format, GL_UNSIGNED_BYTE, apPixelData);
		}
		else if(mTarget == eTextureTarget_2D)
		{
			glTexImage2D(GLTarget, 0, lChannels, mlWidth, mlHeight, 0, format, GL_UNSIGNED_BYTE, apPixelData);
		}
		else if(mTarget == eTextureTarget_3D)
		{
			glTexImage3D(GLTarget, 0, lChannels, avSize.x, avSize.y,avSize.z, 0, format, GL_UNSIGNED_BYTE, apPixelData);
		}

		if(mbUseMipMaps && mTarget == eTextureTarget_2D)
		{
			gluBuild2DMipmaps(GLTarget, lChannels, mlWidth, mlHeight, format, GL_UNSIGNED_BYTE, apPixelData);
		}

		PostCreation(GLTarget);

		return true;
	}


	//-----------------------------------------------------------------------

	void cSDLTexture::SetPixels2D(	int alLevel, const cVector2l& avOffset, const cVector2l& avSize,
									eColorDataFormat aDataFormat, void *apPixelData)
	{
		if(mTarget != eTextureTarget_2D && mTarget != eTextureTarget_Rect) return;

		glTexSubImage2D(TextureTargetToGL(mTarget),alLevel,avOffset.x,avOffset.y, avSize.x,avSize.y,
						ColorFormatToGL(aDataFormat),GL_UNSIGNED_BYTE,apPixelData);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::Update(float afTimeStep)
	{
		if(mvTextureHandles.size() > 1)
		{
			float fMax = (float)(mvTextureHandles.size());
			mfTimeCount += afTimeStep * (1.0f/mfFrameTime) * mfTimeDir;

			if(mfTimeDir > 0)
			{
				if(mfTimeCount >= fMax)
				{
					if(mAnimMode == eTextureAnimMode_Loop)
					{
						mfTimeCount =0;
					}
					else
					{
						mfTimeCount = fMax - 1.0f;
						mfTimeDir = -1.0f;
					}
				}
			}
			else
			{
				if(mfTimeCount < 0)
				{
					mfTimeCount =1;
					mfTimeDir = 1.0f;
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::HasAnimation()
	{
		return mvTextureHandles.size() > 1;
	}

	void cSDLTexture::NextFrame()
	{
		mfTimeCount += mfTimeDir;

		if(mfTimeDir > 0)
		{
			float fMax = (float)(mvTextureHandles.size());
			if(mfTimeCount >= fMax)
			{
				if(mAnimMode == eTextureAnimMode_Loop)
				{
					mfTimeCount =0;
				}
				else
				{
					mfTimeCount = fMax - 1.0f;
					mfTimeDir = -1.0f;
				}
			}
		}
		else
		{
			if(mfTimeCount < 0)
			{
				mfTimeCount =1;
				mfTimeDir = 1.0f;
			}
		}
	}

	void cSDLTexture::PrevFrame()
	{
		mfTimeCount -= mfTimeDir;

		if(mfTimeDir < 0)
		{
			float fMax = (float)(mvTextureHandles.size());
			if(mfTimeCount >= fMax)
			{
				if(mAnimMode == eTextureAnimMode_Loop)
				{
					mfTimeCount =0;
				}
				else
				{
					mfTimeCount = fMax - 1.0f;
					mfTimeDir = -1.0f;
				}
			}
		}
		else
		{
			if(mfTimeCount < 0)
			{
				mfTimeCount =1;
				mfTimeDir = 1.0f;
			}
		}
	}

	float cSDLTexture::GetT()
	{
		return cMath::Modulus(mfTimeCount,1.0f);
	}

	float cSDLTexture::GetTimeCount()
	{
		return mfTimeCount;
	}
	void cSDLTexture::SetTimeCount(float afX)
	{
		mfTimeCount = afX;
	}
	int cSDLTexture::GetCurrentLowlevelHandle()
	{
		return GetTextureHandle();
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetFilter(eTextureFilter aFilter)
	{
		if(mFilter == aFilter) return;

		mFilter = aFilter;
		if(mbContainsData)
		{
			GLenum GLTarget = mpGfxSDL->GetGLTextureTargetEnum(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);

				if(mbUseMipMaps && mTarget != eTextureTarget_Rect){
					if(mFilter == eTextureFilter_Bilinear)
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
					else
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				}
				else{
					glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetAnisotropyDegree(float afX)
	{
		if(!mpLowLevelGraphics->GetCaps(eGraphicCaps_AnisotropicFiltering)) return;
		if(afX < 1.0f) return;
		if(afX > (float) mpLowLevelGraphics->GetCaps(eGraphicCaps_MaxAnisotropicFiltering)) return;

		if(mfAnisotropyDegree == afX) return;

		mfAnisotropyDegree = afX;

		GLenum GLTarget = mpGfxSDL->GetGLTextureTargetEnum(mTarget);

		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameterf(GLTarget,GL_TEXTURE_MAX_ANISOTROPY_EXT ,mfAnisotropyDegree);
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapS(eTextureWrap aMode)
	{
		if(mbContainsData)
		{
			GLenum GLTarget = mpGfxSDL->GetGLTextureTargetEnum(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);

				glTexParameteri(GLTarget,GL_TEXTURE_WRAP_S,GetGLWrap(aMode));
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapT(eTextureWrap aMode)
	{
		if(mbContainsData)
		{
			GLenum GLTarget = mpGfxSDL->GetGLTextureTargetEnum(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);
				glTexParameteri(GLTarget,GL_TEXTURE_WRAP_T,GetGLWrap(aMode));
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapR(eTextureWrap aMode)
	{
		if(mbContainsData)
		{
			GLenum GLTarget = mpGfxSDL->GetGLTextureTargetEnum(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);

				glTexParameteri(GLTarget,GL_TEXTURE_WRAP_R,GetGLWrap(aMode));
			}
		}
	}

	//-----------------------------------------------------------------------

	unsigned int cSDLTexture::GetTextureHandle()
	{
		if(mvTextureHandles.size() > 1)
		{
			int lFrame = (int) mfTimeCount;
			return mvTextureHandles[lFrame];
		}
		else
		{
			return mvTextureHandles[0];
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromBitmapToHandle(const Bitmap &bmp, int alHandleIdx)
	{
		GLenum GLTarget = InitCreation(alHandleIdx);

		mlWidth = bmp.GetWidth();
		mlHeight = bmp.GetHeight();

		if((!cMath::IsPow2(mlHeight) || !cMath::IsPow2(mlWidth)) && mTarget != eTextureTarget_Rect)
		{
			Warning("Texture '%s' does not have a pow2 size!\n",msName.c_str());
		}

		int lChannels = 0;
		GLenum format = 0;
		GetSettings(bmp, lChannels, format);

		mlBpp = lChannels * 8;

		auto pPixelSrc = bmp.GetRawData<unsigned char>();

		//Log("Loading %s  %d x %d\n",msName.c_str(), pSrc->GetWidth(), pSrc->GetHeight());
		//Log("Channels: %d Format: %x\n",lChannels, format);

		//Clear error flags
		while(glGetError()!=GL_NO_ERROR);

		if(mTarget == eTextureTarget_1D)
			glTexImage1D(GLTarget, 0, lChannels, mlWidth,0,format,
			GL_UNSIGNED_BYTE, pPixelSrc);
		else
			glTexImage2D(GLTarget, 0, lChannels, mlWidth, mlHeight,
			0, format, GL_UNSIGNED_BYTE, pPixelSrc);

		if(glGetError()!=GL_NO_ERROR) return false;

		if(mbUseMipMaps && mTarget != eTextureTarget_Rect)
		{
			if(mTarget == eTextureTarget_1D)
				gluBuild1DMipmaps(GLTarget,lChannels,mlWidth,
				format, GL_UNSIGNED_BYTE, pPixelSrc);
			else
				gluBuild2DMipmaps(GLTarget,lChannels,mlWidth, mlHeight,
				format, GL_UNSIGNED_BYTE, pPixelSrc);
		}

		PostCreation(GLTarget);

		return true;
	}

	//-----------------------------------------------------------------------

	GLenum cSDLTexture::InitCreation(int alHandleIdx)
	{
		GLenum GLTarget = mpGfxSDL->GetGLTextureTargetEnum(mTarget);
		glBindTexture(GLTarget, mvTextureHandles[alHandleIdx]);
		return GLTarget;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::PostCreation(GLenum aGLTarget)
	{
		if(mbUseMipMaps && mTarget != eTextureTarget_Rect)
		{
			if(mFilter == eTextureFilter_Bilinear)
				glTexParameteri(aGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(aGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else{
			glTexParameteri(aGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(aGLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(aGLTarget,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(aGLTarget,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(aGLTarget,GL_TEXTURE_WRAP_R,GL_REPEAT);

		mbContainsData = true;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::GetSettings(const Bitmap &bmp, int &alChannels, GLenum &aFormat)
	{
		alChannels = bmp.GetNumChannels();

		if(alChannels==4)
		{
			aFormat = GL_BGRA;
		}
		else if(alChannels==3)
		{
			aFormat = GL_BGR;
		}
		else if(alChannels==1)
		{
			aFormat = GL_ALPHA;
		}
	}

	//-----------------------------------------------------------------------

	GLenum cSDLTexture::GetGLWrap(eTextureWrap aMode)
	{
		switch(aMode)
		{
		case eTextureWrap_Clamp: return GL_CLAMP;
		case eTextureWrap_Repeat: return GL_REPEAT;
		case eTextureWrap_ClampToEdge: return GL_CLAMP_TO_EDGE;
		case eTextureWrap_ClampToBorder: return GL_CLAMP_TO_BORDER;
		default: return GL_REPEAT;
		}
	}


	//-----------------------------------------------------------------------

}
