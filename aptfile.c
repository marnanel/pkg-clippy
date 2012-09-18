/* 
 * Copyright (C) 2006-2011 Tollef Fog Heen <tfheen@err.no>
 * Copyright (C) 2001, 2002, 2005-2006 Red Hat Inc.
 * Copyright (C) 2010 Dan Nicholson <dbn.lists@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "aptfile.h"

/* TEMP: */
#include <stdio.h>

/*
If "packages" contains exactly one \n and it's at the end of the string,
then this function strips that \n and returns TRUE.
Otherwise it returns FALSE and "packages" is unchanged.
*/
static gboolean
aptfile_check_for_single_package (gchar *packages)
{
	/*

	   Three possibilities hee:
	   1) standard_output is "": there are no such packages
	   2) standard output contains exactly one \n, at the end: there is one package
	   3) standard_output contains other \n's; there are multiple packages..

	 */

	gchar *newline_pos;
	glong packages_length;

	if (*packages==0)
	{
		/* There appear to be no such packages. */
		printf("There appear to be no such packages.\n");
		return FALSE;
	}

	/* FIXME: is the output really utf-8? */
	/* FIXME: can we really pass -1 as the gssize here? */

	newline_pos = g_utf8_strchr (packages, -1, '\n');
	packages_length = g_utf8_strlen (packages, -1);

	printf ("Newline position is %d from the start.\n", newline_pos - packages);
	printf ("And string length is %ld.\n", packages_length);

	if (packages_length == (newline_pos-packages)+1)
	{
		printf ("A single line! Looks like we have what we're looking for.\n");
		*newline_pos = 0; /* knock out the trailing newline */
		return TRUE;
	}
	else
	{
		printf ("Multiple lines; give up.\n");
		return FALSE;
	}

}

const gchar*
aptfile_owner_package (const gchar *package_name)
{
	gchar *command_line;
	gchar *standard_output = NULL;
	gchar *standard_error = NULL;
	gint exit_status;
	gboolean spawned_ok;
	GError *err = NULL;
	gchar *result = NULL;

	printf ("We are looking for: %s\n", package_name);

	command_line = g_strdup_printf ("apt-file search -l %s.pc", package_name);

	printf ("The command line is: %s\n", command_line);

	g_clear_error (&err);

	spawned_ok =
		g_spawn_command_line_sync (command_line,
					&standard_output,
					&standard_error,
					&exit_status,
					&err);

	printf ("Spawning status is: %d\n", spawned_ok);
	printf ("Exit status is: %d\n", exit_status);
	printf ("Standard output: [%s]\n", standard_output);
	printf ("Standard error: [%s]\n", standard_error);

	if (spawned_ok)
	{
		gboolean single;

		single = aptfile_check_for_single_package (standard_output);

		printf ("Result of check for single package: %d [%s]\n", single, standard_output);

		if (single)
		{
			result = standard_output;
		}
	}
	else
	{
		/* result will be NULL */
		/* FIXME: clear the error too */
	}

	g_free (command_line);
	g_free (standard_error);
	if (result != standard_output)
	{
		g_free (standard_output);
	}

	return result;
}

gboolean
aptfile_attempt_installation (const gchar *package)
{
	gboolean result;
	GError *err = NULL;
	gint exit_status;
	gchar* argv[4];

	printf ("Attempting installation of %s.\n", package);

	/*
	I was going to miss off the sudo here, but nobody
	should be running ./configure as root!
	*/
	argv[0] = "sudo";
	argv[1] = "apt-get";
	argv[2] = "install";
	argv[3] = g_strdup (package);

	g_clear_error (&err);

	/*
	Is there a way NOT to capture stdout etc?
	Maybe passing NULL as things.
	Need to check API etc.
	-- Not exactly clear from API! Experiment.
	NULL/NULL works as intended; FIXME: add comment to complain.
	*/

	result =
		g_spawn_sync ("/",
			argv,
			NULL,
			G_SPAWN_SEARCH_PATH |
			G_SPAWN_CHILD_INHERITS_STDIN,
			NULL, NULL, NULL, NULL,
			&exit_status,
			&err);

        g_free (argv[3]);

	/* FIXME And clear the error if appropriate */

	printf ("Exit status was %d; result was %d.\n", exit_status, result);

	/*
	...though TBH it doesn't matter whether it was successful;
	either way we're going to retry the previous operation.
	*/

	return result;
}


