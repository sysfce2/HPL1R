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
#include "resources/Resources.h"

#include "resources/LowLevelResources.h"
#include "resources/FileSearcher.h"
#include "resources/ImageManager.h"
#include "resources/GpuProgramManager.h"
#include "resources/ParticleManager.h"
#include "resources/SoundManager.h"
#include "resources/FontManager.h"
#include "resources/ScriptManager.h"
#include "resources/TextureManager.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/SoundEntityManager.h"
#include "resources/AnimationManager.h"
#include "resources/VideoManager.h"
#include "resources/ConfigFile.h"
#include "resources/LanguageFile.h"
#include "system/System.h"

#include "system/LowLevelSystem.h"

#include "impl/tinyXML/tinyxml.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cResources::cResources(iLowLevelResources *apLowLevelResources,iLowLevelGraphics *apLowLevelGraphics)
		: iUpdateable("Resources")
	{
		mpLowLevelResources = apLowLevelResources;
		mpLowLevelGraphics = apLowLevelGraphics;

		mpFileSearcher = hplNew( cFileSearcher, (mpLowLevelResources) );

		mpDefaultEntity3DLoader = NULL;
		mpDefaultArea3DLoader = NULL;

		mpLanguageFile = NULL;
	}

	//-----------------------------------------------------------------------

	cResources::~cResources()
	{
		Log("Exiting Resources Module\n");
		Log("--------------------------------------------------------\n");

		STLMapDeleteAll(m_mEntity3DLoaders);
		STLMapDeleteAll(m_mArea3DLoaders);

		hplDelete(mpFontManager);
		hplDelete(mpScriptManager);
		hplDelete(mpParticleManager);
		hplDelete(mpSoundManager);
		hplDelete(mpMeshManager);
		hplDelete(mpMaterialManager);
		hplDelete(mpGpuProgramManager);
		hplDelete(mpImageManager);
		hplDelete(mpTextureManager);
		hplDelete(mpSoundEntityManager);
		hplDelete(mpAnimationManager);
		hplDelete(mpVideoManager);

		Log(" All resources deleted\n");

		hplDelete(mpFileSearcher);
		hplDelete(mpMeshLoaderHandler);

		if(mpLanguageFile) hplDelete(mpLanguageFile);

		mlstManagers.clear();
		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cResources::Init(	cGraphics* apGraphics,cSystem *apSystem, cSound* apSound, cScene *apScene,
							cGui *apGui)
	{
		Log("Initializing Resources Module\n");
		Log("--------------------------------------------------------\n");

		mpLowLevelSystem = apSystem->GetLowLevel();

		Log(" Setting default directories\n");
		AddBaseDirectories();

		Log(" Creating resource managers\n");

		mpImageManager = hplNew( cImageManager,(mpFileSearcher,mpLowLevelGraphics,mpLowLevelResources,mpLowLevelSystem) );
		mlstManagers.push_back(mpImageManager);
		mpGpuProgramManager = hplNew( cGpuProgramManager,(mpFileSearcher,mpLowLevelGraphics,mpLowLevelResources,mpLowLevelSystem) );
		mlstManagers.push_back(mpGpuProgramManager);
		mpParticleManager = hplNew( cParticleManager,(apGraphics, this) );
		mlstManagers.push_back(mpParticleManager);
		mpSoundManager = hplNew( cSoundManager,(apSound, this) );
		mlstManagers.push_back(mpParticleManager);
		mpFontManager = hplNew( cFontManager,(apGraphics,apGui, this) );
		mlstManagers.push_back(mpFontManager);
		mpScriptManager = hplNew( cScriptManager,(apSystem, this) );
		mlstManagers.push_back(mpScriptManager);
		mpTextureManager = hplNew( cTextureManager,(apGraphics, this) );
		mlstManagers.push_back(mpTextureManager);
		mpMaterialManager = hplNew( cMaterialManager,(apGraphics, this) );
		mlstManagers.push_back(mpMaterialManager);
		mpMeshManager = hplNew( cMeshManager,(apGraphics, this) );
		mlstManagers.push_back(mpMeshManager);
		mpSoundEntityManager = hplNew( cSoundEntityManager,(apSound, this) );
		mlstManagers.push_back(mpSoundEntityManager);
		mpAnimationManager = hplNew( cAnimationManager,(apGraphics, this) );
		mlstManagers.push_back(mpAnimationManager);
		mpVideoManager = hplNew( cVideoManager,(apGraphics, this) );
		mlstManagers.push_back(mpVideoManager);

		Log(" Misc Creation\n");

		mpMeshLoaderHandler = hplNew( cMeshLoaderHandler,(this, apScene) );

		mpLowLevelResources->AddMeshLoaders(mpMeshLoaderHandler);
		mpLowLevelResources->AddVideoLoaders(mpVideoManager);

		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	void cResources::Update(float afTimeStep)
	{
		tResourceManagerListIt it = mlstManagers.begin();
		for(; it != mlstManagers.end(); ++it)
		{
			iResourceManager *pManager = *it;

			pManager->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	cFileSearcher* cResources::GetFileSearcher()
	{
		return mpFileSearcher;
	}

	//-----------------------------------------------------------------------

	bool cResources::LoadResourceDirsFile(const tString &asFile)
	{
		TiXmlDocument* pXmlDoc = hplNew( TiXmlDocument, (asFile.c_str()) );
		if(pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load XML file '%s'!\n",asFile.c_str());
			hplDelete( pXmlDoc);
			return false;
		}

		//Get the root.
		TiXmlElement* pRootElem = pXmlDoc->RootElement();

		TiXmlElement* pChildElem = pRootElem->FirstChildElement();
		for(; pChildElem != NULL; pChildElem = pChildElem->NextSiblingElement())
		{
			tString sPath = cString::ToString(pChildElem->Attribute("Path"),"");
			if(sPath==""){
				continue;
			}

			if(sPath[0]=='/' || sPath[0]=='\\') sPath = sPath.substr(1);

			AddResourceDir(sPath);
		}

		hplDelete(pXmlDoc);
		return true;
	}

	/**
	 * \todo File searcher should check so if the dir is allready added and if so return false and not add
	 * \param &asDir
	 * \param &asMask
	 * \return
	 */
	bool cResources::AddResourceDir(const tString &asDir, const tString &asMask)
	{
		mpFileSearcher->AddDirectory(asDir, asMask);
		if(iResourceBase::GetLogCreateAndDelete())
			Log(" Added resource directory '%s'\n",asDir.c_str());
		return true;
	}

	void cResources::AddBaseDirectories() {
		// graphics internal files
		AddResourceDir("core/programs");
		AddResourceDir("core/textures");

		// rehatched overrides and extensions
		AddResourceDir("rehatched/config");
	}

	/**
	 * Reset resource directory search paths to built-ins + game specific
	 * paths specified in a resources.cfg file.
	 * Ideally resource paths would be grouped and individually reset
	 * but that is not the HPL1 way.
	 */
	void cResources::SetupResourceDirsWithFile(const tString &asFile)
	{
		mpFileSearcher->ClearDirectories();
		AddBaseDirectories();
		LoadResourceDirsFile(asFile);
	}

	//-----------------------------------------------------------------------

	bool cResources::SetLanguageFile(const tString &asFile)
	{
		if(mpLanguageFile){
			hplDelete(mpLanguageFile);
			mpLanguageFile = NULL;
		}

		tString asPath = mpFileSearcher->GetFilePath(asFile);
		if(asPath=="")
		{
			Error("Couldn't load language file '%s'\n",asFile.c_str());
			return false;
		}

		mpLanguageFile = hplNew( cLanguageFile, (this) );

		return mpLanguageFile->LoadFromFile(asPath);
	}

	const tWString& cResources::Translate(const tString& asCat, const tString& asName)
	{
		if(mpLanguageFile)
		{
			return mpLanguageFile->Translate(asCat,asName);
		}
		else
		{
			return mwsEmptyString;
		}
	}

	//-----------------------------------------------------------------------

	void cResources::AddEntity3DLoader(iEntity3DLoader* apLoader, bool abSetAsDefault)
	{
		m_mEntity3DLoaders.insert(tEntity3DLoaderMap::value_type(apLoader->GetName(), apLoader));

		if(abSetAsDefault){
			mpDefaultEntity3DLoader = apLoader;
		}
	}

	iEntity3DLoader* cResources::GetEntity3DLoader(const tString& asName)
	{
		tEntity3DLoaderMapIt it = m_mEntity3DLoaders.find(asName);
		if(it == m_mEntity3DLoaders.end()){
			Warning("No loader for type '%s' found!\n",asName.c_str());

			if(mpDefaultEntity3DLoader){
				Log("Using default loader!\n");
				return mpDefaultEntity3DLoader;
			}
			else {
				return NULL;
			}
		}

		return it->second;
	}

	//-----------------------------------------------------------------------


	void cResources::AddArea3DLoader(iArea3DLoader* apLoader, bool abSetAsDefault)
	{
		m_mArea3DLoaders.insert(tArea3DLoaderMap::value_type(apLoader->GetName(), apLoader));

		if(abSetAsDefault){
			mpDefaultArea3DLoader = apLoader;
		}
	}

	iArea3DLoader* cResources::GetArea3DLoader(const tString& asName)
	{
		tArea3DLoaderMapIt it = m_mArea3DLoaders.find(asName);
		if(it == m_mArea3DLoaders.end()){
			Warning("No loader for area type '%s' found!\n",asName.c_str());

			if(mpDefaultArea3DLoader){
				Log("Using default loader!\n");
				return mpDefaultArea3DLoader;
			}
			else {
				return NULL;
			}
		}

		return it->second;
	}

	//-----------------------------------------------------------------------

	iLowLevelResources* cResources::GetLowLevel()
	{
		return mpLowLevelResources;
	}

	//-----------------------------------------------------------------------

}
