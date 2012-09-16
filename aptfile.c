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

gchar*
aptfile_owner_package (gchar *package_name)
{
	gchar *command_line;
	gchar *standard_output = NULL;
	gchar *standard_error = NULL;
	gint exit_status;
	gboolean spawned_ok;
	GError *err = NULL;

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

	g_free (command_line);
	g_free (standard_output);
	g_free (standard_error);

	return NULL;
}

