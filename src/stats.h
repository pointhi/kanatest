
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

#ifndef _STATS_H
#define _STATS_H

#define NUMBER_OF_COLUMNS   9

#define STATS_NAME          "kanatest_stats"
#define STATS_FILENAME      "statistics.xml"
#define STATS_FILENAME_OLD  "kanatest-stats.xml"
#define KANAS_FILENAME      "statistics-kanas.xml"

#define X_AXIS_MARGIN       50
#define Y_AXIS_MARGIN       10
#define X_BORDER            8
#define Y_BORDER            32
#define ARROW_A             3
#define ARROW_L             15
#define LEGEND_FONT_SIZE    9
#define TICK_WIDTH          6

typedef struct {
    guint date_day;
    guint date_month;
    guint date_year;
    guint date_hour;
    guint date_minute;
    guint test_type;
    guint test_mode;
    guint test_time;
    guint test_kana_set;
    guint test_questions;
    guint test_correct_answers;
    guint test_repeat_mode;
} stats_entry;

void    stats_create_window (GUI *appGUI);

gint    stats_get_records   (GUI *appGUI);
void    stats_remove_list   (GUI *appGUI);
void    stats_free_list     (GUI *appGUI);
void    stats_add_entry     (stats_entry *entry, GUI *appGUI);
void    stats_read_list     (gchar *stats_filename, gchar *config_dirname, GUI *appGUI);
void    stats_write_list    (gchar *stats_filename, gchar *config_dirname, GUI *appGUI);

#endif /* _STATS_H */

