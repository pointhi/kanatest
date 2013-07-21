
/*
 * Kanatest
 *
 * Copyright (C) 2001-2004, 2006 Tomasz Maka <pasp@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gui.h"
#include "prefs.h"
#include "stats.h"
#include "i18n.h"
#ifdef MAEMO
#include <hildon/hildon.h>
#include <libosso.h>

#if defined(WIN32) || defined(WIN64)
#include "glib.h" /* g_path_get_dirname() */
#endif

#define APP_NAME "org.clayo.kanatest"
#define APP_VER "1.0"
#endif

int main (int argc, char **argv)
{
struct timeval timer;
gchar buffer[PATH_MAX];

GUI *appGUI = NULL;
CHART *chr = NULL;
STATISTICS *sts = NULL;
TEST *tst = NULL;
OPTIONS *opt = NULL;
#ifdef MAEMO
osso_context_t *osso_context = NULL;

    /* initialize maemo application */
    osso_context = osso_initialize(APP_NAME, APP_VER, TRUE, NULL);
    
    if (osso_context == NULL) {
        fprintf (stderr, "osso_initialize failed.\n");
        exit (1);
    }
#endif

    appGUI = g_new0 (GUI, 1);
    g_return_val_if_fail (appGUI != NULL, -1);

    chr = g_new0 (CHART, 1);
    g_return_val_if_fail (chr != NULL, -1);
    sts = g_new0 (STATISTICS, 1);
    g_return_val_if_fail (sts != NULL, -1);
    tst = g_new0 (TEST, 1);
    g_return_val_if_fail (tst != NULL, -1);

    opt = g_new0 (OPTIONS, 1);
    g_return_val_if_fail (opt != NULL, -1);

    /* register modules */
    appGUI->chr = chr;
    appGUI->sts = sts;
    appGUI->tst = tst;
    appGUI->opt = opt;

    /* default values */
    appGUI->old_kana_type = -1;
    appGUI->chr->chart_window = NULL;
    appGUI->tst->any_key = FALSE;
    appGUI->tst->test_state = FALSE;
    appGUI->sts->active_tab = -1;
    appGUI->opt->active_tab = -1;

    /* preferences update */

    g_snprintf (buffer, PATH_MAX, "%s%c%s", g_get_home_dir(), G_DIR_SEPARATOR, CONFIG_DIRNAME);

    if (g_file_test(buffer, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR) != TRUE) {
        prefs_read_config (CONFIG_FILENAME_OLD, CONFIG_DIRNAME_OLD);
        prefs_write_config (CONFIG_FILENAME, CONFIG_DIRNAME);
        if (g_file_test(prefs_get_config_filename (STATS_FILENAME_OLD, CONFIG_DIRNAME_OLD), G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR) == TRUE) {
            stats_read_list (STATS_FILENAME_OLD, CONFIG_DIRNAME_OLD, appGUI);
        }
        stats_write_list (STATS_FILENAME, CONFIG_DIRNAME, appGUI);
    }

    prefs_read_config (CONFIG_FILENAME, CONFIG_DIRNAME);
    stats_read_list (STATS_FILENAME, CONFIG_DIRNAME, appGUI);

    /* init */
#ifdef MAEMO
    hildon_gtk_init (&argc, &argv);
#else
    gtk_init (&argc, &argv);
#endif
    setlocale (LC_ALL, "");

#if defined(WIN32) || defined(WIN64)
    /* Set the locale directory as subdirectory of application path */
    char WINLOCALEDIR[MAX_PATH];
    strcpy(WINLOCALEDIR, g_path_get_dirname(argv[0]));
    strcat(WINLOCALEDIR, "\\share\\locale");

    bindtextdomain (PACKAGE, WINLOCALEDIR);
#else
    bindtextdomain (PACKAGE, LOCALEDIR);
#endif
    bind_textdomain_codeset (PACKAGE, "UTF-8");
    textdomain (PACKAGE);

    /* set seed */
    gettimeofday(&timer, NULL);

#if defined(WIN32) || defined(WIN64)
    srand((long)time(NULL));
#else
    srand48(timer.tv_usec + getpid());
#endif

#ifdef MAEMO
    show_splash_screen ();
#endif
    gui_create_window (appGUI);
    gtk_main ();

    stats_write_list (STATS_FILENAME, CONFIG_DIRNAME, appGUI);
    stats_free_list (appGUI);

    prefs_write_config (CONFIG_FILENAME, CONFIG_DIRNAME);

    g_free (opt);
    g_free (tst);
    g_free (sts);
    g_free (chr);
    g_free (appGUI);
#ifdef MAEMO
    /* deinitialize OSSO */
    osso_deinitialize (osso_context);
#endif
    return 0;
}


