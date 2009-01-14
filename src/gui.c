
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

#include <config.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>

#include "gui_utils.h"
#include "about.h"
#include "main.h"
#include "test.h"
#include "prefs.h"
#include "stats.h"
#include "stock_icons.h"
#include "chart.h"
#include "i18n.h"
#include "options.h"

#include "graphics.h"

/*--------------------------------------------------------------------*/

void
gui_set_progress (GUI *appGUI) {

gchar buffer[BUFFER_SIZE];

    if (config.repeat_mode == REPEAT_ALL) {
        gtk_widget_hide (GTK_WIDGET(appGUI->progressbar));
        gtk_widget_hide (GTK_WIDGET(appGUI->hseparator_up));
    } else {
        gtk_widget_show (GTK_WIDGET(appGUI->progressbar));
        gtk_widget_show (GTK_WIDGET(appGUI->hseparator_up));
        g_snprintf (buffer, BUFFER_SIZE, "[ %d / %d ]", appGUI->tst->question_counter, 
                    appGUI->tst->max_entries_in_test);
        gtk_progress_bar_set_text (GTK_PROGRESS_BAR (appGUI->progressbar), buffer);
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (appGUI->progressbar), (gfloat)appGUI->tst->question_counter / appGUI->tst->max_entries_in_test); 
    }
    gtk_entry_set_text (GTK_ENTRY(appGUI->romaji_entry), "");
}

/*--------------------------------------------------------------------*/

void
show_statistics_window_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    stats_create_window (appGUI);
}

/*--------------------------------------------------------------------*/

void
show_about_window_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    about_create_window (appGUI);
}

/*--------------------------------------------------------------------*/

void
show_options_window_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    options_create_window (appGUI);
}

/*--------------------------------------------------------------------*/

void
show_chart_window_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    chart_create_window (appGUI);
}

/*--------------------------------------------------------------------*/

void
update_timer (GUI *appGUI) {

static gchar buffer[BUFFER_SIZE];

    if (appGUI->tst->test_state == FALSE) return;
    
    g_snprintf (buffer, BUFFER_SIZE, "<big><tt><b>%02d:%02d</b></tt></big>", 
                appGUI->time_counter / 60, appGUI->time_counter % 60);
    gtk_label_set_markup (GTK_LABEL (appGUI->timer_label), buffer);
}

/*--------------------------------------------------------------------*/

gboolean
time_handler (GUI *appGUI) {

    appGUI->time_counter++;
    update_timer (appGUI);

  	return appGUI->tst->test_state;
}

/*--------------------------------------------------------------------*/

void
start_test_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    appGUI->tst->test_state = TRUE;
    g_timeout_add (1000, (GtkFunction) time_handler, appGUI);

    gtk_widget_show (appGUI->char_label);
    gtk_widget_hide (appGUI->logo_area);

    gui_set_widgets_status (FALSE, appGUI);

    gtk_widget_grab_focus (appGUI->romaji_entry);

    test_init (appGUI);
    test_generate_tables (appGUI->tst->max_entries_in_test, appGUI);

    gui_display_kana (appGUI->tst->questions_table[appGUI->tst->question_counter], config.kana_mode, appGUI);
    gui_set_progress (appGUI);

    appGUI->time_counter = 0;
    gtk_label_set_markup (GTK_LABEL (appGUI->timer_label), "<big><tt><b>00:00</b></tt></big>");
}

/*--------------------------------------------------------------------*/

void
stop_test_cb (GtkWidget *widget, gpointer user_data) {

GtkWidget *info_dialog = NULL;
gint response = -1;

    GUI *appGUI = (GUI *)user_data;

    info_dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(appGUI->main_window), 
                                                      GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                      GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, 
                                                      _("Test is in progress... Abort?"));

    gtk_window_set_title (GTK_WINDOW(info_dialog), _("Question"));
    gtk_widget_show (info_dialog);

    response = gtk_dialog_run(GTK_DIALOG(info_dialog));
    gtk_widget_destroy(info_dialog);

    if (response == GTK_RESPONSE_YES) {
        if (config.repeat_mode == REPEAT_ALL) {
            test_info (appGUI);
        } else {
            gui_disable_test (appGUI);
        }
    }
}

/*--------------------------------------------------------------------*/

void
gui_disable_test (GUI *appGUI) {

    appGUI->tst->test_state = FALSE;

    gtk_widget_show (appGUI->logo_area);
    gtk_widget_hide (appGUI->char_label);

    gui_set_widgets_status (TRUE, appGUI);

    gtk_widget_show (GTK_WIDGET(appGUI->progressbar));
    gtk_widget_show (GTK_WIDGET(appGUI->hseparator_up));
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (appGUI->progressbar), 0.0);
    gtk_entry_set_text (GTK_ENTRY(appGUI->romaji_entry), "");

    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (appGUI->progressbar),
                                _("Select test mode, kana set and press START"));
}

/*--------------------------------------------------------------------*/

void
gui_set_widgets_status (gboolean mode, GUI *appGUI) {

    gtk_widget_set_sensitive (appGUI->romaji_entry, !mode);
    gtk_widget_set_sensitive (appGUI->frame_ro, !mode);

    if (mode) {
        gtk_widget_show (appGUI->start_button);
        gtk_widget_show (appGUI->quit_button);
        gtk_widget_hide (appGUI->stop_button);

        gtk_widget_show (appGUI->stat_button);
        gtk_widget_show (appGUI->about_button);
        gtk_widget_show (appGUI->prefs_button);
        gtk_widget_show (appGUI->chart_button);

        gtk_widget_hide (appGUI->timer_label);

    } else {
        gtk_widget_hide (appGUI->start_button);
        gtk_widget_hide (appGUI->quit_button);
        gtk_widget_show (appGUI->stop_button);

        gtk_widget_hide (appGUI->stat_button);
        gtk_widget_hide (appGUI->about_button);
        gtk_widget_hide (appGUI->prefs_button);
        gtk_widget_hide (appGUI->chart_button);

        gtk_widget_show (appGUI->timer_label);
    }

    gtk_widget_set_sensitive (appGUI->combobox_kana_mode, mode);
    gtk_widget_set_sensitive (appGUI->combobox_lesson, mode);
    gtk_widget_set_sensitive (appGUI->label_ka, mode);
    gtk_widget_set_sensitive (appGUI->label_le, mode);
}

/*--------------------------------------------------------------------*/

void
gui_display_kana (gint number, gint mode, GUI *appGUI) {

gchar tmpbuf[BUFFER_SIZE], letbuf[BUFFER_SIZE];

    switch (mode) {
        case HIRAGANA:
            if (number >= MIXED_SEPARATOR) {
                number -= MIXED_SEPARATOR;
            }
            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                        config.kana_font_face,
                        config.kana_color,
                        get_kana_sign(number, HIRAGANA, TRUE));
            appGUI->old_kana_type = HIRAGANA;
            break;
        case KATAKANA:
            if (number >= MIXED_SEPARATOR) {
                number -= MIXED_SEPARATOR;
            }
            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                        config.kana_font_face,
                        config.kana_color,
                        get_kana_sign(number, KATAKANA, TRUE));
            appGUI->old_kana_type = KATAKANA;
            break;
        case ROMAJI:
            if (number >= MIXED_SEPARATOR) {
                number -= MIXED_SEPARATOR;
            }

            g_snprintf (letbuf, BUFFER_SIZE, "%s", get_kana_sign(number, ROMAJI, TRUE));

            if (appGUI->old_kana_type == HIRAGANA) {
                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='60' face='%s' color='%s'>%s</span>"
                                                 "<span font_desc='40' face='%s' color='%s'>(%s)</span>",
                                                 config.kana_font_face,
                                                 config.kana_color,
                                                 get_kana_sign(number, HIRAGANA, TRUE),
                                                 config.kana_font_face,
                                                 config.romaji_color,
                                                 letbuf);
            } else if (appGUI->old_kana_type == KATAKANA) {
                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='60' face='%s' color='%s'>%s</span>"
                                                 "<span font_desc='40' face='%s' color='%s'>(%s)</span>",
                                                 config.kana_font_face,
                                                 config.kana_color,
                                                 get_kana_sign(number, KATAKANA, TRUE),
                                                 config.kana_font_face,
                                                 config.romaji_color,
                                                 letbuf);
            } else {
                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                                                 config.kana_font_face,
                                                 config.romaji_color,
                                                 get_kana_sign(number, ROMAJI, TRUE));
            }
            break;
        case MIXED:
            if (number >= MIXED_SEPARATOR) {
                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                                                 config.kana_font_face,
                                                 config.kana_color,
                                                 get_kana_sign((number-MIXED_SEPARATOR), KATAKANA, TRUE));
                appGUI->old_kana_type = KATAKANA;
            } else {
                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                                                 config.kana_font_face,
                                                 config.kana_color,
                                                 get_kana_sign(number, HIRAGANA, TRUE));
                appGUI->old_kana_type = HIRAGANA;
            }
            break;
        default:
            g_warning ("Invalid mode selected.");
            break;
    }

    gtk_label_set_markup (GTK_LABEL (appGUI->char_label), tmpbuf);
}

/*--------------------------------------------------------------------*/

void
gui_close_window_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    gtk_window_get_size (GTK_WINDOW(appGUI->main_window),
                        &config.window_size_x, &config.window_size_y);
    gdk_window_get_root_origin ((appGUI->main_window)->window,
                                &config.window_x, &config.window_y);
    gtk_main_quit ();
}

/*--------------------------------------------------------------------*/

void
gui_delete_event_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    appGUI->tst->test_state = FALSE;
    gui_close_window_cb (widget, user_data);
}

/*--------------------------------------------------------------------*/

gint
gui_rm_key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    switch (event->keyval) {

        case GDK_Escape:
            gtk_entry_set_text (GTK_ENTRY(appGUI->romaji_entry), "");
            return TRUE;

        case GDK_Return:
            if (strlen ((gchar*)gtk_entry_get_text (GTK_ENTRY(appGUI->romaji_entry)))) {
                test_check_answer ((gchar*)(gtk_entry_get_text (GTK_ENTRY(appGUI->romaji_entry))), appGUI);
            }
            return TRUE;

    }

    return FALSE;
}

/*--------------------------------------------------------------------*/

gint
gui_mw_key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    if (appGUI->tst->test_state == FALSE) {

        switch (event->keyval) {
            case GDK_Return:
                start_test_cb (NULL, appGUI);
                return TRUE;
            case GDK_o:
                options_create_window (appGUI);
                return TRUE;
            case GDK_a:
                about_create_window (appGUI);
                return TRUE;
            case GDK_c:
                chart_create_window (appGUI);
                return TRUE;
            case GDK_s:
                stats_create_window (appGUI);
                return TRUE;
            case GDK_q:
                if (event->state & GDK_CONTROL_MASK) {  /* CTRL + q */
                    gui_close_window_cb (NULL, appGUI);
                }
                return TRUE;
        }

    } else if (event->keyval == GDK_Escape && appGUI->tst->any_key != FALSE) {   /* Esc in test mode? */
        stop_test_cb (NULL, appGUI);
    }

    appGUI->tst->any_key = TRUE;
    return FALSE;
}

/*--------------------------------------------------------------------*/

void
gui_combobox_kana_handler_cb (GtkComboBox *widget, gpointer user_data) {

    config.kana_mode = HIRAGANA + gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
}

/*--------------------------------------------------------------------*/

void
gui_combobox_kana_set_handler_cb (GtkComboBox *widget, gpointer user_data) {

    config.kana_set = gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
}

/*--------------------------------------------------------------------*/

void
gui_create_window (GUI *appGUI) {

GtkWidget       *frame1;
GtkWidget       *vbox0;
GtkWidget       *vbox1;
GtkWidget       *hseparator_down;
GtkWidget       *frame2;
GtkWidget       *frame;
GtkWidget       *hbox1;
GtkWidget       *hbox2;
GtkWidget       *empty_hbox;
GtkWidget       *hbuttonbox;
GtkWidget       *label;
GtkWidget       *alignment;
gint            i;
gchar           buffer[BUFFER_SIZE];

    appGUI->tooltips = gtk_tooltips_new();

    kanatest_register_stock_icons ();

    appGUI->main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->main_window), 4);
#ifndef REV
    sprintf (buffer, "Kanatest %s", VERSION);
#else
    sprintf (buffer, "Kanatest SVN r%d", REV);
#endif
    gtk_window_set_title (GTK_WINDOW (appGUI->main_window), buffer);

    gtk_window_set_default_size (GTK_WINDOW(appGUI->main_window),
                                 config.window_size_x, config.window_size_y);

    gtk_window_set_resizable (GTK_WINDOW (appGUI->main_window), TRUE);

    gtk_window_move (GTK_WINDOW (appGUI->main_window),
                                 config.window_x, config.window_y);

    g_signal_connect (G_OBJECT (appGUI->main_window), "delete_event",
                        G_CALLBACK(gui_delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->main_window), "key_press_event",
                        G_CALLBACK(gui_mw_key_press_cb), appGUI);

    gtk_widget_show (appGUI->main_window);

    appGUI->icon = gdk_pixbuf_new_from_inline (-1, kanatest_icon, FALSE, NULL);
    gtk_window_set_icon (GTK_WINDOW(appGUI->main_window), appGUI->icon);
    g_object_unref (appGUI->icon);

    vbox0 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox0);
    gtk_container_add (GTK_CONTAINER (appGUI->main_window), vbox0);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_show (frame1);
    gtk_container_set_border_width (GTK_CONTAINER (frame1), 4);
    gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_ETCHED_OUT);
    gtk_box_pack_start (GTK_BOX (vbox0), frame1, TRUE, TRUE, 0);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (frame1), vbox1);

    appGUI->logo_area = gtk_image_new();
    appGUI->logo = gdk_pixbuf_new_from_inline (-1, kanatest_logo, FALSE, NULL);
    gtk_widget_show(appGUI->logo_area);
    gtk_widget_set_size_request (appGUI->logo_area, -1, 255);  /* logo height */
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->logo_area, FALSE, FALSE, 0);
    gtk_image_set_from_pixbuf (GTK_IMAGE (appGUI->logo_area), appGUI->logo);

    appGUI->char_label = gtk_label_new (NULL);
    gtk_widget_set_size_request (appGUI->char_label, -1, 255);  /* logo height */
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->char_label, FALSE, FALSE, 0);

    appGUI->hseparator_up = gtk_hseparator_new ();
    gtk_widget_show (appGUI->hseparator_up);
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->hseparator_up, FALSE, TRUE, 0);

    appGUI->progressbar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
    gtk_widget_show (GTK_WIDGET(appGUI->progressbar));
    gtk_box_pack_start (GTK_BOX (vbox1), GTK_WIDGET(appGUI->progressbar), FALSE, FALSE, 0);

    hseparator_down = gtk_hseparator_new ();
    gtk_widget_show (hseparator_down);
    gtk_box_pack_start (GTK_BOX (vbox1), hseparator_down, FALSE, TRUE, 0);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 8);

    appGUI->frame_ro = gtk_frame_new (NULL);
    gtk_widget_show (appGUI->frame_ro);
    gtk_box_pack_start (GTK_BOX (hbox1), appGUI->frame_ro, FALSE, FALSE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (appGUI->frame_ro), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (appGUI->frame_ro), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

    label = gtk_label_new (NULL);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME(appGUI->frame_ro), label);
    g_snprintf (buffer, BUFFER_SIZE, "<b>%s: </b>", _("Romaji"));
    gtk_label_set_markup (GTK_LABEL (label), buffer);

    appGUI->romaji_entry = gtk_entry_new();
    gtk_entry_set_max_length (GTK_ENTRY(appGUI->romaji_entry), 3);
    g_signal_connect (G_OBJECT(appGUI->romaji_entry), "key_press_event",
                        G_CALLBACK(gui_rm_key_press_cb), appGUI);
    gtk_widget_set_size_request (appGUI->romaji_entry, 130, -1);
    gtk_widget_show (appGUI->romaji_entry);
    gtk_container_add (GTK_CONTAINER (alignment), appGUI->romaji_entry);

    empty_hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (empty_hbox);
    gtk_box_pack_start (GTK_BOX (hbox1), empty_hbox, TRUE, TRUE, 0);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_show (frame2);
    gtk_box_pack_end (GTK_BOX (hbox1), frame2, TRUE, FALSE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame2), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

    label = gtk_label_new (NULL);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME(frame2), label);
    gtk_label_set_markup (GTK_LABEL (label), NULL);

    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox2);
    gtk_container_add (GTK_CONTAINER (alignment), hbox2);

    appGUI->timer_label = gtk_label_new (NULL);
    gtk_widget_set_size_request (appGUI->timer_label, -1, 34);  /* icon height */
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->timer_label, TRUE, TRUE, 0);

    appGUI->stat_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_STATISTICS);
    GTK_WIDGET_UNSET_FLAGS (appGUI->stat_button, GTK_CAN_FOCUS);
    g_signal_connect (G_OBJECT (appGUI->stat_button), "clicked",
                        G_CALLBACK (show_statistics_window_cb), appGUI);
    gtk_widget_show (appGUI->stat_button);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->stat_button, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->stat_button), 2);
    gtk_tooltips_set_tip (appGUI->tooltips, appGUI->stat_button, _("Statistics"), NULL);

    appGUI->chart_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_CHART);
    GTK_WIDGET_UNSET_FLAGS (appGUI->chart_button, GTK_CAN_FOCUS);
    g_signal_connect (G_OBJECT (appGUI->chart_button), "clicked",
                        G_CALLBACK (show_chart_window_cb), appGUI);
    gtk_widget_show (appGUI->chart_button);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->chart_button, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->chart_button), 2);
    gtk_tooltips_set_tip (appGUI->tooltips, appGUI->chart_button, _("Kana chart"), NULL);

    appGUI->prefs_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_OPTIONS);
    GTK_WIDGET_UNSET_FLAGS (appGUI->prefs_button, GTK_CAN_FOCUS);
    g_signal_connect (G_OBJECT (appGUI->prefs_button), "clicked",
                        G_CALLBACK (show_options_window_cb), appGUI);
    gtk_widget_show (appGUI->prefs_button);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->prefs_button, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->prefs_button), 2);
    gtk_tooltips_set_tip (appGUI->tooltips, appGUI->prefs_button, _("Options"), NULL);

    appGUI->about_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_ABOUT);
    GTK_WIDGET_UNSET_FLAGS (appGUI->about_button, GTK_CAN_FOCUS);
    g_signal_connect (G_OBJECT (appGUI->about_button), "clicked",
                        G_CALLBACK (show_about_window_cb), appGUI);
    gtk_widget_show (appGUI->about_button);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->about_button, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->about_button), 2);
    gtk_tooltips_set_tip (appGUI->tooltips, appGUI->about_button, _("About"), NULL);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 8);

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (hbox1), frame, FALSE, FALSE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

    appGUI->combobox_kana_mode = gtk_combo_box_new_text ();
    gtk_widget_show (appGUI->combobox_kana_mode);
    g_signal_connect (G_OBJECT (appGUI->combobox_kana_mode), "changed",
                      G_CALLBACK (gui_combobox_kana_handler_cb), NULL);
    gtk_container_add (GTK_CONTAINER (alignment), appGUI->combobox_kana_mode);
    gtk_container_set_border_width (GTK_CONTAINER (alignment), 4);

    for(i=0; i < KANA_MODE_NAMES; i++) {
        gtk_combo_box_append_text (GTK_COMBO_BOX (appGUI->combobox_kana_mode), get_test_mode_name(i+1));
    }

    appGUI->label_ka = gtk_label_new (NULL);
    gtk_widget_show (appGUI->label_ka);
    gtk_frame_set_label_widget (GTK_FRAME(frame), appGUI->label_ka);
    g_snprintf (buffer, BUFFER_SIZE, "<b>%s: </b>", _("Test mode"));
    gtk_label_set_markup (GTK_LABEL (appGUI->label_ka), buffer);

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_end (GTK_BOX (hbox1), frame, FALSE, FALSE, 8);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

    appGUI->combobox_lesson = gtk_combo_box_new_text ();
    gtk_widget_show (appGUI->combobox_lesson);
    g_signal_connect (G_OBJECT (appGUI->combobox_lesson), "changed",
                      G_CALLBACK (gui_combobox_kana_set_handler_cb), NULL);
    gtk_container_add (GTK_CONTAINER (alignment), appGUI->combobox_lesson);
    gtk_container_set_border_width (GTK_CONTAINER (alignment), 4);

    for(i=0; i < KANA_SET_NAMES; i++) {
        gtk_combo_box_append_text (GTK_COMBO_BOX (appGUI->combobox_lesson), get_kana_set_name(i));
    }

    appGUI->label_le = gtk_label_new (NULL);
    gtk_widget_show (appGUI->label_le);
    gtk_frame_set_label_widget (GTK_FRAME(frame), appGUI->label_le);
    g_snprintf (buffer, BUFFER_SIZE, "<b>%s: </b>", _("Kana set"));
    gtk_label_set_markup (GTK_LABEL (appGUI->label_le), buffer);

    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox0), hbuttonbox, FALSE, TRUE, 0);
    gtk_box_set_spacing (GTK_BOX (hbuttonbox), 5);

    appGUI->start_button = gui_stock_label_button(_("Start"), GTK_STOCK_EXECUTE);
    GTK_WIDGET_UNSET_FLAGS (appGUI->start_button, GTK_CAN_FOCUS);
    g_signal_connect (G_OBJECT (appGUI->start_button), "clicked",
                        G_CALLBACK (start_test_cb), appGUI);
    gtk_widget_show (appGUI->start_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->start_button);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->start_button), 4);
    GTK_WIDGET_SET_FLAGS (appGUI->start_button, GTK_CAN_DEFAULT);
    gtk_tooltips_set_tip (appGUI->tooltips, appGUI->start_button, _("Press to begin testing..."), NULL);

    appGUI->stop_button = gui_stock_label_button(_("Stop"), GTK_STOCK_STOP);
    GTK_WIDGET_UNSET_FLAGS (appGUI->stop_button, GTK_CAN_FOCUS);
    g_signal_connect (G_OBJECT (appGUI->stop_button), "clicked",
                        G_CALLBACK (stop_test_cb), appGUI);
    gtk_widget_show (appGUI->stop_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->stop_button);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->stop_button), 4);
    GTK_WIDGET_SET_FLAGS (appGUI->stop_button, GTK_CAN_DEFAULT);
    gtk_tooltips_set_tip (appGUI->tooltips, appGUI->stop_button, _("Press to stop testing..."), NULL);

    appGUI->quit_button = gui_stock_label_button(_("Quit"), GTK_STOCK_QUIT);
    GTK_WIDGET_UNSET_FLAGS (appGUI->quit_button, GTK_CAN_FOCUS);
    g_signal_connect (G_OBJECT (appGUI->quit_button), "clicked",
                        G_CALLBACK (gui_close_window_cb), appGUI);
    gtk_widget_show (appGUI->quit_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->quit_button);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->quit_button), 4);
    GTK_WIDGET_SET_FLAGS (appGUI->quit_button, GTK_CAN_DEFAULT);
    gtk_tooltips_set_tip (appGUI->tooltips, appGUI->quit_button, _("Exit!"), NULL);

    gui_disable_test (appGUI);

    gtk_combo_box_set_active (GTK_COMBO_BOX (appGUI->combobox_lesson), config.kana_set);
    gtk_combo_box_set_active (GTK_COMBO_BOX (appGUI->combobox_kana_mode), config.kana_mode - HIRAGANA);

    if (config.enable_tooltips == TRUE) {
        gtk_tooltips_enable (appGUI->tooltips);
    } else {
        gtk_tooltips_disable (appGUI->tooltips);
    }

    gui_url_initialize(appGUI);
}

/*--------------------------------------------------------------------*/

