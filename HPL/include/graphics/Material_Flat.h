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
#ifndef HPL_MATERIAL_FLAT_H
#define HPL_MATERIAL_FLAT_H

#include "graphics/Material.h"

namespace hpl {

	class cMaterial_Flat : public iMaterial
	{
	public:
		cMaterial_Flat(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
			cRenderer3D *apRenderer3D);

		virtual ~cMaterial_Flat();

		tTextureTypeList GetTextureTypes();

		bool UsesType(eMaterialRenderType aType);

		iGpuProgram* GetVertexProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		iGpuProgram* GetFragmentProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);

		int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight);
	};

	class cMaterialType_Flat : public iMaterialType
	{
	public:
		bool IsCorrect(tString asName){
			return cString::ToLowerCase(asName)=="flat";
		}

		iMaterial* Create(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
			cRenderer3D *apRenderer3D)
		{
			return hplNew( cMaterial_Flat,(asName,apLowLevelGraphics,
				apTextureManager,apProgramManager,apRenderer3D) );
		}
	};

};
#endif // HPL_MATERIAL_FLAT_H
