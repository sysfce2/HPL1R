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
#ifndef HPL_MESH_MANAGER_H
#define HPL_MESH_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl {

	class cMeshLoaderHandler;
	class cMesh;

	class cMeshManager : public iResourceManager
	{
	public:
		cMeshManager(cMeshLoaderHandler *meshLoadHandler);

		iResourceBase* LoadAsset(const tString &name, const tString &fullPath) override;
		std::span<const tString> SupportedExtensions() const override;

		cMesh* CreateMesh(const tString &name);

	private:
		cMeshLoaderHandler *_meshLoadHandler;
	};

};
#endif // HPL_MESH_MANAGER_H
