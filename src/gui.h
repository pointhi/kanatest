
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

#include <config.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#if GTK_CHECK_VERSION(2,90,7)
#include <gdk/gdkkeysyms-compat.h>
#endif
#include <glib/gstdio.h>
#include <libxml/parser.h>
#ifdef MAEMO
#define MAEMO_VERSION "maemo2"

void        show_splash_screen  (void);
#endif

#define     BUFFER_SIZE         1024

#define     NUMBER_OF_SIGNS     104
#define     MIXED_SEPARATOR     1000        /* magic number for mixed mode */
#define     ANSWER_LEN          8
#define     KANA_SET_NAMES      15
#define     KANA_MODE_NAMES     3
#define     MODE_NAMES          2
#define     MAX_NUMBER_OF_SIGNS (NUMBER_OF_SIGNS*2)
#define     KANA_MAX_CHOICES    6

#define     ANSWER_DELIMITER    ","
#define     MAX_RANSWERS        10

#define     CHART_ROWS          27
#define     CHART_COLUMNS       5

#define     TRANSLATION_DOMAIN  "kanatest"

#if !GTK_CHECK_VERSION(2,13,4)
#define     gtk_widget_get_window(x) (x)->window
#endif

#if !GTK_CHECK_VERSION(2,90,0)
#define     gtk_bin_get_child(x)          (x)->child
#define     gtk_dialog_get_content_area(x) (x)->vbox
#endif

#if GTK_CHECK_VERSION(2,91,0)
#define     GtkObject GtkAdjustment
#endif

#if GTK_CHECK_VERSION(2,91,2)
#define     gtk_combo_box_new_text    gtk_combo_box_text_new
#define     gtk_combo_box_append_text gtk_combo_box_text_append_text
#endif

#if GTK_CHECK_VERSION(3,1,90)
#undef      GTK_FONT_SELECTION_DIALOG
#define     GTK_FONT_SELECTION_DIALOG                  GTK_FONT_CHOOSER
#define     gtk_font_selection_dialog_new(x)           gtk_font_chooser_dialog_new(x, NULL)
#define     gtk_font_selection_dialog_get_font_name    gtk_font_chooser_get_font
#define     gtk_font_selection_dialog_set_font_name    gtk_font_chooser_set_font
#define     gtk_font_selection_dialog_set_preview_text gtk_font_chooser_set_preview_text
#define     gtk_hbutton_box_new()                      gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL)
#define     gtk_hseparator_new()                       gtk_separator_new(GTK_ORIENTATION_HORIZONTAL)
#endif

#if GTK_CHECK_VERSION(3,3,2)
#undef      GTK_TABLE
#define     GTK_TABLE GTK_GRID
#define     gtk_table_attach(q, r, s, t, u, v, w, x, y, z) gtk_grid_attach(q, r, s, u, 1, 1)
#define     gtk_table_new(x, y, z)                         gtk_grid_new()
#define     gtk_table_set_col_spacings                     gtk_grid_set_column_spacing
#endif

typedef struct {

    GtkWidget   *stat_window;
    GSList      *stats_list;

    gint        old_column;
    gint        disable_dir;

    GtkWidget   *notebook;
    gint        active_tab;

    GtkWidget   *graph_viewport;
    GtkWidget   *graph_drawing_area;

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
    GtkWidget   *timer_colorbutton;

    GtkWidget   *notebook;
    gint        active_tab;

    GtkWidget   *number_of_kanas_label;
    GtkWidget   *check_buttons[NUMBER_OF_SIGNS];
    gboolean    disable_toggles;

    GtkWidget   *row_button_s[CHART_ROWS];
    GtkWidget   *row_button_c[CHART_ROWS];
#ifdef MAEMO
    gint        row_pos[CHART_ROWS][CHART_COLUMNS];
#else
    gchar       row_pos[CHART_ROWS][CHART_COLUMNS];
#endif

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

    gint        questions_table[MAX_NUMBER_OF_SIGNS]; // questions in order of asking
    gint        kana_set_table[MAX_NUMBER_OF_SIGNS]; // possible answers
    gint        kana_set_len[USER_DEFINED_LEN_OFFSET + 1];

    gint        kana_signs_set_0_idx[NUMBER_OF_SIGNS];      /* all kanas */
    gint        kana_signs_set_14_idx[NUMBER_OF_SIGNS];     /* user-defined-lessons - all kanas reserved */

} TEST;


typedef struct {

    GtkWidget       *main_window;

    GtkWidget       *about_window;
    GtkWidget       *about_textview;
    gint            about_link_index_main;
    GSList          about_links_list_main;
    gint            about_link_index_postcards;
    GSList          about_links_list_postcards;

    GdkPixbuf       *icon;
    GdkPixbuf       *logo;

    gboolean        hovering_over_link;
    GdkCursor       *hand_cursor;
    GdkCursor       *regular_cursor;
    GtkTextTag      *gui_url_tag;

    GtkWidget       *stop_button;
    GtkWidget       *start_button;
    GtkWidget       *reverse_button;
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
    GtkWidget       *label_tm;
    GtkWidget       *romaji_entry;
    GtkProgressBar  *progressbar;
    GtkWidget       *hseparator_up;
    GtkWidget       *vseparator_r;
    GtkWidget       *info_label;
    GtkWidget       *kana_choices_area;
    GtkWidget       *kana_choices[KANA_MAX_CHOICES];
#ifndef MAEMO
    GtkWidget       *logo_area;
#endif
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


void    gui_create_window       (GUI *appGUI);
void    gui_set_widgets_status  (gboolean mode, GUI *appGUI);
void    gui_next_text_question  (gint number, gint mode, GUI *appGUI);
void    gui_display_kana        (gint number, gint mode, GUI *appGUI);
void    gui_display_kana_choices(gint number, gint mode, GUI *appGUI);
void    gui_set_progress        (GUI *appGUI);
void    gui_disable_test        (GUI *appGUI);
void    gui_disable_start       (void);
void    gui_set_button_kana     (gint button_number, gint kana_number, gint mode, GUI *appGUI);
void    gui_disable_buttons     (gint first_button, GUI *appGUI);
void    gui_show_correct_answer (gint number, GUI *appGUI);

#endif /* _GUI_H */

