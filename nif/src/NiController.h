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
#include "NiObjectNET.h"

namespace nif
{
	template<typename T>
	struct Key
	{
		float key;
		T value;
	};

	template<typename T>
	struct Tangent
	{
		T forward;
		T backward;
	};

	struct TBC
	{
		float tension;
		float bias;
		float continuity;
	};

	template<typename T> constexpr bool operator==(const Key<T>& lhs, const Key<T>& rhs)
	{
		return lhs.key == rhs.key && lhs.value == rhs.value;
	}
	template<typename T> constexpr bool operator!=(const Key<T>& lhs, const Key<T>& rhs) { return !(lhs == rhs); }
	template<typename T> constexpr bool operator==(const Tangent<T>& lhs, const Tangent<T>& rhs)
	{
		return lhs.forward == rhs.forward && lhs.backward == rhs.backward;
	}
	template<typename T> constexpr bool operator!=(const Tangent<T>& lhs, const Tangent<T>& rhs) { return !(lhs == rhs); }
	constexpr bool operator==(const TBC& lhs, const TBC& rhs)
	{
		return lhs.tension == rhs.tension && lhs.bias == rhs.bias && lhs.continuity == rhs.continuity;
	}
	constexpr bool operator!=(const TBC& lhs, const TBC& rhs) { return !(lhs == rhs); }

	class NiInterpolator : public NiObject
	{
	public:
		using native_type = native::NiInterpolator;

	protected:
		friend class File;
		NiInterpolator(native_type* obj);

	public:
		virtual ~NiInterpolator() = default;

		native_type& getNative() const;
	};

	class NiBoolData : public NiObject
	{
	public:
		using native_type = native::NiBoolData;

	protected:
		friend class File;
		NiBoolData();
		NiBoolData(native_type* obj);

	public:
		virtual ~NiBoolData() = default;

		native_type& getNative() const;

		Property<KeyType>& keyType() { return m_keyType; }

	private:
		PropertyFcn<KeyType, NiBoolData, native::KeyType> m_keyType;
	};

	class NiBoolInterpolator : public NiInterpolator
	{
	public:
		using native_type = native::NiBoolInterpolator;

	protected:
		friend class File;
		NiBoolInterpolator();
		NiBoolInterpolator(native_type* obj);

	public:
		virtual ~NiBoolInterpolator() = default;

		native_type& getNative() const;

		Property<bool>& value() { return m_value; }
		const Property<bool>& value() const { return m_value; }

		Assignable<NiBoolData>& data() { return m_data; }
		const Assignable<NiBoolData>& data() const { return m_data; }

	private:
		PropertyFcn<bool, NiBoolInterpolator> m_value;
		AssignableFcn<NiBoolData, NiBoolInterpolator> m_data;
	};

	class NiFloatData : public NiObject
	{
	public:
		using native_type = native::NiFloatData;

	protected:
		friend class File;
		NiFloatData();
		NiFloatData(native_type* obj);

	public:
		virtual ~NiFloatData() = default;

		native::NiFloatData& getNative() const;

		Property<KeyType>& keyType() { return m_keyType; }
		InterpolationData<float>& iplnData() { return m_keys; }

	private:
		struct IplnData final : InterpolationData<float>
		{
			IplnData(NiFloatData& super);

			virtual VectorProperty<Key<float>>& keys() override;
			virtual VectorProperty<Tangent<float>>& tangents() override;
			virtual VectorProperty<TBC>& tbc() override;

			class Keys;
			class Tangents;
			class TBCs;

			class Keys final : public VectorPropertyBase<Key<float>, NiFloatData>
			{
			public:
				Keys(NiFloatData& block) : 
					VectorPropertyBase<Key<float>, NiFloatData>(block) {}

				virtual container get() const override;
				virtual void set(const container& c) override;

				virtual Key<float> get(int i) const override;
				virtual void set(int i, const Key<float>& key) override;

				virtual int insert(int i, const Key<float>& key) override;
				virtual int erase(int i) override;

				friend class Tangents;
				friend class TBCs;
			};
			class Tangents final : public VectorPropertyBase<Tangent<float>, NiFloatData>
			{
			public:
				Tangents(NiFloatData& block) : 
					VectorPropertyBase<Tangent<float>, NiFloatData>(block) {}

				virtual container get() const override;
				virtual void set(const container& c) override;

				virtual Tangent<float> get(int i) const override;
				virtual void set(int i, const Tangent<float>& tan) override;

				virtual int insert(int i, const Tangent<float>& tan) override;
				virtual int erase(int i) override;

				friend class Keys;
				friend class TBCs;
			};
			class TBCs final : public VectorPropertyBase<TBC, NiFloatData>
			{
			public:
				TBCs(NiFloatData& block) : VectorPropertyBase<TBC, NiFloatData>(block) {}

				virtual container get() const override;
				virtual void set(const container& c) override;

				virtual TBC get(int i) const override;
				virtual void set(int i, const TBC& tbc) override;

				virtual int insert(int i, const TBC& tbc) override;
				virtual int erase(int i) override;

				friend class Keys;
				friend class Tangents;
			};

			Keys m_keys;
			Tangents m_tans;
			TBCs m_tbcs;

			NiFloatData& m_super;
		};
		PropertyFcn<KeyType, NiFloatData, native::KeyType> m_keyType;
		IplnData m_keys;
	};

	class NiFloatInterpolator : public NiInterpolator
	{
	public:
		using native_type = native::NiFloatInterpolator;

	protected:
		friend class File;
		NiFloatInterpolator();
		NiFloatInterpolator(native_type* obj);

	public:
		virtual ~NiFloatInterpolator() = default;

		native_type& getNative() const;

		Property<float>& value() { return m_value; }
		const Property<float>& value() const { return m_value; }

		Assignable<NiFloatData>& data() { return m_data; }
		const Assignable<NiFloatData>& data() const { return m_data; }

	private:
		PropertyFcn<float, NiFloatInterpolator> m_value;
		AssignableFcn<NiFloatData, NiFloatInterpolator> m_data;
	};

	class NiBlendBoolInterpolator : public NiInterpolator//skipping NiBlendInterpolator for now
	{
	public:
		using native_type = native::NiBlendBoolInterpolator;

	protected:
		friend class File;
		NiBlendBoolInterpolator();
		NiBlendBoolInterpolator(native_type* obj);

	public:
		virtual ~NiBlendBoolInterpolator() = default;

		native_type& getNative() const;
	};

	class NiBlendFloatInterpolator : public NiInterpolator//skipping NiBlendInterpolator for now
	{
	public:
		using native_type = native::NiBlendFloatInterpolator;

	protected:
		friend class File;
		NiBlendFloatInterpolator();
		NiBlendFloatInterpolator(native_type* obj);

	public:
		virtual ~NiBlendFloatInterpolator() = default;

		native_type& getNative() const;
	};

	class NiTimeController : public NiObject
	{
	public:
		using native_type = native::NiTimeController;

	protected:
		friend class File;
		NiTimeController(native_type* obj);

	public:
		virtual ~NiTimeController() = default;

		native_type* nativePtr() const;

		//disallow assigning to these?
		//Assignable<NiTimeController>& nextCtlr();
		//Assignable<NiObjectNET>& target();

		Property<unsigned short>& flags() { return m_flags; }
		const Property<unsigned short>& flags() const { return m_flags; }

		Property<float>& frequency() { return m_frequency; }
		const Property<float>& frequency() const { return m_frequency; }

		Property<float>& phase() { return m_phase; }
		const Property<float>& phase() const { return m_phase; }

		Property<float>& startTime() { return m_startTime; }
		const Property<float>& startTime() const { return m_startTime; }

		Property<float>& stopTime() { return m_stopTime; }
		const Property<float>& stopTime() const { return m_stopTime; }

	private:
		PropertyFcn<unsigned short, NiTimeController> m_flags;
		PropertyFcn<float, NiTimeController> m_frequency;
		PropertyFcn<float, NiTimeController> m_phase;
		PropertyFcn<float, NiTimeController> m_startTime;
		PropertyFcn<float, NiTimeController> m_stopTime;
	};

	class NiSingleInterpController : public NiTimeController
	{
	public:
		using native_type = native::NiSingleInterpController;

	protected:
		friend class File;
		NiSingleInterpController(native_type* obj);

	public:
		virtual ~NiSingleInterpController() = default;

		native_type& getNative() const;

		Assignable<NiInterpolator>& interpolator() { return m_iplr; }

	private:
		AssignableFcn<NiInterpolator, NiSingleInterpController> m_iplr;
	};
}