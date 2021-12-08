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
#include <filesystem>
#include <map>
#include <set>
#include <vector>

#include "nif_objects.h"

namespace nif
{
	class File
	{
	public:
		//using Version = unsigned int;
		//constexpr static Version SKYRIM = 0x14020007;

		enum class Version
		{
			UNKNOWN,
			SKYRIM,
			SKYRIM_SE,
		};

	private:
		using ObjectPair = std::pair<std::shared_ptr<NiObject>, std::shared_ptr<Niflib::NiObject>>;
		using CreateFcn = ObjectPair(*)(const Niflib::Ref<Niflib::NiObject>&);

	public:
		File(Version version = Version::UNKNOWN);
		File(const std::filesystem::path& path);

		File(const File&) = delete;
		File& operator=(const File&) = delete;

		~File();

		//Create a new object of type T.
		template<typename T>
		[[nodiscard]] std::shared_ptr<T> create();

		//Get a pointer to the root node
		std::shared_ptr<NiNode> getRoot() const { return m_rootNode; }

		//The the nif version of the file
		Version getVersion() const { return m_version; }

		//Write the file to the target path
		void write(const std::filesystem::path& path);


		//These getters should not be part of the public interface, they are nif internal business!

		//Return the object that corresponds to Niflib object nativeRef.
		//If nativeRef is null, returns null.
		//If nativeRef is non-null, returns the indexed object. Creates a new object if 
		//nativeRef is not indexed.
		//(Should maybe be called getOrCreate, or similar)
		template<typename T>
		[[nodiscard]] std::shared_ptr<T> get(const Niflib::Ref<typename type_map<T>::type>& nativeRef);

		//Return the Niflib object that corresponds to object.
		template<typename T>
		[[nodiscard]] Niflib::Ref<typename type_map<T>::type> getNative(T* object) const;

	private:
		//Create a new object and add to our index
		template<typename T>
		std::shared_ptr<T> make_ni(const Niflib::Ref<typename type_map<T>::type>& native);

	private:
		//Our factory functions
		template<typename T>
		static ObjectPair make_NiObject(const Niflib::Ref<Niflib::NiObject>& native);

		//Register factory functions on first use
		static void registerTypes();

	private:
		static std::map<size_t, CreateFcn> s_typeRegistry;

		Version m_version{ Version::UNKNOWN };
		std::shared_ptr<NiNode> m_rootNode;

		std::map<Niflib::NiObject*, std::weak_ptr<NiObject>> m_nativeIndex;
		std::map<NiObject*, std::weak_ptr<Niflib::NiObject>> m_objectIndex;
	};

	//Use explicit specialisation here to avoid the public having to know anything about the native type.
	//Annoying, but what are the alternatives? We could map another set of factory functions, which is
	//just as much work for a worse solution.
	template<typename T> [[nodiscard]] std::shared_ptr<T> File::create() { return std::shared_ptr<T>(); }
	template<> [[nodiscard]] std::shared_ptr<NiObject> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiObjectNET> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiAVObject> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiNode> File::create();
	template<> [[nodiscard]] std::shared_ptr<BSFadeNode> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiProperty> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiAlphaProperty> File::create();
	template<> [[nodiscard]] std::shared_ptr<BSShaderProperty> File::create();
	template<> [[nodiscard]] std::shared_ptr<BSEffectShaderProperty> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiBoolData> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiFloatData> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiInterpolator> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiBoolInterpolator> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiFloatInterpolator> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiBlendInterpolator> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiBlendBoolInterpolator> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiBlendFloatInterpolator> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiTimeController> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiSingleInterpController> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiParticleSystem> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysData> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiPSysModifier> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysAgeDeathModifier> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysBoundUpdateModifier> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysGravityModifier> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysPositionModifier> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysRotationModifier> File::create();
	template<> [[nodiscard]] std::shared_ptr<BSPSysScaleModifier> File::create();
	template<> [[nodiscard]] std::shared_ptr<BSPSysSimpleColorModifier> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiPSysEmitter> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysVolumeEmitter> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysBoxEmitter> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysCylinderEmitter> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysSphereEmitter> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiPSysModifierCtlr> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysUpdateCtlr> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiPSysEmitterCtlr> File::create();

	template<> [[nodiscard]] std::shared_ptr<NiExtraData> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiStringExtraData> File::create();
	template<> [[nodiscard]] std::shared_ptr<NiStringsExtraData> File::create();
}
