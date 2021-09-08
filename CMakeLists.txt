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

#use C++11
ADD_DEFINITIONS(-std=c++17)

#setup boost
set(Boost_USE_STATIC_LIBS on)

if(APPLE)
    set(Boost_LIBRARY_DIRS /opt/local/lib)
    set(Boost_INCLUDE_DIRS /opt/local/include)
    set(Boost_LIBRARIES boost_system boost_thread boost_regex boost_filesystem boost_date_time boost_serialization boost_program_options)
else()
	find_package(Boost COMPONENTS filesystem date_time serialization regex thread system)
endif()

link_directories(${Boost_LIBRARY_DIRS})
link_directories(/usr/local/lib)
include_directories(/usr/local/include)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src)
