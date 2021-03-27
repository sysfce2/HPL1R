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
#ifndef HPL_SCRIPT_H
#define HPL_SCRIPT_H

#include "resources/ResourceBase.h"

#ifdef __GNUC__
	#if defined (__APPLE__) || defined(__LP64__)
		#define __stdcall
	#else
		#define __stdcall __attribute__((stdcall))
	#endif
#endif

// Macros to generate signatures and generic function wrappers for AS entrypoints

#define SCRIPT_DEFINE_FUNC(ret, name) \
	static const char* name##_proto = #ret" "#name"()";
#define SCRIPT_DEFINE_FUNC_1(ret, name, a0) \
	static const char* name##_proto = #ret" "#name"("#a0" a0)";
#define SCRIPT_DEFINE_FUNC_2(ret, name, a0,a1) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1)";
#define SCRIPT_DEFINE_FUNC_3(ret, name, a0,a1,a2) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2)";
#define SCRIPT_DEFINE_FUNC_4(ret, name, a0,a1,a2,a3) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3)";
#define SCRIPT_DEFINE_FUNC_5(ret, name, a0,a1,a2,a3,a4) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3,"#a4" a4)";
#define SCRIPT_DEFINE_FUNC_6(ret, name, a0,a1,a2,a3,a4,a5) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3,"#a4" a4,"#a5" a5)";
#define SCRIPT_DEFINE_FUNC_7(ret, name, a0,a1,a2,a3,a4,a5,a6) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3,"#a4" a4,"#a5" a5,"#a6" a6)";
#define SCRIPT_DEFINE_FUNC_8(ret, name, a0,a1,a2,a3,a4,a5,a6,a7) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3,"#a4" a4,"#a5" a5,"#a6" a6,"#a7" a7)";
#define SCRIPT_DEFINE_FUNC_9(ret, name, a0,a1,a2,a3,a4,a5,a6,a7,a8) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3,"#a4" a4,"#a5" a5,"#a6" a6,"#a7" a7,"#a8" a8)";
#define SCRIPT_DEFINE_FUNC_12(ret, name, a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3,"#a4" a4,"#a5" a5,"#a6" a6,"#a7" a7,"#a8" a8,"#a9" a9,"#a10" a10,"#a11" a11)";
#define SCRIPT_DEFINE_FUNC_17(ret, name, a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16) \
	static const char* name##_proto = #ret" "#name"("#a0" a0,"#a1" a1,"#a2" a2,"#a3" a3,"#a4" a4,"#a5" a5,"#a6" a6,"#a7" a7,"#a8" a8,"#a9" a9,"#a10" a10,"#a11" a11,"#a12" a12,"#a13" a13,"#a14" a14,"#a15" a15,"#a16" a16)";

#define SCRIPT_REGISTER_FUNC(func) func##_proto, reinterpret_cast<void*>(func)


namespace hpl {

	class iScript : public iResourceBase
	{
	public:
		iScript(const tString& asName) : iResourceBase(asName,0){}
		virtual ~iScript() = default;

		void Destroy(){}

		virtual bool CreateFromFile(const tString& asFile, tString *apCompileMessages=NULL)=0;

		virtual int GetFuncHandle(const tString& asFunc)=0;

		/**
		 * Runs a func in the script, for example "test(15)"
		 * \param asFuncLine the line of code
		 * \return true if everything was ok, else false
		 */
		virtual bool Run(const tString& asFuncLine)=0;

		virtual bool Run(int alHandle)=0;
	};
};
#endif // HPL_SCRIPT_H
