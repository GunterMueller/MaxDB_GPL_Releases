/* 
 * cba 1999/11/02
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>

static char *malloc_cat (char *, char *);
static int setuidroot (char *);

int main () {

        char *pFileName = 0;
        char *pInstRoot = 0;

        pInstRoot = getenv ("INSTROOT");
        if (pInstRoot == 0) {
                fprintf (stderr, "INSTROOT not set\n");
                return 1;
        }

        pFileName = malloc_cat (pInstRoot, "/pgm/lserver");

        if (pFileName == 0) {
                fprintf (stderr, "malloc failed\n");
                return 1;
        }

        if (setuidroot (pFileName) != 0) {
                return 1;
        }

        free (pFileName);

        return 0;
}

static char *malloc_cat (char *str0, char *str1) {
        char *str_out;
        int len;

        len = strlen (str0) + strlen (str1) + 1;
        str_out = malloc (len);

        strcpy (str_out, str0);
        strcat (str_out, str1);

        return (str_out);
}

static int setuidroot (char * pFileName) {
	struct passwd *entry;

	entry = getpwuid(0);

        if (chown (pFileName, 0, entry->pw_gid) != 0) {
                fprintf (stderr, "chown root failed\n");
                return 1;
        }

        if (chmod (pFileName, 04777) != 0) {
                fprintf (stderr, "chmod setuid failed\n");
                return 1;
        }

        return 0;
}

