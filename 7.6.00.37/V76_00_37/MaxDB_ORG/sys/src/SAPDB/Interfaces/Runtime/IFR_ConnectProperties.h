/*!
  @file           IFR_ConnectProperties.h
  @author         D039759
  @ingroup        IFR_Connection
  @brief          Connection properties
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef IFR_CONNECTPROPERTIES_H
#define IFR_CONNECTPROPERTIES_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"

IFR_BEGIN_NAMESPACE

/**
 * Property key for the COMPNAME connect property.
 */
#define IFR_CONNECTPROPERTY_COMPNAME     "COMPNAME"

/**
 * Property key for the APPLICATION connect property.
 */
#define IFR_CONNECTPROPERTY_APPLICATION  "APPLICATION"

/**
 * Property key for the APPVERSION connect property.
 */
#define IFR_CONNECTPROPERTY_APPVERSION  "APPVERSION"

/**
 * Property key for the SQLMODE connect property.
 */
#define IFR_CONNECTPROPERTY_SQLMODE     "SQLMODE"

/**
 * Property key for the TIMEOUT connect property.
 */
#define IFR_CONNECTPROPERTY_TIMEOUT     "TIMEOUT"
/**
 * Property key for the CACHELIMIT connect property.
 */
#define IFR_CONNECTPROPERTY_CACHELIMIT    "CACHELIMIT"

/**
 * Property key for the ISOLATIONLEVEL connect property.
 */
#define IFR_CONNECTPROPERTY_ISOLATIONLEVEL    "ISOLATIONLEVEL"

/**
 * Property key for the UNICODE connect property.
 */
#define IFR_CONNECTPROPERTY_UNICODE    "UNICODE"

/**
 * Property key for the PACKETCOUNT connect property.
 */
#define IFR_CONNECTPROPERTY_PACKETCOUNT    "PACKETCOUNT"

/**
 * Property key for the STATEMENTCACHESIZE connect property.
 */
#define IFR_CONNECTPROPERTY_STATEMENTCACHESIZE    "STATEMENTCACHESIZE"

/**
 * Property key for the HOSTNAME connect property.
 */
#define IFR_CONNECTPROPERTY_HOSTNAME    "HOSTNAME"

/**
 * Property key for the PORT connect property.
 */
#define IFR_CONNECTPROPERTY_PORT    "PORT"

/**
 * Property key for the DATABASE connect property.
 */
#define IFR_CONNECTPROPERTY_DATABASE   "DATABASE"

/**
 * Property key for the KEY connect property.
 */
#define IFR_CONNECTPROPERTY_KEY   "KEY"

/**
 * Property key for the SPACEOPTION property.
 */
#define IFR_CONNECTPROPERTY_SPACEOPTION   "SPACEOPTION"

/**
 * Property key for the VARIABLEINPUT property.
 */
#define IFR_CONNECTPROPERTY_VARIABLEINPUT   "VARIABLEINPUT"

/**
 * Property key for the MULTIDROPPARSEID property.
 */
#define IFR_CONNECTPROPERTY_MULTIDROPPARSEID   "MULTIDROPPARSEID"

/**
 * Property key for the OPTIMIZEDSTREAMS property.
 */
#define IFR_CONNECTPROPERTY_OPTIMIZEDSTREAMS   "OPTIMIZEDSTREAMS"

/**
 * Property key for the CHECKSCROLLABLE property.
 */
#define IFR_CONNECTPROPERTY_CHECKSCROLLABLE   "CHECKSCROLLABLE"

/**
 * Property key for the SELECT-FETCH-OPTIMIZATION property. "0" means off, "1" means on.
 */
#define IFR_CONNECTPROPERTY_SELECTFETCHOPTIMIZE "SELECTFETCHOPTIMIZE"

/**
 * Property key for the CONNECT MODE.
 */
#define IFR_CONNECTPROPERTY_CONNECTTYPE "CONNECTTYPE"

/**
 * A connection may have several properties, which can be specified
 * before the connect, which are:
 * <table border="1">
 *   <tr>
 *     <td><b>Key</b></td><td><b>Data Type</b></td><td><b>Description</b></td>
 *   </tr>
 *   <tr>
 *     <td><code>COMPNAME</code></td>
 *     <td><code>VARCHAR(64)</code></td>
 *     <td>The component name used to initialise the runtime environment. If it starts with an 
 *         'R', it is assumed to be a runtime library instead of an application.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>APPLICATION</code></td>
 *     <td><code>CHAR(3)</code></td>
 *     <td>The application that is connecting to the database. You will get an error
 *         if you don't use one of the predefined possible values.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>APPVERSION</code>
 *     <td><code>NUMBER(5)</code>
 *     <td>The component's version. This is computed: 
 *         major * 10000 + minor * 100 + correction_level.
 *         E.g. version 7.4.4  has the version identifier "70404".
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>SQLMODE</code>
 *     <td><code>VARCHAR</code>
 *     <td>The SQL Mode. One of "INTERNAL","ORACLE","ANSI", or "DB2".
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>UNICODE</code>
 *     <td><code>BOOLEAN</code>
 *     <td>If true/1, the connection is an unicode (UCS2) client, otherwise an ASCII client.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>TIMEOUT</code>
 *     <td><code>NUMBER</code>
 *     <td>The timeout of the connection in seconds (parameter of the CONNECT statement).
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>CACHELIMIT</code>
 *     <td><code>NUMBER</code>
 *     <td>The CACHELIMIT parameter of the connection (parameter of the CONNECT statement)
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>ISOLATIONLEVEL</code>
 *     <td><code>NUMBER</code>
 *     <td>The ISOLATION LEVEL parameter of the connection (parameter of the CONNECT statement)
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>PACKETCOUNT</code></td>
 *     <td><code>UNLIMITED, 1 ... n</code></td>
 *     <td>The number of different request packet structures that will be used for the connection.
 *         <code>UNLIMITED</code> means that there is no limit on the number.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>STATEMENTCACHESIZE</code></td>
 *     <td><code>UNLIMITED, 0 ... n</code></td>
 *     <td>The number of prepared statements the connection shall cache for reuse. 
 *         <code>UNLIMITED</code> means that there is no limit on the number of cached statements,
 *         0 means there is no caching at all. 
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>FEATURE</code></td>
 *     <td><code>SPACEOPTION,VARIABLEINPUT,MULTIPARSEIDDROP,OPTIMIZEDSTREAMS</code></td>
 *     <td>Requests one or more experimental feature from SQLDBC.</td>
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><code>ODBC_BIN2HEX</code></td>
 *     <td><code>0, 1</code></td>
 *     <td>If 1, column binary data is converted to hexadecimal string representation, if host type is ASCII.
 *         0 means no conversion to hexadecimal string representation.
 *     </td>
 *   </tr>
 * </table>
 */
class IFR_ConnectProperties
{
public:
    /**
     * Creates an empty property set.
     */
    IFR_ConnectProperties(IFR_Bool& memory_ok);

    /**
     * Creates an empty property set.
     */
    IFR_ConnectProperties(SAPDBMem_IRawAllocator& allocator, IFR_Bool& memory_ok);

    /**
     * Copy constructor.
     * @param copy property set to be copied.
     */
    IFR_ConnectProperties(const IFR_ConnectProperties& copy, IFR_Bool& memory_ok);

    /**
     * Copy constructor that uses not the same allocator.
     * @param copy Property set to be copied.
     * @param allocator The allocator to be used.
     */
    IFR_ConnectProperties(const IFR_ConnectProperties& copy,
                          SAPDBMem_IRawAllocator& allocator,
                          IFR_Bool& memory_ok);

    /**
     * Destructor.
     */
    ~IFR_ConnectProperties();
    
    /**
     * Sets a property.
     * @param key the property key (ASCII string).
     * @param value the property value (ASCII string).
     */
    void setProperty(const char *key, const char *value, IFR_Bool& memory_ok);
    
    /**
     * Checks whether a certain property is set.
     * @param key The key to check.
     * @return <code>true</code> if the property is set.
     */
    inline IFR_Bool containsProperty(const char *key) const
    {
        return (getProperty(key) != 0);
    }
        
    /**
     * Gets a property.
     * @param key the key to look up
     * @param defaultvalue the value to return if the key is not found (default: 0).
     * @return the value found in the property set, or the defaultvalue specified
     *         (which is 0 if not given explicitely).
     */
    const char *getProperty(const char *key, const char *defaultvalue=0) const;
    
    inline const char * getParameterString() const
    {
        return m_parameterstring.getBuffer();
    }

    SAPDBMem_IRawAllocator& allocator;
    
    /**
     * Trace method for SQL trace.
     * @param s The trace output stream.
     */
    void sqlTrace(IFR_TraceStream& s);

    void clear();
    
    /**
     * Internal structure for a property entry.
     */
    class Property
    {
    public:
        Property(SAPDBMem_IRawAllocator& allocator); //!< constuctor
        Property(const Property& copy, IFR_Bool& memory_ok); //!< copy constructor
        void assign(const Property& assign, IFR_Bool& memory_ok);
    public:
        IFR_String key;      //!< the name of the property
        IFR_String value;    //!< the content of the property
    private:
        Property(const Property& copy); //!< copy constructor
        Property& operator=(const Property& assign); //!< assignment 
    };
private:
    IFR_ConnectProperties(const IFR_ConnectProperties& copy);
    Property                 m_dummy;             //!< Dummy instance needed for vector creation. 
    IFRUtil_Vector<Property> m_properties;        //!< Storage of the properties.
    IFR_String               m_parameterstring;   //!< URL-encoded parameter string.

};


/**
 * Template specialisation for @c IFR_ConnectProperties::Property
 */
template<> class IFRUtil_CopyTraits<IFR_ConnectProperties::Property>
{
public:
    typedef SAPDBMem_IRawAllocator& ConstructorType;
    typedef IFRUtil_FalseType IsDefaultConstructible;
    typedef IFRUtil_FalseType IsCopyConstructible;

    static inline void copy(IFR_ConnectProperties::Property& target, const IFR_ConnectProperties::Property& source, IFR_Bool& check)
    {
        if(check) {
            target.assign(source, check);
        }
        return;
    }
    
    static inline ConstructorType init_helper(SAPDBMem_IRawAllocator* allocator, const IFR_ConnectProperties::Property& source)
    {
        return allocator==0 ? source.key.getAllocator() : *allocator;
    }

};

IFR_END_NAMESPACE

#endif // IFR_CONNECTPROPERTIES_H
