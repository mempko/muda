#
# Copyright (C) 2012  Maxim Noah Khailo
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.6)
project(muda)

#setup boost
set(Boost_USE_STATIC_LIBS on)

if(APPLE)
	find_package(Boost 1.42 COMPONENTS filesystem date_time serialization regex signals thread)
else()
	find_package(Boost 1.42 COMPONENTS filesystem date_time serialization regex signals )
endif()
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

#setup wt
link_directories(${muda_SOURCE_DIR}/external/wt/lib)
include_directories(${muda_SOURCE_DIR}/external/wt/include)

add_subdirectory(src)
