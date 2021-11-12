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

		IProperty<KeyType>& keyType() { return m_keyType; }

	private:
		Property<KeyType, native::KeyType> m_keyType;
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

		IProperty<bool>& value() { return m_value; }
		const IProperty<bool>& value() const { return m_value; }

		IAssignable<NiBoolData>& data() { return m_data; }
		const IAssignable<NiBoolData>& data() const { return m_data; }

	private:
		Property<bool> m_value;
		Assignable<NiBoolData> m_data;
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

		IProperty<KeyType>& keyType() { return m_keyType; }
		std::shared_ptr<IProperty<KeyType>> keyType_ptr();
		InterpolationData<float>& iplnData() { return m_keys; }
		std::shared_ptr<InterpolationData<float>> iplnData_ptr();

	private:
		struct IplnData final : InterpolationData<float>
		{
			IplnData(NiFloatData& super);

			virtual IVectorProperty<Key<float>>& keys() override;
			virtual IVectorProperty<Tangent<float>>& tangents() override;
			virtual IVectorProperty<TBC>& tbc() override;

			virtual std::shared_ptr<IVectorProperty<Key<float>>> keys_ptr() override;
			virtual std::shared_ptr<IVectorProperty<Tangent<float>>> tangents_ptr() override;
			virtual std::shared_ptr<IVectorProperty<TBC>> tbc_ptr() override;

			class Keys;
			class Tangents;
			class TBCs;

			class Keys final : public VectorPropertyBase<Key<float>>
			{
			public:
				Keys(NiFloatData& super) : m_super{ super } {}

				virtual container get() const override;
				virtual void set(const container& c) override;

				virtual Key<float> get(int i) const override;
				virtual void set(int i, const Key<float>& key) override;

				virtual int insert(int i, const Key<float>& key) override;
				virtual int erase(int i) override;

				friend class Tangents;
				friend class TBCs;

				NiFloatData& m_super;
			};
			class Tangents final : public VectorPropertyBase<Tangent<float>>
			{
			public:
				Tangents(NiFloatData& super) : m_super{ super } {}

				virtual container get() const override;
				virtual void set(const container& c) override;

				virtual Tangent<float> get(int i) const override;
				virtual void set(int i, const Tangent<float>& tan) override;

				virtual int insert(int i, const Tangent<float>& tan) override;
				virtual int erase(int i) override;

				friend class Keys;
				friend class TBCs;

				NiFloatData& m_super;
			};
			class TBCs final :
				public VectorPropertyBase<TBC>
			{
			public:
				TBCs(NiFloatData& super) : m_super{ super } {}

				virtual container get() const override;
				virtual void set(const container& c) override;

				virtual TBC get(int i) const override;
				virtual void set(int i, const TBC& tbc) override;

				virtual int insert(int i, const TBC& tbc) override;
				virtual int erase(int i) override;

				friend class Keys;
				friend class Tangents;

				NiFloatData& m_super;
			};

			Keys m_keys;
			Tangents m_tans;
			TBCs m_tbcs;

			NiFloatData& m_super;
		};
		Property<KeyType, native::KeyType> m_keyType;
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

		IProperty<float>& value() { return m_value; }
		const IProperty<float>& value() const { return m_value; }

		IAssignable<NiFloatData>& data() { return m_data; }
		const IAssignable<NiFloatData>& data() const { return m_data; }

	private:
		Property<float> m_value;
		Assignable<NiFloatData> m_data;
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

		native_type& getNative() const;

		//NiTimeController
		//disallow assigning to these?
		//IAssignable<NiTimeController>& nextCtlr();
		//IAssignable<NiObjectNET>& target();
		IProperty<unsigned short>& flags() { return m_flags; }
		IProperty<float>& frequency() { return m_frequency; }
		IProperty<float>& phase() { return m_phase; }
		IProperty<float>& startTime() { return m_startTime; }
		IProperty<float>& stopTime() { return m_stopTime; }

	private:
		Property<unsigned short> m_flags;
		Property<float> m_frequency;
		Property<float> m_phase;
		Property<float> m_startTime;
		Property<float> m_stopTime;
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

		IAssignable<NiInterpolator>& interpolator() { return m_iplr; }

	private:
		Assignable<NiInterpolator> m_iplr;
	};
}