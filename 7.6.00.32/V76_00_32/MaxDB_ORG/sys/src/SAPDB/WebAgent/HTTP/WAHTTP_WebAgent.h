/*



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

typedef struct st_agent_client	*webagent_dummy1;

int InitWebAgent
(
 struct st_agent_server *as,
 struct st_agent_client	**wah
);

int ExitWebAgent
(
 struct st_agent_server *as,
 struct st_agent_client	*wah
);

enum en_af_returncode TryWebAgent
(
 struct st_agent_server *as,
 struct st_agent_client	*wah,
 struct st_agent_req	*req,
 struct st_agent_rep	*rep
);