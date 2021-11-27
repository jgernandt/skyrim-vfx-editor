//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor, a program for creating visual effects
//in the NetImmerse format.
//
//SVFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//SVFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with SVFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "nodes_internal.h"

namespace nodes
{
	using namespace nif;

	TEST_CLASS(EffectShader)
	{
	public:

		TEST_METHOD(Connector_Target)
		{
			File file{ File::Version::SKYRIM_SE };
			auto obj = file.create<BSEffectShaderProperty>();

			//Testing for BSShaderProperty now, should be BSEffectShaderProperty.
			//We'll have to fix inheritance for Assignable.
			AssignableReceiverTest<Ref>(std::make_unique<node::EffectShader>(obj), 
				*std::static_pointer_cast<BSShaderProperty>(obj), node::EffectShader::GEOMETRY, true);
		}
	};
}
