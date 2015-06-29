/* resource.cpp: Handles getting resources like textures and stuff.
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

slop::Resource* resource = new slop::Resource();

slop::Resource::Resource() {
    // Find the configuration directory.
    // usually ~/.config/slop and /usr/share/slop
    char* config = getenv( "XDG_CONFIG_HOME" );
    if ( config == NULL ) {
        char* home = getpwuid(getuid())->pw_dir;
        m_usrconfig += home;
        m_usrconfig += "/.config/slop/";
        m_sysconfig = INSTALL_PREFIX;
        m_sysconfig += "/share/slop/";
        return;
    }
    m_usrconfig += config;
    m_usrconfig += "/slop/";
    m_sysconfig = INSTALL_PREFIX;
    m_sysconfig += "/share/slop/";
}

std::string slop::Resource::getRealPath( std::string localpath ) {
    if ( validatePath( m_usrconfig + localpath ) ) {
        return m_usrconfig + localpath;
    }
    if ( validatePath( m_sysconfig + localpath ) ) {
        return m_sysconfig + localpath;
    }
    std::string err = "The file or folder " + localpath + " was not found in either " + m_sysconfig + " or " + m_usrconfig + "\n";
    throw err;
    return localpath;
}

bool slop::Resource::validatePath( std::string path ) {

    struct stat st;

    const char* dirname = path.c_str();
    if ( stat( dirname, &st ) != 0 ) {
        return false;
    }
    // No read permissions.
    //if ( !(st.st_mode & S_IROTH) &&
         //(st.st_uid != getuid() || !(st.st_mode & S_IRUSR)) &&
         //(st.st_gid != getgid() || !(st.st_mode & S_IRGRP)) ) {
        //return false;
    //}
    // Lookin' good!
    return true;
}

