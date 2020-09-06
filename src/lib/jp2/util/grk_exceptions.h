/*
 *    Copyright (C) 2016-2020 Grok Image Compression Inc.
 *
 *    This source code is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This source code is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#pragma once
#include <stdexcept>

namespace grk {

class DecodeUnknownMarkerAtEndOfTileException: public std::exception {};
class PluginDecodeUnsupportedException: public std::exception {};
class CorruptJP2BoxException: public std::exception {};
class TruncatedStreamException: public std::exception {};
// thrown if SOT marker from next tile is encountered
// note: this will only happen if number of tile parts is set to zero for SOT markers
class FoundNextTileSOTMarkerException: public std::exception {};
}
