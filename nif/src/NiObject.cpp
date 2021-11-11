#include "pch.h"
#include "NiObject.h"

nif::NiObject::NiObject(native_type* obj) : m_ptr{ obj }
{
	assert(m_ptr);
	m_ptr->AddRef();
}

nif::NiObject::~NiObject()
{
	m_ptr->SubtractRef();
}
