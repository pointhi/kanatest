
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

#ifndef _PREFS_H
#define _PREFS_H

#define CONFIG_DIRNAME      ".kanatest"
#define CONFIG_DIRNAME_OLD  ".clay"
#define CONFIG_FILENAME     "config.xml"
#define CONFIG_FILENAME_OLD "kanatest.xml"
#define CONFIG_NAME         "kanatest_config"

#define MAX_FONTNAME_LEN    256
#define MAX_COLORNAME_LEN   64
#define MAX_BUFFER_SIZE     1024

struct kanatest_prefs
{
#ifndef MAEMO   
    gint        window_x;
    gint        window_y;
    gint        window_size_x;
    gint        window_size_y;
#endif    
    gint        repeat_mode;
    gint        kana_mode;
    gint        kana_set;
    gint        test_mode;
    gint        ca_timeout;
#ifndef MAEMO       
    gint        stat_window_x;
    gint        stat_window_y;
    gint        stat_size_x;
    gint        stat_size_y;
    gint        about_window_x;
    gint        about_window_y;
    gint        options_window_x;
    gint        options_window_y;
    gint        chart_window_x;
    gint        chart_window_y;
#endif    
    gint        stats_sort_column;
    gint        stats_sort_column_dir;
    gint        hiragana_mode;
    gint        ratio_begin;
    gint        ratio_end;
    gchar       kana_font_face[MAX_FONTNAME_LEN];
    gchar       kana_color[MAX_COLORNAME_LEN];
    gchar       romaji_color[MAX_COLORNAME_LEN];
    gchar       timer_color[MAX_COLORNAME_LEN];
    gchar       user_defined_lesson[NUMBER_OF_SIGNS+1];
};

extern  struct kanatest_prefs        config;

void    prefs_read_config           (gchar *config_filename, gchar *config_dirname);
void    prefs_write_config          (gchar *config_filename, gchar *config_dirname);
gchar*  prefs_get_config_filename   (gchar *config_filename, gchar *config_dirname);

#endif /* _PREFS_H */

