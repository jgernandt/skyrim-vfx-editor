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

#pragma once
#include "Modifier.h"

namespace node
{
	using namespace nif;

	class GravityModifier : public Modifier
	{
	public:
		class StrengthField final :
			public Field,
			public IControllable,
			public AssignableListener<NiInterpolator>
		{
		public:
			StrengthField(const std::string& name, GravityModifier& node, ni_ptr<Property<float>>&& obj);

			virtual Ref<NiInterpolator>& iplr() override;
			virtual ni_ptr<NiTimeController> ctlr() override;
			virtual ni_ptr<NiAVObject> object() override { return ni_ptr<NiAVObject>(); }
			virtual std::string propertyType() override { return std::string(); }
			virtual std::string ctlrType() override { return std::string(); }
			virtual Property<std::string>* ctlrID() override { return nullptr; }
			virtual std::string iplrID() override { return std::string(); }

			virtual void onAssign(NiInterpolator* obj) override;

			void setController(const ni_ptr<NiPSysGravityStrengthCtlr>& ctlr);

		private:
			GravityModifier& m_node;
			ni_ptr<NiPSysGravityStrengthCtlr> m_ctlr;
			FloatCtlrReceiver m_rcvr;
			Sender<IControllable> m_sndr;
		};

	protected:
		GravityModifier(File& file, const ni_ptr<NiPSysGravityModifier>& obj);

	public:
		virtual ~GravityModifier() = default;

		//I think this is how we want this to work, very approximately
		StrengthField& strength() { return *m_strengthField; }

		constexpr static const char* GRAVITY_OBJECT = "Field object";
		constexpr static const char* STRENGTH = "Strength";
		constexpr static const char* DECAY = "Decay";
		constexpr static const char* TURBULENCE = "Turbulence";
		constexpr static const char* TURBULENCE_SCALE = "Turbulence scale";

	private:
		//I'm not sure how we want to store this, just put it here for now
		File& m_file;

		//should not be pointers, but a composition?
		std::unique_ptr<Field> m_objectField;
		std::unique_ptr<StrengthField> m_strengthField;
		std::unique_ptr<Field> m_decayField;
		std::unique_ptr<Field> m_turbField;
		std::unique_ptr<Field> m_turbScaleField;
	};

	class PlanarForceField final : public GravityModifier
	{
	public:
		PlanarForceField(File& file, const ni_ptr<NiPSysGravityModifier>& obj);
		~PlanarForceField();

		constexpr static const char* GRAVITY_AXIS = "Direction";
		constexpr static const char* WORLD_ALIGNED = "World aligned";

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 280.0f;

	private:
		std::unique_ptr<Field> m_axisField;
	};

	class SphericalForceField final : public GravityModifier
	{
	public:
		SphericalForceField(File& file, const ni_ptr<NiPSysGravityModifier>& obj);
		~SphericalForceField();

		constexpr static float WIDTH = 150.0f;
		constexpr static float HEIGHT = 210.0f;

	};
}