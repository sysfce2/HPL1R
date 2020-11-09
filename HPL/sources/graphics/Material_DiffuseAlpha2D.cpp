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
#include "graphics/Material_DiffuseAlpha2D.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_DiffuseAlpha2D::cMaterial_DiffuseAlpha2D(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cImageManager* apImageManager, cTextureManager *apTextureManager,
		cGpuProgramManager* apProgramManager,
		eMaterialPicture aPicture, cRenderer3D *apRenderer3D)
	: iOldMaterial(asName,apLowLevelGraphics,apImageManager,apTextureManager,apProgramManager,
				   aPicture,apRenderer3D)
	{
		mbIsTransperant = true;
		mbIsGlowing= true;//Set to false later on
		mType = eMaterialType_DiffuseAlpha;
	}

	//-----------------------------------------------------------------------

	cMaterial_DiffuseAlpha2D::~cMaterial_DiffuseAlpha2D()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cMaterial_DiffuseAlpha2D::StartRendering(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Diffuse)
		{
			mpLowLevelGraphics->SetBlendActive(true);
			mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);

			mpLowLevelGraphics->SetTexture(0, GetTexture(eMaterialTexture_Diffuse));
			//mpLowLevelGraphics->SetTextureParam(eTextureParam_ColorOp1,eTextureOp_Alpha);
			//mpLowLevelGraphics->SetTextureParam(eTextureParam_ColorFunc, eTextureFunc_Modulate);

			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	void cMaterial_DiffuseAlpha2D::EndRendering(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Diffuse)
		{
			//mpLowLevelGraphics->SetTextureParam(eTextureParam_ColorOp1,eTextureOp_Color);
			//mpLowLevelGraphics->SetTextureParam(eTextureParam_ColorFunc, eTextureFunc_Modulate);
		}

	}

	//-----------------------------------------------------------------------

	tVtxBatchFlag cMaterial_DiffuseAlpha2D::GetBatchFlags(eMaterialRenderType aType)
	{
		return eVtxBatchFlag_Position |	eVtxBatchFlag_Texture0 | eVtxBatchFlag_Color0;
	}

	eMaterialType cMaterial_DiffuseAlpha2D::GetType(eMaterialRenderType aType)
	{
		return mType;
	}

	//-----------------------------------------------------------------------
}
