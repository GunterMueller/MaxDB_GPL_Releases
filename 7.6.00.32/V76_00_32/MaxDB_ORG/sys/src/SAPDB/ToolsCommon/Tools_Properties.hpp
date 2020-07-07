/*!
    @file           Tools_Properties.hpp
    @author         MarcW
    @brief          reading, storing properties to/from a file - Specification

\if EMIT_LICENCE

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


\endif
*/

#ifndef _Tools_Properties_HPP_
#define _Tools_Properties_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>

#include "Messages/Msg_List.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "ToolsCommon/Tools_List.hpp"
#include "ToolsCommon/Time/ToolsTime_Time.hpp"
#include "ToolsCommon/Time/ToolsTime_TimeFormat.hpp"

/*!
  @brief     Property class

  One instance of this class represents one property (a key/value pair). It is used
  in class Tools_Properties.
  @see Tools_Properties.
 */
class Tools_Property {
friend class Tools_Properties;
    public:
        /*!
            @brief   Constructor 

            Constructor to create a property from a key and a value. Makes copies of
            key and value.
            @param key   [IN]  this property's key
            @param value [IN]  this property's value
        */
        Tools_Property(const char* key, const char* value);

        /*!
            @brief   Destructor 
        */
        ~Tools_Property();

        /*!
            @brief get key

            Get this property's key
            @return this property's key
        */
        const char* getKey() const { return m_key; };

        /*!
            @brief get value

            Get this property's value
            @return this property's value
        */
        const char* getValue() const { return m_value; };

        /*!
            @brief set value

            Set this property's value
            @param newval [IN] this property's new value
        */
        void setValue(const char* newval);

        /*!
            @brief set next property

            A Properties object stores the properties that it
            contains as a linked list. Therefore a property can
            have a successor.
            @param prop [IN] this property's new successor
        */
        void setNext(Tools_Property* prop) { m_next = prop; };

    private:
        /*!
            @brief get next property

            @see setNext()
            @return this property's successor
        */
        Tools_Property* getNext() const { return m_next; };

        /*! @brief key */
        char* m_key;

        /*! @brief value */
        char* m_value;

        /*! @brief successor */
        Tools_Property* m_next;
};

inline Tools_Property::Tools_Property(const char* key, const char* value)
        : m_key(NULL),
          m_value(NULL),
          m_next(NULL) {
    m_key = new char[strlen(key)+1];
    SAPDB_strcpy(m_key,key);
    m_value = new char[strlen(value)+1];
    SAPDB_strcpy(m_value,value);
}

inline Tools_Property::~Tools_Property() {
    // if I have a successor, delete it, too
    // (deletion of a linked list...)
    if( m_next != NULL )
        delete m_next;
    delete[] m_key;
    delete[] m_value;
}

inline void Tools_Property::setValue(const char* newval) {
    delete[] m_value;
    m_value = new char[strlen(newval)+1];
    SAPDB_strcpy(m_value,newval);
}

/*!
  @brief     Properties class

  One instance of this class represents a set of properties. The properties can be stored
  to a file on the filesystem and also be loaded from there.
  Properties are persisted in a file as follows:
  <pre>
  key1=value1
  ...
  keyn=valuen
  </pre>
  Comment lines must start with a '#' character. Blank lines are ignored.
 */
class Tools_Properties {

public:
    /*!
        @brief Default Constructor

        Constructor without parameters.
        The Properties object contains no properties initially.
    */
    Tools_Properties();

    /*!
        @brief Constructor taking a file name

        An instance constructed with this constructor contains the
        properties from the passed file.
        @param rawfilename [IN] file name
     */
    Tools_Properties( const Tools_DynamicUTF8String& rawfilename );

    /*!
        @brief Destructor
    */
    virtual ~Tools_Properties();

    /*!
        @brief load properties

        Load properties from disk. A filename must be
        known to the object (either through constructor or some other method that takes a
        filename). The properties that were in this object before the call are discarded,
        if the operation was successful.
        @param lazy [IN] if true, properties are reloaded only if the file was modified after last load
        @return indicator whether operation was successful
    */

    SAPDB_Bool load( Msg_List &errOut, SAPDB_Bool lazy=false );

    SAPDB_Bool load( Msg_List &errOut, SAPDB_Bool lazy, SAPDB_Bool& reloadedNow );

    /*!
        @brief load properties

        Load properties from the passed file. The properties that were in this object
        before that call are discarded, if the operation was successful. If successful,
        subsequent load and store methods without parameters operate on the passed file.
        @param rawfilename [IN] file name
        @return indicator whether operation was successful
    */
    SAPDB_Bool load( Msg_List &errOut, const Tools_DynamicUTF8String& rawfilename, SAPDB_Bool lazy=false );

    /*!
        @brief store properties

        Store properties on disk. A filename must be known to the object (either through
        constructor or some other method that takes a filename).
        @return indicator whether operation was successful. 
    */
    SAPDB_Bool store(Msg_List &errOut);

    /*!
        @brief store properties

        Store properties to the passed file. If successful, subsequent load and
        store methods without parameters operate on the passed file.
        @param rawfilename [IN] file name
        @return indicator whether operation was successful
    */
    SAPDB_Bool store( Msg_List &errOut, const Tools_DynamicUTF8String& rawfilename );

    /*!
        @brief delete the file that is currently used

        @return indicator whether operation was sucessful
    */
    SAPDB_Bool deleteStorage(Msg_List &errOut);

    /*!
        @brief dump properties

        Dumps properties to the passed I/O stream. 
        @param f [IN] stream (e.g. stdout)
    */
    void dump(FILE* const f) const;

    /*!
        @brief set file name

        Sets a file name. Subsequent load/store oprations work on this file,
        if paramenterless methdos are used.
        @param s [IN] file name
    */
    void setFileName( const Tools_DynamicUTF8String& s );

    /*!
        @brief set a property

        Sets property with key "key" to value "value". An old value is overwritten.
        @param key   [IN] key of the property
        @param value [IN] value of the property
    */
    void setProperty( const Tools_DynamicUTF8String& key, const Tools_DynamicUTF8String& value );

    /*!
        @brief remove a property

        Removes property with key "key", if it exists. Has no effect, if it doesn't.
        @param key   [IN] key of the property
    */
    void removeProperty( const Tools_DynamicUTF8String& key );

    /*!
        @brief get a property value

        Gets the value of the property with key "key".
        @param key   [IN] key of the property
        @return value of the property
    */
    Tools_DynamicUTF8String getProperty( const Tools_DynamicUTF8String& key ) const;

    /*!
        @brief get a property value with default

        Gets the value of the property with key "key". If no propery with key "key" is available,
        "defval" is returned.
        @param key    [IN] key of the property
        @param defval [IN] default value of the property
        @return value of the property
    */
    Tools_DynamicUTF8String getProperty( const Tools_DynamicUTF8String& key, const Tools_DynamicUTF8String& defval ) const;

    /*!
        @brief to checks whether there is a property at all 

        @return true if there is at least one property, false otherwise
    */
    SAPDB_Bool isEmpty() {return first==NULL;}

    /*!
        @brief get a list of all contained properties

        The caller must take care of the allocated memory
        @return a list of all contained properties
    */
    Tools_List<Tools_Property*>* getAllProperties() const;
    
    /*! @brief count contained properties */
    int getCount() const;

    static const char CommentPrefix;

protected:
    /*!
        @brief create property

        Creates a property from a string, that must contain a '=' character, that
        is used as separator of key and value. The substring left of the '=' is
        the key, the substring right of it is the value.
        @param line [IN] string containing a property
        @return property object
    */
    virtual Tools_Property* createProperty( const char* line) const;


private:
    static tsp00_C40c m_RTETextConverter;

    static const ToolsTime_TimeFormat m_FileModificationTimeFormat;
    SAPDB_Bool m_FileLoadedOnce;
    ToolsTime_Time m_LastModificationTime;

    /*!
        @brief pointer to first property

        It is the head of the list of properties contained in this instance.
    */
    Tools_Property* first;
    /*! @brief file name for load/store */
    char* filename;

    /*!
        @brief find property with key "key"

        Steps through linked list of contained properties and returns the one.
        @param head [IN] head of the list to seach in
        @param key [IN] key of requested property
        @return requested property object if it exists, NULL otherwise.
    */
    Tools_Property* findProperty( Tools_Property* head, const char* key ) const;
    
    /*!
        @brief insert property

        Inserts property "newp" as successor of property "pred". "newp" is not copied.
        The list starting with start will be updated with newp and returned.
        @param pred [IN] property, after which "newp" is to ne inserted
        @param newp [IN] new property
        @return head of linked list of properties
    */
    Tools_Property* insertProperty(Tools_Property* pred, Tools_Property* newp) const;

    /*!
        @brief convert a Tools_DynamicUTF8String to a NULL-terminated char array

        @param s [IN] the object to be converted
        @return pointer to NULL-terminated char array
    */
    char* getCharPointer( const Tools_DynamicUTF8String& s ) const;
};

#endif // _Tools_Properties_HPP_
