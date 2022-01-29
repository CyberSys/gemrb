/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 */

/**
 * @file Resource.h
 * Declares Resource class, base class for all resources
 * @author The GemRB Project
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "Plugin.h"
#include "Strings/StringConversion.h"

#include <unordered_map>

namespace GemRB {

/** Resource reference */
class DataStream;

inline bool IsStar(const ResRef& resref) {
	return resref[0] == '*';
}

template <typename T>
using ResRefMap = std::unordered_map<ResRef, T, CstrHashCI<ResRef>>;

/**
 * Base class for all GemRB resources
 */

using Resource = ImporterBase;

}

#endif
