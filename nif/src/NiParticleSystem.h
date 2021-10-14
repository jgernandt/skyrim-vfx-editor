//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
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
		NiPSysData();
		NiPSysData(native::NiPSysData* obj);
		NiPSysData(const NiPSysData&) = delete;

		virtual ~NiPSysData() = default;

		NiPSysData& operator=(const NiPSysData&) = delete;

		native::NiPSysData& getNative() const;

		IProperty<unsigned short>& maxCount() { return m_maxCount; }
		IProperty<std::vector<SubtextureOffset>>& subtexOffsets() { return m_subtexOffsets; }
		IProperty<bool>& hasColour() { return m_hasColour; }
		IProperty<bool>& hasRotationAngles() { return m_hasRotationAngles; }
		IProperty<bool>& hasRotationSpeeds() { return m_hasRotationSpeeds; }

	private:
		Property<unsigned short> m_maxCount;

		struct SubtexOffsets : PropertyBase<std::vector<SubtextureOffset>>
		{
			SubtexOffsets(NiPSysData& super) : m_super{ super } {}

			virtual std::vector<SubtextureOffset> get() const override;
			virtual void set(const std::vector<SubtextureOffset>& offsets) override;

			NiPSysData& m_super;

		} m_subtexOffsets;

		Property<bool> m_hasColour;
		Property<bool> m_hasRotationAngles;
		Property<bool> m_hasRotationSpeeds;
	};

	class NiParticleSystem : public NiAVObject
	{
	public:
		NiParticleSystem();
		NiParticleSystem(native::NiParticleSystem* obj);
		NiParticleSystem(const NiParticleSystem&) = delete;

		virtual ~NiParticleSystem() = default;

		NiParticleSystem& operator=(const NiParticleSystem&) = delete;

		native::NiParticleSystem& getNative() const;

		IAssignable<NiPSysData>& data() { return m_data; }

		ISequence<NiPSysModifier>& modifiers() { return m_modifiers; }

		IAssignable<BSEffectShaderProperty>& shaderProperty() { return m_shader; }
		IAssignable<NiAlphaProperty>& alphaProperty() { return m_alpha; }

		IProperty<bool>& worldSpace() { return m_worldSpace; }

	private:
		struct Data final : AssignableBase<NiPSysData>
		{
			Data(NiParticleSystem& super) : m_super{ super } {}

			virtual void assign(NiPSysData* data) override;
			virtual bool isAssigned(NiPSysData* data) const override;

			NiParticleSystem& m_super;

		} m_data;

		struct Modifiers final : SequenceBase<NiPSysModifier>
		{
			Modifiers(NiParticleSystem& super) : m_super{ super } {}

			virtual size_t insert(size_t pos, const NiPSysModifier& mod) override;
			virtual size_t erase(size_t pos) override;
			virtual size_t find(const NiPSysModifier& mod) const override;
			virtual size_t size() const override;

			NiParticleSystem& m_super;

		} m_modifiers;

		struct ShaderProperty final : AssignableBase<BSEffectShaderProperty>
		{
			ShaderProperty(NiParticleSystem& super) : m_super{ super } {}

			virtual void assign(BSEffectShaderProperty* shader) override;
			virtual bool isAssigned(BSEffectShaderProperty* shader) const override;

			NiParticleSystem& m_super;

		} m_shader;

		struct AlphaProperty final : AssignableBase<NiAlphaProperty>
		{
			AlphaProperty(NiParticleSystem& super) : m_super{ super } {}

			virtual void assign(NiAlphaProperty* alpha) override;
			virtual bool isAssigned(NiAlphaProperty* alpha) const override;

			NiParticleSystem& m_super;

		} m_alpha;

		Property<bool> m_worldSpace;
	};

}