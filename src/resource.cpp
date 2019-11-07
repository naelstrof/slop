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

slop::Resource::Resource() {
    // Find the configuration directory.
    // usually ~/.config/slop and /usr/share/slop
    char* config = getenv( "XDG_CONFIG_HOME" );
    if ( config == NULL ) {
        char* home = getpwuid(getuid())->pw_dir;
        usrconfig += home;
        usrconfig += "/.config/slop/";
        return;
    }
    usrconfig += config;
    usrconfig += "/slop/";
}

std::string slop::Resource::getRealPath( const std::string& localpath ) {
    if (localpath[0] == '/' && validatePath(dirname(localpath))) {
        return localpath;
    }
    if ( validatePath( usrconfig + localpath ) ) {
        return usrconfig + localpath;
    }
    std::string err = "The file or folder " + localpath + " was not found in " + usrconfig + "\n";
    throw std::runtime_error(err);
    return localpath;
}

std::string slop::Resource::dirname(const std::string& localpath) {
    size_t i = localpath.find_last_of("/");
    return (i != std::string::npos ? localpath.substr(0, i) : localpath);
}

bool slop::Resource::validatePath( const std::string& path ) {
    struct stat st;
    const char* dirname = path.c_str();
    if ( stat( dirname, &st ) != 0 ) {
        return false;
    }
    // Lookin' good!
    return true;
}

