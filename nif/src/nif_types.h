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
#include <memory>
#include <string>
#include "Rotation.h"
#include "nif_data.h"

namespace nif
{
	template<size_t N>
	using Floats = std::array<float, N>;
	template<size_t N>
	using Ints = std::array<int, N>;

	using ColRGBA = Floats<4>;
	constexpr ColRGBA COL_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr ColRGBA COL_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr ColRGBA COL_RED = { 1.0f, 0.0f, 0.0f, 1.0f };
	constexpr ColRGBA COL_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
	constexpr ColRGBA COL_BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };

	using SubtextureCount = Ints<2>;
	using SubtextureOffset = Floats<4>;

	using translation_t = Floats<3>;
	using rotation_t = math::Rotation;

	using BillboardMode = uint_fast16_t;
	constexpr BillboardMode ALWAYS_FACE_CAMERA = 0;
	constexpr BillboardMode ROTATE_ABOUT_UP = 1;
	constexpr BillboardMode RIGID_FACE_CAMERA = 2;
	constexpr BillboardMode ALWAYS_FACE_CENTER = 3;
	constexpr BillboardMode RIGID_FACE_CENTER = 4;
	constexpr BillboardMode BS_ROTATE_ABOUT_UP = 5;
	constexpr BillboardMode ROTATE_ABOUT_UP2 = 9;

	using ControllerFlags = std::uint_fast16_t;
	constexpr ControllerFlags CTLR_APP_INIT = 0x01;
	//Not sure if loop types work like flags or like an enum (0 = loop, 1 = reverse, 2 = clamp).
	//Need to test what happens if you set both reverse and clamp.
	constexpr ControllerFlags CTLR_LOOP_REVERSE = 0x02;
	constexpr ControllerFlags CTLR_LOOP_CLAMP = 0x04;
	constexpr ControllerFlags CTLR_LOOP_MASK = 0x06;
	//Actually, NiControllerSequence treats it as an enum, so we might want to switch to that
	enum class CycleType : std::uint_fast32_t
	{
		REPEAT = 0,
		REVERSE = 1,
		CLAMP = 2,
	};

	constexpr ControllerFlags CTLR_ACTIVE = 0x08;
	constexpr ControllerFlags CTLR_PLAY_BWD = 0x10;
	constexpr ControllerFlags CTLR_MNGR_CTRLD = 0x20;
	constexpr ControllerFlags CTLR_BS_UNKNOWN = 0x40;

	using ForceType = uint_fast32_t;
	constexpr ForceType FORCE_PLANAR = 0;
	constexpr ForceType FORCE_SPHERICAL = 1;
	constexpr ForceType FORCE_UNKNOWN = 2;

	using KeyType = uint_fast32_t;
	constexpr KeyType KEY_LINEAR		= 1;
	constexpr KeyType KEY_QUADRATIC		= 2;
	constexpr KeyType KEY_TBC			= 3;
	constexpr KeyType KEY_XYZ_ROTATION	= 4;
	constexpr KeyType KEY_CONSTANT		= 5;

	using ShaderFlags = std::uint_fast32_t;
	constexpr ShaderFlags SF1_PALETTE_COLOUR = 0x00000010;
	constexpr ShaderFlags SF1_PALETTE_ALPHA = 0x00000020;
	constexpr ShaderFlags SF1_ZBUFFER_TEST = 0x80000000;

	constexpr ShaderFlags SF2_DOUBLE_SIDED = 0x00000010;
	constexpr ShaderFlags SF2_VERTEX_COLOUR = 0x00000020;

	template<typename T>
	struct Key
	{
		Key() = default;
		Key(const Key<T>&) = delete;
		Key(Key<T>&& other) noexcept { *this = std::move(other); }

		Key<T>& operator=(const Key<T>&) = delete;
		Key<T>& operator=(Key<T>&& other) noexcept
		{
			time = std::move(other.time);
			value = std::move(other.value);
			fwdTan = std::move(other.fwdTan);
			bwdTan = std::move(other.bwdTan);
			tension = std::move(other.tension);
			bias = std::move(other.bias);
			continuity = std::move(other.continuity);
			return *this;
		}

		Property<float> time;
		Property<T> value;
		Property<T> fwdTan;
		Property<T> bwdTan;
		Property<float> tension;
		Property<float> bias;
		Property<float> continuity;
	};

	template<>
	struct Key<std::string>
	{
		Key() = default;
		Key(const Key<std::string>&) = delete;
		Key(Key<std::string>&& other) noexcept { *this = std::move(other); }

		Key<std::string>& operator=(const Key<std::string>&) = delete;
		Key<std::string>& operator=(Key<std::string>&& other) noexcept
		{
			time = std::move(other.time);
			value = std::move(other.value);
			return *this;
		}

		Property<float> time;
		Property<std::string> value;
	};

	template<typename T>
	using ni_ptr = std::shared_ptr<T>;

	struct NiObject;
	struct NiObjectNET;
	struct NiAVObject;
	struct NiNode;
	struct NiBillboardNode;
	struct BSFadeNode;

	struct NiProperty;
	struct NiAlphaProperty;
	struct BSShaderProperty;
	struct BSEffectShaderProperty;

	struct NiBoolData;
	struct NiFloatData;

	struct NiInterpolator;
	struct NiBoolInterpolator;
	struct NiFloatInterpolator;
	struct NiBlendInterpolator;
	struct NiBlendBoolInterpolator;
	struct NiBlendFloatInterpolator;

	struct NiTimeController;
	struct NiSingleInterpController;

	struct NiControllerManager;
	struct NiControllerSequence;
	struct NiDefaultAVObjectPalette;
	struct BSAnimNotes;

	struct NiParticleSystem;
	struct NiPSysData;

	struct NiPSysModifier;
	struct NiPSysAgeDeathModifier;
	struct NiPSysBoundUpdateModifier;
	struct NiPSysGravityModifier;
	struct NiPSysPositionModifier;
	struct NiPSysRotationModifier;
	struct BSPSysScaleModifier;
	struct BSPSysSimpleColorModifier;

	struct NiPSysEmitter;
	struct NiPSysVolumeEmitter;
	struct NiPSysBoxEmitter;
	struct NiPSysCylinderEmitter;
	struct NiPSysSphereEmitter;

	struct NiPSysModifierCtlr;
	struct NiPSysUpdateCtlr;
	struct NiPSysEmitterCtlr;
	struct NiPSysGravityStrengthCtlr;

	struct NiExtraData;
	struct NiStringExtraData;
	struct NiStringsExtraData;
	struct NiTextKeyExtraData;
	struct BSBehaviorGraphExtraData;
}

namespace Niflib
{
	template<typename T> class Ref;

	class NiObject;
	class NiObjectNET;
	class NiAVObject;
	class NiNode;
	class NiBillboardNode;
	class BSFadeNode;

	class NiProperty;
	class NiAlphaProperty;
	class BSShaderProperty;
	class BSEffectShaderProperty;

	class NiBoolData;
	class NiFloatData;

	class NiInterpolator;
	class NiBoolInterpolator;
	class NiFloatInterpolator;
	class NiBlendInterpolator;
	class NiBlendBoolInterpolator;
	class NiBlendFloatInterpolator;

	class NiTimeController;
	class NiSingleInterpController;

	class NiControllerManager;
	class NiControllerSequence;
	class NiDefaultAVObjectPalette;
	class BSAnimNotes;

	class NiParticleSystem;
	class NiPSysData;

	class NiPSysModifier;
	class NiPSysAgeDeathModifier;
	class NiPSysBoundUpdateModifier;
	class NiPSysGravityModifier;
	class NiPSysPositionModifier;
	class NiPSysRotationModifier;
	class BSPSysScaleModifier;
	class BSPSysSimpleColorModifier;

	class NiPSysEmitter;
	class NiPSysVolumeEmitter;
	class NiPSysBoxEmitter;
	class NiPSysCylinderEmitter;
	class NiPSysSphereEmitter;

	class NiPSysModifierCtlr;
	class NiPSysUpdateCtlr;
	class NiPSysEmitterCtlr;
	class NiPSysGravityStrengthCtlr;

	class NiExtraData;
	class NiStringExtraData;
	class NiStringsExtraData;
	class NiTextKeyExtraData;
	class BSBehaviorGraphExtraData;
}
