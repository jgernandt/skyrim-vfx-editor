//Copyright 2021 Jonas Gernandt
//
//This file is part of VFX Editor.
//
//VFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VFX Editor. If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include "NiObjectNET.h"

namespace nif
{
	class NiInterpolator : public NiObject
	{
	public:
		NiInterpolator(native::NiInterpolator* obj);
		NiInterpolator(const NiInterpolator&) = delete;

		virtual ~NiInterpolator() = default;

		NiInterpolator& operator=(const NiInterpolator&) = delete;

		native::NiInterpolator& getNative() const;
	};

	class NiBoolData : public NiObject
	{
	public:
		NiBoolData();
		NiBoolData(native::NiBoolData* obj);
		NiBoolData(const NiBoolData&) = delete;

		virtual ~NiBoolData() = default;

		NiBoolData& operator=(const NiBoolData&) = delete;

		native::NiBoolData& getNative() const;
	};

	class NiBoolInterpolator : public NiInterpolator
	{
	public:
		NiBoolInterpolator();
		NiBoolInterpolator(native::NiBoolInterpolator* obj);
		NiBoolInterpolator(const NiBoolInterpolator&) = delete;

		virtual ~NiBoolInterpolator() = default;

		NiBoolInterpolator& operator=(const NiBoolInterpolator&) = delete;

		native::NiBoolInterpolator& getNative() const;

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
		NiFloatData();
		NiFloatData(native::NiFloatData* obj);
		NiFloatData(const NiFloatData&) = delete;

		virtual ~NiFloatData() = default;

		NiFloatData& operator=(const NiFloatData&) = delete;

		native::NiFloatData& getNative() const;
	};

	class NiFloatInterpolator : public NiInterpolator
	{
	public:
		NiFloatInterpolator();
		NiFloatInterpolator(native::NiFloatInterpolator* obj);
		NiFloatInterpolator(const NiFloatInterpolator&) = delete;

		virtual ~NiFloatInterpolator() = default;

		NiFloatInterpolator& operator=(const NiFloatInterpolator&) = delete;

		native::NiFloatInterpolator& getNative() const;

		IProperty<float>& value() { return m_value; }
		const IProperty<float>& value() const { return m_value; }

		IAssignable<NiFloatData>& data() { return m_data; }
		const IAssignable<NiFloatData>& data() const { return m_data; }

	private:
		Property<float> m_value;
		Assignable<NiFloatData> m_data;
	};

	class NiTimeController : public NiObject
	{
	public:
		NiTimeController(native::NiTimeController* obj);
		NiTimeController(const NiTimeController&) = delete;

		virtual ~NiTimeController() = default;

		NiTimeController& operator=(const NiTimeController&) = delete;

		native::NiTimeController& getNative() const;

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
		NiSingleInterpController(native::NiSingleInterpController* obj);
		NiSingleInterpController(const NiSingleInterpController&) = delete;

		virtual ~NiSingleInterpController() = default;

		NiSingleInterpController& operator=(const NiSingleInterpController&) = delete;

		native::NiSingleInterpController& getNative() const;

		//NiSingleInterpController
		IAssignable<NiInterpolator>& interpolator() { return m_iplr; }

	private:
		Assignable<NiInterpolator> m_iplr;
	};
}