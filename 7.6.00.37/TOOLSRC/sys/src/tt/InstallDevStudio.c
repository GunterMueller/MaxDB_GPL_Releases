/* @lastChanged: "1998-07-02  17:20"
 * @filename:    InstallDevStudio.h
 * @purpose:     "Aufrufer fuer Installation des DevStudio"
 * @release:     #.#.#.#
 * @see:         ""
 * @creates:     Burkhard Diesing (D025086)
 *
 * @copyright:   "Copyright by SAP AG, 1998"


    ========== licence begin  GPL
    Copyright (C) 2001 SAP AG

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

 */
int main(int argc, char ** argv)
{
  char szCmd[128];
  int ret;
  char c;
  for (c='e'; c <= 'z'; c++) {
    sprintf(szCmd, "net use %c: \\\\P26326\\DevTool 2> NUL", c);
    if(!system(szCmd)) {
      printf("Drive mounted on %c:\n", c);
      system("SETUP.BAT");
      sprintf(szCmd, "net use %c: /delete 1> NUL", c);      
      system(szCmd);
      return 0;
    }    
  }
  return -1;
}
