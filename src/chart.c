
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

#include "i18n.h"
#include "gui.h"
#include "main.h"
#include "test.h"
#include "prefs.h"
#ifdef MAEMO
#include <hildon/hildon.h>
#endif
/*--------------------------------------------------------------------*/

gint
get_kana_sign_mask (gint row, gint column) {

gint kana_signs_mask[CHART_ROWS][CHART_COLUMNS] = {

    { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 },

    { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 0, 1, 0, 1 },
    { 1, 1, 1, 1, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 0 },

    { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },

    { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 },
    { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 },

    { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 }, { 1, 0, 1, 0, 1 }
};

    return kana_signs_mask[row][column];
}

/*--------------------------------------------------------------------*/

void
chart_display_kanas (gint type, GUI *appGUI) {

gint i, j;
gint pos, cpos;
gchar tmpbuf[BUFFER_SIZE];

    pos = cpos = 0;

    for(j=0; j < CHART_ROWS; j++) {
        for(i=0; i < CHART_COLUMNS; i++) {

            if(get_kana_sign_mask(j, i)) {

                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='20' face='%s'>%s</span> (%s)",
                            config.kana_font_face,
                            get_kana_sign(cpos, type, TRUE), get_kana_sign(cpos, ROMAJI, TRUE));

                gtk_label_set_markup (GTK_LABEL (appGUI->chr->labels[pos]), tmpbuf);
                cpos++;
            }

            pos++;
        }
    }
}

/*--------------------------------------------------------------------*/

void
chart_displaymode_hiragana_selected_cb (GtkWidget *entry, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    chart_display_kanas(HIRAGANA, appGUI);
}

/*--------------------------------------------------------------------*/

void
chart_displaymode_katakana_selected_cb (GtkWidget *entry, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    chart_display_kanas(KATAKANA, appGUI);
}

/*--------------------------------------------------------------------*/

void
chart_window_close_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
#ifdef MAEMO
    hildon_window_stack_pop_1 (hildon_window_stack_get_default());
#else
    gdk_window_get_root_origin (gtk_widget_get_window(appGUI->chr->chart_window),
                                &config.chart_window_x, &config.chart_window_y);
    gtk_widget_destroy (appGUI->chr->chart_window);
#endif
    appGUI->chr->chart_window = NULL;
}

/*--------------------------------------------------------------------*/

void
chart_delete_event_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    chart_window_close_cb (widget, user_data);
}

/*--------------------------------------------------------------------*/

gint
chart_key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    if (event->keyval == GDK_Escape) {
        chart_window_close_cb (widget, appGUI);
        return TRUE;
    }

    return FALSE;
}

/*--------------------------------------------------------------------*/

void
chart_close_button_cb (GtkWidget *widget, gpointer user_data) {
    chart_window_close_cb (widget, user_data);
}

/*--------------------------------------------------------------------*/

void
chart_create_window (GUI *appGUI) {

GtkWidget   *vbox1;
GtkWidget   *vbox2;
GtkWidget   *hbox1;
GtkWidget   *hbuttonbox;
#ifndef MAEMO
GtkWidget   *close_button;
#endif
GtkWidget   *hseparator;
GtkWidget   *table;
GtkWidget   *scrolledwindow;
GtkWidget   *viewport;
GtkWidget   *frames[CHART_ROWS*CHART_COLUMNS];
GtkWidget   *hiragana_radiobutton;
GtkWidget   *katakana_radiobutton;
GSList      *radiobutton_group = NULL;
gint        i, j, pos;

#ifdef MAEMO
    appGUI->chr->chart_window = hildon_stackable_window_new ();
    hildon_window_stack_push_1 (hildon_window_stack_get_default (), HILDON_STACKABLE_WINDOW (appGUI->chr->chart_window));  
    gtk_window_set_title (GTK_WINDOW (appGUI->chr->chart_window), _("Kana chart"));

    g_signal_connect (G_OBJECT (appGUI->chr->chart_window), "delete_event",
                        G_CALLBACK(chart_delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->chr->chart_window), "key_press_event",
                        G_CALLBACK(chart_key_press_cb), appGUI);
#else
    if (appGUI->chr->chart_window) return;

    appGUI->chr->chart_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_transient_for (GTK_WINDOW(appGUI->chr->chart_window),GTK_WINDOW(appGUI->main_window));
    gtk_window_set_resizable (GTK_WINDOW (appGUI->chr->chart_window), TRUE);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->chr->chart_window), 12);
    gtk_window_set_title (GTK_WINDOW (appGUI->chr->chart_window), _("Kana chart"));
    gtk_widget_set_size_request (appGUI->chr->chart_window, -1, 600);

    g_signal_connect (G_OBJECT (appGUI->chr->chart_window), "delete_event",
                        G_CALLBACK(chart_delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->chr->chart_window), "key_press_event",
                        G_CALLBACK(chart_key_press_cb), appGUI);

    gtk_window_move (GTK_WINDOW (appGUI->chr->chart_window),
                                config.chart_window_x, config.chart_window_y);
#endif
    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (appGUI->chr->chart_window), vbox2);
#ifdef MAEMO
    scrolledwindow = hildon_pannable_area_new ();
#else
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
#endif
    gtk_widget_show (scrolledwindow);
    gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (viewport), vbox1);

    table = gtk_table_new (CHART_ROWS, CHART_COLUMNS, FALSE);
    gtk_widget_show (table);
    gtk_box_pack_start (GTK_BOX (vbox1), table, FALSE, TRUE, 0);

    pos = 0;

    for(j=0; j < CHART_ROWS; j++) {
        for(i=0; i < CHART_COLUMNS; i++) {

            if(get_kana_sign_mask(j, i)) {

                frames[pos] = gtk_frame_new (NULL);
                gtk_widget_show (frames[pos]);
                gtk_table_attach (GTK_TABLE (table), frames[pos], i, i+1, j, j+1,
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
                gtk_container_set_border_width (GTK_CONTAINER (frames[pos]), 2);
                gtk_frame_set_shadow_type (GTK_FRAME (frames[pos]), GTK_SHADOW_ETCHED_OUT);

                appGUI->chr->labels[pos] = gtk_label_new (NULL);
                gtk_widget_show (appGUI->chr->labels[pos]);
                gtk_container_add (GTK_CONTAINER (frames[pos]), appGUI->chr->labels[pos]);
                gtk_misc_set_padding (GTK_MISC (appGUI->chr->labels[pos]), 0, 4);
            }

            pos++;
        }
    }

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 4);

    hiragana_radiobutton = gtk_radio_button_new_with_mnemonic (NULL, _("Hiragana"));
#ifdef MAEMO
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON (hiragana_radiobutton ), FALSE);
#endif
    gtk_widget_show (hiragana_radiobutton);
    g_signal_connect (G_OBJECT (hiragana_radiobutton), "clicked",
                      G_CALLBACK (chart_displaymode_hiragana_selected_cb), appGUI);
    gtk_box_pack_start (GTK_BOX (hbox1), hiragana_radiobutton, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hiragana_radiobutton), 4);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (hiragana_radiobutton), radiobutton_group);
    radiobutton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (hiragana_radiobutton));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (hiragana_radiobutton), TRUE);

    katakana_radiobutton = gtk_radio_button_new_with_mnemonic (NULL, _("Katakana"));
#ifdef MAEMO
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON (katakana_radiobutton ), FALSE);
#endif
    gtk_widget_show (katakana_radiobutton);
    g_signal_connect (G_OBJECT (katakana_radiobutton), "clicked",
                      G_CALLBACK (chart_displaymode_katakana_selected_cb), appGUI);
    gtk_box_pack_start (GTK_BOX (hbox1), katakana_radiobutton, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (katakana_radiobutton), 4);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (katakana_radiobutton), radiobutton_group);


    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_button_box_set_layout (GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_END);
    gtk_box_pack_end (GTK_BOX (vbox2), hbuttonbox, FALSE, TRUE, 0);

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_box_pack_end (GTK_BOX (vbox2), hseparator, FALSE, TRUE, 8);
#ifdef MAEMO
    gtk_widget_show (appGUI->chr->chart_window);
    chart_display_kanas(HIRAGANA, appGUI);
#else
    close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    gtk_widget_show (close_button);
    g_signal_connect (G_OBJECT (close_button), "clicked",
                        G_CALLBACK (chart_close_button_cb), appGUI);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), close_button);

#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default (close_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
#endif 

    chart_display_kanas(HIRAGANA, appGUI);

    gtk_widget_show (appGUI->chr->chart_window);
    gtk_widget_grab_default (close_button);
#endif
}

/*--------------------------------------------------------------------*/

