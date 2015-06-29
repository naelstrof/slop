/* resource.hpp: Handles getting resources like textures and stuff.
 *
 * Copyright (C) 2014: Dalton Nell, Slop Contributors (https://github.com/naelstrof/slop/graphs/contributors).
 *
 * This file is part of Slop.
 *
 * Slop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Slop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Slop.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IS_RESOURCE_H_
#define IS_RESOURCE_H_

#include <stdlib.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

namespace slop {

class Resource {
public:

                    Resource();
                    ~Resource();
    std::string     getRealPath( std::string localpath );
    bool            validatePath( std::string path );
private:
    std::string     m_usrconfig;
    std::string     m_sysconfig;
};

}

extern slop::Resource* resource;

#endif // IS_RESOURCE_H_
