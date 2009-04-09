
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

#ifndef _GUI_H
#define _GUI_H

#include <gtk/gtk.h>
#include <time.h>

#define		BUFFER_SIZE		    1024

#define		NUMBER_OF_SIGNS		104
#define		MIXED_SEPARATOR		1000        /* magic number for mixed mode */
#define		ANSWER_LEN			8
#define		KANA_SET_NAMES		15
#define		KANA_MODE_NAMES		3
#define		MAX_NUMBER_OF_SIGNS (NUMBER_OF_SIGNS*2)

#define     ANSWER_DELIMITER    ","
#define     MAX_RANSWERS        10

#define     CHART_ROWS          27
#define     CHART_COLUMNS       5

#define     TRANSLATION_DOMAIN  "kanatest"

typedef struct {

    GtkWidget   *stat_window;
    GSList      *stats_list;

    gint        old_column;
    gint        disable_dir;

    GtkWidget   *notebook;
    gint        active_tab;

    gint        hiragana_counters[NUMBER_OF_SIGNS];
    gint        correct_hiragana_counters[NUMBER_OF_SIGNS];
    gint        katakana_counters[NUMBER_OF_SIGNS];
    gint        correct_katakana_counters[NUMBER_OF_SIGNS];

} STATISTICS;

typedef struct {

    GtkWidget   *options_window;

    GtkWidget   *auto_selection_window;
    GtkObject   *begin_range_spinbutton_adj;
    GtkWidget   *begin_range_spinbutton;
    GtkObject   *end_range_spinbutton_adj;
    GtkWidget   *end_range_spinbutton;
    GtkWidget   *kanas_textview;
    GtkWidget   *hiragana_radiobutton;
    GtkWidget   *select_button;
    GtkWidget   *kanas_found_label;

    GtkWidget   *rna_radio_button;
    GtkWidget   *rwa_radio_button;
    GtkWidget   *raa_radio_button;
    GtkWidget   *example_label;

    GtkWidget   *kana_colorbutton;
    GtkWidget   *romaji_colorbutton;

    GtkWidget   *notebook;
    gint        active_tab;

    GtkWidget   *number_of_kanas_label;
    GtkWidget   *check_buttons[NUMBER_OF_SIGNS];
    gboolean    disable_toggles;

    GtkWidget   *row_button_s[CHART_ROWS];
    GtkWidget   *row_button_c[CHART_ROWS];
    gchar       row_pos[CHART_ROWS][CHART_COLUMNS];

} OPTIONS;

typedef struct {

    GtkWidget   *chart_window;
    GtkWidget   *labels[CHART_ROWS*CHART_COLUMNS];

} CHART;

#define     KANA_SET_0_LEN              104
#define     KANA_SET_1_LEN              46
#define     KANA_SET_2_LEN              5
#define     KANA_SET_3_LEN              5
#define     KANA_SET_4_LEN              5
#define     KANA_SET_5_LEN              5
#define     KANA_SET_6_LEN              5
#define     KANA_SET_7_LEN              5
#define     KANA_SET_8_LEN              5
#define     KANA_SET_9_LEN              3
#define     KANA_SET_10_LEN             5 
#define     KANA_SET_11_LEN             3
#define     KANA_SET_12_LEN             25 
#define     KANA_SET_13_LEN             33 

#define     USER_DEFINED_LEN_OFFSET     14  /* -1 is for number of selected kanas  - offset 14 */

typedef struct {

    gint        question_counter;
    gint        right_answer_counter;
    gint        wrong_answer_counter;
    gint        max_entries_in_test;

    gboolean    any_key;
    gboolean    test_state;

    gint        questions_table[MAX_NUMBER_OF_SIGNS];
    gint        kana_set_len[USER_DEFINED_LEN_OFFSET + 1];

    gint        kana_signs_set_0_idx[NUMBER_OF_SIGNS];      /* all kanas */
    gint        kana_signs_set_14_idx[NUMBER_OF_SIGNS];     /* user-defined-lessons - all kanas reserved */

} TEST;


typedef struct {

    GtkWidget       *main_window;

    GtkWidget       *about_window;
    GtkWidget       *about_textview;
    gint            about_link_index;
    GSList          about_links_list;

    GdkPixbuf       *icon;
    GdkPixbuf       *logo;

    gboolean        hovering_over_link;
    GdkCursor       *hand_cursor;
    GdkCursor       *regular_cursor;
    GtkTextTag      *gui_url_tag;

    GtkWidget       *stop_button;
    GtkWidget       *start_button;
    GtkWidget       *stat_button;
    GtkWidget       *chart_button;
    GtkWidget       *quit_button;
    GtkWidget       *about_button;
    GtkWidget       *prefs_button;
    GtkWidget       *timer_label;

    GtkWidget       *frame_ro;
    GtkWidget       *combobox_lesson;
    GtkWidget       *combobox_kana_mode;
    GtkWidget       *label_ka;
    GtkWidget       *label_le;
    GtkWidget       *romaji_entry;
    GtkProgressBar  *progressbar;
    GtkWidget       *hseparator_up;
    GtkWidget       *logo_area;
    GtkWidget       *char_label;

    gint            old_kana_type;
    gint            time_counter;

    /* modules */
    CHART *chr;
    STATISTICS *sts;
    TEST *tst;
    OPTIONS *opt;

} GUI;

typedef struct {
    gpointer *data;
    GUI *appGUI;
} MESSAGE;


void			gui_create_window		(GUI *appGUI);
void            gui_set_widgets_status  (gboolean mode, GUI *appGUI);
void            gui_display_kana        (gint number, gint mode, GUI *appGUI);
void            gui_set_progress        (GUI *appGUI);
void            gui_disable_test        (GUI *appGUI);

void			gui_disable_start		(void);

#endif /* _GUI_H */

