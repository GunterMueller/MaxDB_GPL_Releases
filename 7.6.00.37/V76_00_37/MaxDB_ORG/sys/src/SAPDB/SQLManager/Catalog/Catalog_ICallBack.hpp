/*!
  @file           Catalog_ICallBack.hpp
  @author         ThomasA
  @brief          callback interface for catalog iterators  
  @ingroup        Catalog

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

#ifndef Catalog_ICallBack_hpp
#define Catalog_ICallBack_hpp

class Catalog_Object;

/*!
   @brief defines the interface of the callback called by Catalog_IAuthorization::ForEach
*/
class Catalog_ICallBack
{
public :
    /*!
       @brief this method is called for each authorization object found by Catalog_IAuthorization::ForEach
       @param pContext pointer to the current authorization object
     */
    virtual bool Callback(Catalog_Object* pContext) = 0;
};

#endif