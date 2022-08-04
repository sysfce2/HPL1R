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
#ifndef HPL_GPU_PROGRAM_MANAGER_H
#define HPL_GPU_PROGRAM_MANAGER_H

#include "resources/ResourceManager.h"

#include "graphics/GPUProgram.h"

namespace hpl {

	class iLowLevelGraphics;

	class cGpuProgramManager : public iResourceManager
	{
	public:
		cGpuProgramManager(cFileSearcher *apFileSearcher, iLowLevelGraphics *apLowLevelGraphics);
		~cGpuProgramManager();

		iResourceBase* Create(const tString& asName);
		/**
		 * Creates a new GPU program
		 * \param asVertexName file name of the vertex shader
		 * \param asFragmentName file name of the fragment shader
		 * \return
		 */
		iGpuProgram* CreateProgram(const tString& asVertexName,const tString& asFragmentName);

		void Destroy(iResourceBase* apResource);

	private:
		iLowLevelGraphics *mpLowLevelGraphics;
	};

};
#endif // HPL_GPU_PROGRAM_MANAGER_H
