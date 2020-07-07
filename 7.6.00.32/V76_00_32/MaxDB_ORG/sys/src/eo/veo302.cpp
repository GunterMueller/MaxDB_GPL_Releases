/*!
  @file           veo302.cpp
  @author         JoergM
  @special area   C++ Wrapper for C main function mit environment argument
  @brief          This is needed for especially HP C/C++ mixup. Without it
              the static instantiations will not take place. The environment
              allows the main function to access its environment directly.
              Used for vserver to dump environment in xserver.prot

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/


extern "C" int c_main_with_envp(int argc, char *argv[], char *envp[]);

int main(int argc,
         char *argv[],
         char *envp[])
{
    return c_main_with_envp(argc, argv, envp);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/