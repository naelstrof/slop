/* resource.cpp: Handles converting local paths to full paths.
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

#include "resource.hpp"

Resource::Resource() {
    // Find the configuration directory.
    // usually ~/.config/slop-wayland and /usr/share/slop-wayland
    char* config = getenv( "XDG_CONFIG_HOME" );
    if ( config == NULL ) {
        char* home = getpwuid(getuid())->pw_dir;
        usrconfig += home;
        usrconfig += "/.config/slop-wayland/";
        // SHADER_PREFIX is defined within the CMakeLists.txt
        sysconfig = SHADER_PREFIX;
        sysconfig += "/share/slop-wayland/";
        return;
    }
    usrconfig += config;
    usrconfig += "/slop-wayland/";
    sysconfig = SHADER_PREFIX;
    sysconfig += "/share/slop-wayland/";
}

std::string Resource::getRealPath( std::string localpath ) {
    if ( validatePath( usrconfig + localpath ) ) {
        return usrconfig + localpath;
    }
    if ( validatePath( sysconfig + localpath ) ) {
        return sysconfig + localpath;
    }
    std::string err = "The file or folder " + localpath + " was not found in either " + sysconfig + " or " + usrconfig + "\n";
    throw new std::runtime_error(err);
    return localpath;
}

bool Resource::validatePath( std::string path ) {
    struct stat st;
    const char* dirname = path.c_str();
    if ( stat( dirname, &st ) != 0 ) {
        return false;
    }
    // Lookin' good!
    return true;
}

