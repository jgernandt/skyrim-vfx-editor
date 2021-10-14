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

#define NIFLIB_STATIC_LINK
#include "niflib.h"
#include "nif_math.h"

#include "obj/BSFadeNode.h"
#include "obj/NiBillboardNode.h"

#include "obj/NiBoolInterpolator.h"
#include "obj/NiBoolData.h"
#include "obj/NiFloatInterpolator.h"
#include "obj/NiFloatData.h"

#include "obj/NiParticleSystem.h"
#include "obj/NiPSysData.h"

#include "obj/BSPSysScaleModifier.h"
#include "obj/BSPSysSimpleColorModifier.h"
#include "obj/NiPSysGravityModifier.h"
#include "obj/NiPSysRotationModifier.h"

#include "obj/NiPSysBoxEmitter.h"
#include "obj/NiPSysCylinderEmitter.h"
#include "obj/NiPSysSphereEmitter.h"
#include "obj/NiPSysEmitterCtlr.h"

#include "obj/NiPSysAgeDeathModifier.h"
#include "obj/NiPSysPositionModifier.h"
#include "obj/NiPSysBoundUpdateModifier.h"
#include "obj/NiPSysUpdateCtlr.h"

#include "obj/NiStringExtraData.h"

#include "obj/BSEffectShaderProperty.h"
#include "obj/NiAlphaProperty.h"