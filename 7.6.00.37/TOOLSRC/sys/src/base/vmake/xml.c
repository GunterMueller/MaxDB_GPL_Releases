/* CTS XML 

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

*/
struct xml_tag
{
    struct xml_tag      * prev ;
    char                * tag ;
	trg_info            * target;
};

struct xml_info
{
	struct xml_tag      * last;
	struct xml_tag      * to_open;
	FILE				* xmlfile
}



/* CTS XML */
xml_tag * open_xml_tag 
(
 xml_info * xmllist,
 char * tag, 
 trginfo * target, 
 int flush       /* if 1 then write all open xml_tags */
)
{

	
	if (flush)
	{
		flush_all_open_xml_tags (xmllist);
	}
}



int close_xml_tag (
xml_info  * xmllist
				   )
{
	xml_tag tag_to_delete;

	if (xmllist->to_open == NULL )
	{
		write_xml_tag (tag_to_delete, xmllist->xmlfile, 1);
	}
	else
	{
		/* reset openlist if only one tag is opened */
		if (xmllist->to_open == xmllist->last)
			xmllist->to_open = NULL;
	}
	tag_to_delete = xmllist->last;
	xmllist->last = xmllist->last->prev;

	FREE_AND_SET_TO_NULL( tag_to_delete->tag );
	FREE_AND_SET_TO_NULL( tag_to_delete );
}


/* use recursion */
int flush_all_open_xml_tags 
(
xml_info * xmllist
)
{
	xml_tag saved_last_tag;
	
	if (xmllist->to_open)
	{
		saved_last_tag = xmllist->last;

		if ( xmllist->to_open !=  xmllist->last ) /* found the last opened tag */
		{
			xmllist->last = xmllist->last->prev;
			flush_all_open_xml_tags (xmllist);
		}
		else
			xmllist->to_open = NULL;
		flush_xml_tag( saved_last_tag );
		xmllist->last = saved_last_tag;
	}
	else
	{
		DBG1 (( "fluxml: nothing to flush\n" ))
	}
}

 
int write_formated_start_tag 
(
 xml_tag * tag_to_write,
 FILE      xmlfile
)
{
	int tag_len;
	char * tag_string = NULL;
	char versionstring = versname (tag_to_write->target->version);
	if (strlen(versionstring) == 0)
		versionstring="none";

	/*        <>\n*/
	wrtie_tag (tag_to_write->tag, xmlfile, 0);
	write_tag ("NAME", xmlfile, 0);
	write_xml_text("tag_to_write->target->name");
	write_tag ("NAME", xmlfile, 1);
	write_tag ("VERSION", xmlfile, 1);
	write_xml_text("tag_to_write->target->version");
	write_tag ("VERSION", xmlfile, 1);
	
}
	
int write_xml_tag ( char * tag, FILE xmlfile, int endtag )
{
	char tagstring = palloc (strlen(tag)+ endtag ? 4 : 3);
	sprintf (tagstring, endtag ? "</%s>": "<%s>", tag);
	write_to_file
}


int write_xml_enty ( char * tag, FILE xmlfile, char * tagstring)
{
	
}

int write_xml_text (char * tag, FILE xmlfile)
{
	
}

	tag_len+= strlen(" name=") + strlen (tag_to_write->target->name);
	tag_len+= strlen(" version=") + strlen (versionstring) + 1; /* + \0*/

	tag_string = (char*) palloc ( tag_len );
	if ( sprintf ( tag_string,"<%s name=%s version=%s>", tag_to_write->tag, 
		 tag_to_write->target->name, versionstring) != tag_len )
	{
		error;
	}
}


&lt;
&gt;
" &quot;
' &apos;
& &amp;

  
int write_xml
(
 char * tag_string;
 FILE * xmlfile
)
{


}
