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
#include "BSEffectShaderProperty.h"
#include "NiAVObject.h"

namespace nif
{
	class NiPSysModifier;

	class NiPSysData : public NiObject
	{
	public:
		using native_type = native::NiPSysData;

	protected:
		friend class File;
		NiPSysData();
		NiPSysData(native_type* obj);

	public:
		virtual ~NiPSysData() = default;

		native_type& getNative() const;

		Property<unsigned short>& maxCount() { return m_maxCount; }
		Property<std::vector<SubtextureOffset>>& subtexOffsets() { return m_subtexOffsets; }
		Property<bool>& hasColour() { return m_hasColour; }
		Property<bool>& hasRotationAngles() { return m_hasRotationAngles; }
		Property<bool>& hasRotationSpeeds() { return m_hasRotationSpeeds; }

	private:
		PropertyFcn<unsigned short, NiPSysData> m_maxCount;

		struct SubtexOffsets : PropertyBase<std::vector<SubtextureOffset>, NiPSysData>
		{
			SubtexOffsets(NiPSysData& block) : 
				PropertyBase<std::vector<SubtextureOffset>, NiPSysData>(block) {}

			virtual std::vector<SubtextureOffset> get() const override;
			virtual void set(const std::vector<SubtextureOffset>& offsets) override;

		} m_subtexOffsets;

		PropertyFcn<bool, NiPSysData> m_hasColour;
		PropertyFcn<bool, NiPSysData> m_hasRotationAngles;
		PropertyFcn<bool, NiPSysData> m_hasRotationSpeeds;
	};

	class NiParticleSystem : public NiAVObject
	{
	public:
		using native_type = native::NiParticleSystem;

	protected:
		friend class File;
		NiParticleSystem();
		NiParticleSystem(native_type* obj);

	public:
		virtual ~NiParticleSystem() = default;

		native_type& getNative() const;

		Assignable	<NiPSysData>&				data()				{ return m_data; }
		Sequence	<NiPSysModifier>&			modifiers()			{ return m_modifiers; }
		Assignable	<BSEffectShaderProperty>&	shaderProperty()	{ return m_shader; }
		Assignable	<NiAlphaProperty>&			alphaProperty()		{ return m_alpha; }
		Property	<bool>&						worldSpace()		{ return m_worldSpace; }

	private:
		struct Data final : AssignableBase<NiPSysData, NiParticleSystem>
		{
			Data(NiParticleSystem& super) : 
				AssignableBase<NiPSysData, NiParticleSystem>{ super } {}

			virtual void assign(NiPSysData* data) override;
			virtual bool isAssigned(NiPSysData* data) const override;

		};

		struct Modifiers final : SequenceBase<NiPSysModifier, NiParticleSystem>
		{
			Modifiers(NiParticleSystem& block) : 
				SequenceBase<NiPSysModifier, NiParticleSystem>(block) {}

			virtual size_t insert(size_t pos, const NiPSysModifier& mod) override;
			virtual size_t erase(size_t pos) override;
			virtual size_t find(const NiPSysModifier& mod) const override;
			virtual size_t size() const override;

		};

		struct ShaderProperty final : AssignableBase<BSEffectShaderProperty, NiParticleSystem>
		{
			ShaderProperty(NiParticleSystem& block) :
				AssignableBase<BSEffectShaderProperty, NiParticleSystem>(block) {}

			virtual void assign(BSEffectShaderProperty* shader) override;
			virtual bool isAssigned(BSEffectShaderProperty* shader) const override;

		};

		struct AlphaProperty final : AssignableBase<NiAlphaProperty, NiParticleSystem>
		{
			AlphaProperty(NiParticleSystem& block) :
				AssignableBase<NiAlphaProperty, NiParticleSystem>(block) {}

			virtual void assign(NiAlphaProperty* alpha) override;
			virtual bool isAssigned(NiAlphaProperty* alpha) const override;

		};

		Data m_data;
		Modifiers m_modifiers;
		ShaderProperty m_shader;
		AlphaProperty m_alpha;
		PropertyFcn<bool, NiParticleSystem> m_worldSpace;
	};
}
