
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
#include "i18n.h"
#include "gui_utils.h"
#include "chart.h"
#include "main.h"
#include "test.h"
#include "prefs.h"
#include "options.h"
#ifdef MAEMO
#include <hildon/hildon.h>
#define MIN_BEGIN_RANGE_VAL 0
#define MAX_BEGIN_RANGE_VAL 99

#define MIN_END_RANGE_VAL 1
#define MAX_END_RANGE_VAL 100

void select_all_action_cb (GtkWidget *widget, gpointer user_data);
void select_none_action_cb (GtkWidget *widget, gpointer user_data);
void select_invert_action_cb (GtkWidget *widget, gpointer user_data); 
void create_auto_select_window (GtkWidget *widget, gpointer user_data);

void refresh_select_kanas (gboolean select, GUI *appGUI);

void
create_options_window_menu (GUI *appGUI){

    HildonAppMenu *menu = HILDON_APP_MENU (hildon_app_menu_new ());
    GtkWidget *button1 = gtk_button_new_with_label (_("All"));
    GtkWidget *button2 = gtk_button_new_with_label (_("None"));
    GtkWidget *button3 = gtk_button_new_with_label (_("Invert"));
    GtkWidget *button4 = gtk_button_new_with_label (_("Statistics based selection"));
       
    g_signal_connect_after (button1, "clicked", G_CALLBACK (select_all_action_cb), appGUI);
    hildon_app_menu_append (menu, GTK_BUTTON (button1));    
    
    g_signal_connect_after (button2, "clicked", G_CALLBACK (select_none_action_cb), appGUI);
    hildon_app_menu_append (menu, GTK_BUTTON (button2));    
   
    g_signal_connect_after (button3, "clicked", G_CALLBACK (select_invert_action_cb), appGUI);
    hildon_app_menu_append (menu, GTK_BUTTON (button3));    
   
    g_signal_connect_after (button4, "clicked", G_CALLBACK (create_auto_select_window), appGUI);
    hildon_app_menu_append (menu, GTK_BUTTON (button4));    
    
    hildon_window_set_app_menu (HILDON_WINDOW (appGUI->opt->options_window), HILDON_APP_MENU (menu));
    gtk_widget_show_all (GTK_WIDGET (menu));
}

static void
show_auto_selection_confirmation_note (GUI *appGUI)
{
    GtkWidget *confirmation = NULL;
    gint response = -1;
    
    if(GTK_WIDGET_SENSITIVE(appGUI->opt->select_button))
    {
        confirmation = hildon_note_new_confirmation (GTK_WINDOW(appGUI->opt->auto_selection_window),
                                              _("Select"));
        response = gtk_dialog_run (GTK_DIALOG (confirmation));

        if (response == GTK_RESPONSE_OK)
            refresh_select_kanas (TRUE, appGUI);
    }
    gtk_widget_destroy (confirmation);    
}

static void
toggle_page_menu_cb (GtkNotebook *nb, GtkNotebookPage *page,
                             gint page_num, gpointer user_data)
{
    GUI *appGUI = (GUI *)user_data;
    
    if(page_num)
        create_options_window_menu (appGUI);
    else    
        hildon_window_set_app_menu (HILDON_WINDOW (appGUI->opt->options_window), NULL);    
}
#endif
/*--------------------------------------------------------------------*/

void
options_font_select_handler_cb (GtkWidget *widget, gpointer user_data) {

gchar *s, *n;
GtkWidget *font_selector;
gint response;
gchar tmpbuf[BUFFER_SIZE];

    GUI *appGUI = (GUI *)user_data;

    font_selector = gtk_font_selection_dialog_new (_("Select kana font"));
    gtk_window_set_modal (GTK_WINDOW(font_selector), TRUE);
    gtk_window_set_position (GTK_WINDOW(font_selector), GTK_WIN_POS_MOUSE);
    gtk_window_set_transient_for (GTK_WINDOW(font_selector), GTK_WINDOW(appGUI->opt->options_window));
    g_snprintf (tmpbuf, BUFFER_SIZE, "%s 16", config.kana_font_face);
    gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG(font_selector), tmpbuf);
    gtk_font_selection_dialog_set_preview_text (GTK_FONT_SELECTION_DIALOG (font_selector),
                                                EXAMPLE_KANA_TEXT);
    gtk_widget_show (font_selector);

    response = gtk_dialog_run (GTK_DIALOG (font_selector));

    if (response == GTK_RESPONSE_OK) {

        s = gtk_font_selection_dialog_get_font_name (GTK_FONT_SELECTION_DIALOG(font_selector));
        n = strrchr (s, ' ');
        *n= '\0';
        strncpy(config.kana_font_face, s, MAX_FONTNAME_LEN);
        g_free (s);
    }

    gtk_widget_destroy (font_selector);
}

/*--------------------------------------------------------------------*/

void
options_repeat_mode_changed_cb (GtkToggleButton *button, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(appGUI->opt->rna_radio_button)) == TRUE) {
       config.repeat_mode = REPEAT_NONE;
    } else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(appGUI->opt->rwa_radio_button)) == TRUE) {
       config.repeat_mode = REPEAT_WRONG;
    } else {
       config.repeat_mode = REPEAT_ALL;
    }
}

/*--------------------------------------------------------------------*/

gint
update_selected_kanas(GUI *appGUI) {

gint i, n;
gchar tmp[MAX_BUFFER_SIZE];

    for(i = n = 0; i < NUMBER_OF_SIGNS; i++) {
#ifdef MAEMO
        if (hildon_check_button_get_active (HILDON_CHECK_BUTTON (appGUI->opt->check_buttons[i])) == TRUE) {    
#else
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[i])) == TRUE) {
#endif
            n++;
        }
    }

    g_snprintf (tmp, BUFFER_SIZE, "%d", n);
    gtk_label_set_text (GTK_LABEL(appGUI->opt->number_of_kanas_label), tmp);

    return n;
}

/*--------------------------------------------------------------------*/

void
kana_selected_cb (GtkToggleButton *button, gpointer user_data) {

size_t pos;

    MESSAGE *msg = (MESSAGE *)user_data;

    if (msg->appGUI->opt->disable_toggles == FALSE) {

        pos = (size_t) msg->data;
#ifdef MAEMO
        if (hildon_check_button_get_active (HILDON_CHECK_BUTTON (button)) == TRUE) {
#else
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button)) == TRUE) {
#endif
            config.user_defined_lesson[pos] = '+';
        } else {
            config.user_defined_lesson[pos] = '-';
        }

        update_selected_kanas(msg->appGUI);
    }
}

/*--------------------------------------------------------------------*/

void
options_close_button_cb (GtkWidget *widget, gpointer user_data) {

GtkWidget *info_dialog;
GdkColor color;

    GUI *appGUI = (GUI *)user_data;

    if (update_selected_kanas(appGUI) < 2) {

        gtk_notebook_set_current_page (GTK_NOTEBOOK (appGUI->opt->notebook), 1);

        info_dialog = gtk_message_dialog_new (GTK_WINDOW(appGUI->opt->options_window),
                                              GTK_DIALOG_MODAL,
                                              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                              _("Please select at least two kanas to test."));

        gtk_window_set_transient_for (GTK_WINDOW(info_dialog), GTK_WINDOW(appGUI->opt->options_window));
        gtk_window_set_title (GTK_WINDOW(info_dialog), _("Error"));
        gtk_widget_show (info_dialog);
        gtk_dialog_run (GTK_DIALOG(info_dialog));
        gtk_widget_destroy (info_dialog);

    } else {

        gtk_color_button_get_color (GTK_COLOR_BUTTON(appGUI->opt->kana_colorbutton), &color);
        g_snprintf (config.kana_color, MAX_COLORNAME_LEN, "#%02X%02X%02X",
                    color.red * 256 / 65536, color.green * 256 / 65536, color.blue * 256 / 65536);

        gtk_color_button_get_color (GTK_COLOR_BUTTON(appGUI->opt->romaji_colorbutton), &color);
        g_snprintf (config.romaji_color, MAX_COLORNAME_LEN, "#%02X%02X%02X",
                    color.red * 256 / 65536, color.green * 256 / 65536, color.blue * 256 / 65536);

        gtk_color_button_get_color (GTK_COLOR_BUTTON(appGUI->opt->timer_colorbutton), &color);
        g_snprintf (config.timer_color, MAX_COLORNAME_LEN, "#%02X%02X%02X",
                    color.red * 256 / 65536, color.green * 256 / 65536, color.blue * 256 / 65536);

        appGUI->opt->active_tab = gtk_notebook_get_current_page (GTK_NOTEBOOK (appGUI->opt->notebook));
#ifdef MAEMO
        hildon_window_stack_pop_1 (hildon_window_stack_get_default());
        appGUI->opt->options_window = NULL;  
#else
        gdk_window_get_root_origin (gtk_widget_get_window(appGUI->opt->options_window),
                                    &config.options_window_x, &config.options_window_y);
        gtk_widget_destroy (appGUI->opt->options_window);
#endif
        prefs_write_config (CONFIG_FILENAME, CONFIG_DIRNAME);
    }
}

/*--------------------------------------------------------------------*/

void
delete_event_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    options_close_button_cb (widget, user_data);
}

/*--------------------------------------------------------------------*/

void
options_combobox_ca_timeout_handler_cb (GtkComboBox *widget, gpointer user_data) {
#ifdef MAEMO
gint index;
HildonTouchSelector *selector;

    selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (widget) );
    index = hildon_touch_selector_get_active(HILDON_TOUCH_SELECTOR(selector), 0);
    config.ca_timeout =  index;  
#else
    config.ca_timeout = gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
#endif
}

/*--------------------------------------------------------------------*/

gint
options_key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    if (event->keyval == GDK_Escape) {
        options_close_button_cb (NULL, appGUI);
        return TRUE;
    }

    return FALSE;
}

/*--------------------------------------------------------------------*/

void
options_combobox_dtf_handler_cb (GtkComboBox *widget, gpointer user_data) {

gint i, j, n, pos;
gchar tmp[BUFFER_SIZE];
#ifdef MAEMO
HildonTouchSelector *selector;
#endif
    GUI *appGUI = (GUI *)user_data;
#ifdef MAEMO
    selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (widget) );
    n = hildon_touch_selector_get_active(HILDON_TOUCH_SELECTOR(selector), 0);
#else
    n = gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
#endif
    pos = 0;

    for(j=0; j < CHART_ROWS; j++) {
        for(i=0; i < CHART_COLUMNS; i++) {
            if(get_kana_sign_mask(j, i)) {
                if (n == 3) {
                    g_snprintf (tmp, BUFFER_SIZE, "<span font_desc='12' face='%s'>%s</span><span font_desc='10' face='%s'>(%s)</span>",
                                config.kana_font_face, get_kana_sign(pos, HIRAGANA, TRUE), config.kana_font_face, get_kana_sign(pos, ROMAJI, TRUE));
                } else if (n == 4) {
                    g_snprintf (tmp, BUFFER_SIZE, "<span font_desc='12' face='%s'>%s</span><span font_desc='10' face='%s'>(%s)</span>",
                                config.kana_font_face, get_kana_sign(pos, KATAKANA, TRUE), config.kana_font_face, get_kana_sign(pos, ROMAJI, TRUE));
                } else if (n != 0) {
                    g_snprintf (tmp, BUFFER_SIZE, "<span font_desc='12' face='%s'>%s</span>",
                                config.kana_font_face, get_kana_sign(pos, n, TRUE));
                } else {
                    g_snprintf (tmp, BUFFER_SIZE, "<span font_desc='10' face='%s'>%s</span>",
                                config.kana_font_face, get_kana_sign(pos, ROMAJI, TRUE));
                }
#ifdef MAEMO
                g_snprintf (tmp, BUFFER_SIZE, "%s", get_kana_sign (pos, n, TRUE));
                gtk_button_set_label (GTK_BUTTON (appGUI->opt->check_buttons[pos]), tmp);
#else
                gtk_label_set_markup (GTK_LABEL(gtk_bin_get_child(GTK_BIN(GTK_BUTTON(appGUI->opt->check_buttons[pos])))), tmp);
#endif
                pos++;
            }
        }
    }
}

/*--------------------------------------------------------------------*/

void
kana_select_action (gint action, GUI *appGUI) {

    gint i, j, pos;
    gboolean state;

    pos = 0;

    for(j=0; j < CHART_ROWS; j++) {
        for(i=0; i < CHART_COLUMNS; i++) {

            if(get_kana_sign_mask (j, i)) {

                switch(action) {

                    case SELECT_ALL:
#ifdef MAEMO
                        hildon_check_button_set_active (HILDON_CHECK_BUTTON (appGUI->opt->check_buttons[pos]), TRUE);
#else
                        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[pos]), TRUE);
#endif
                        break;

                    case SELECT_NONE:
#ifdef MAEMO
                        hildon_check_button_set_active (HILDON_CHECK_BUTTON (appGUI->opt->check_buttons[pos]), FALSE);
#else
                        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[pos]), FALSE);
#endif
                        break;

                    case SELECT_INVERT:
#ifdef MAEMO
                        state = hildon_check_button_get_active (HILDON_CHECK_BUTTON(appGUI->opt->check_buttons[pos]));
                        hildon_check_button_set_active (HILDON_CHECK_BUTTON(appGUI->opt->check_buttons[pos]), !state);
#else
                        state = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[pos]));
                        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[pos]), !state);
#endif
                        break;

                    default:
                        break;
                };

                pos++;
            }
        }
    }
}


/*--------------------------------------------------------------------*/

void
select_all_action_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    kana_select_action (SELECT_ALL, appGUI);
}

/*--------------------------------------------------------------------*/

void
select_none_action_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    kana_select_action (SELECT_NONE, appGUI);
}

/*--------------------------------------------------------------------*/

void
select_invert_action_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    kana_select_action (SELECT_INVERT, appGUI);
}

/*--------------------------------------------------------------------*/

void
select_deselect_row (gint row, gboolean state, GUI *appGUI) {
gint i;

    for(i=0; i < CHART_COLUMNS; i++) {
        if (appGUI->opt->row_pos[row][i] != -1) {
#ifdef MAEMO
            hildon_check_button_set_active (HILDON_CHECK_BUTTON (appGUI->opt->check_buttons[(gint)appGUI->opt->row_pos[row][i]]), state);
#else
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[(gint)appGUI->opt->row_pos[row][i]]), state);
#endif
        }
    }
}

/*--------------------------------------------------------------------*/

void
row_button_select_cb (GtkWidget *widget, gpointer user_data) {

    MESSAGE *msg = (MESSAGE *)user_data;
    select_deselect_row ((size_t) msg->data, TRUE, msg->appGUI);
}

/*--------------------------------------------------------------------*/

void
row_button_deselect_cb (GtkWidget *widget, gpointer user_data) {

    MESSAGE *msg = (MESSAGE *)user_data;
    select_deselect_row ((size_t) msg->data, FALSE, msg->appGUI);
}

/*--------------------------------------------------------------------*/

void
refresh_select_kanas (gboolean select, GUI *appGUI) {

gboolean hiragana_mode;
double ratio, a, b;
gint begin, end, i, n;
GtkTextBuffer *textbuffer;
GtkTextIter iter_start, iter_end;
gchar buffer[BUFFER_SIZE];
#ifdef MAEMO
HildonTouchSelector *begin_range_selector;
HildonTouchSelector *end_range_selector;

    textbuffer = hildon_text_view_get_buffer(HILDON_TEXT_VIEW(appGUI->opt->kanas_textview));
#else
    textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(appGUI->opt->kanas_textview));
#endif    
    gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(textbuffer), &iter_start, 0);
    gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(textbuffer), &iter_end, -1);
    gtk_text_buffer_delete (GTK_TEXT_BUFFER(textbuffer), &iter_start, &iter_end);
#ifdef MAEMO
    begin_range_selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (appGUI->opt->begin_range_spinbutton));
    begin = hildon_touch_selector_get_active(HILDON_TOUCH_SELECTOR(begin_range_selector), 0);
 
    end_range_selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (appGUI->opt->end_range_spinbutton));
    end = hildon_touch_selector_get_active(HILDON_TOUCH_SELECTOR(end_range_selector), 0) + 1;
#else
    begin = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (appGUI->opt->begin_range_spinbutton));
    end = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (appGUI->opt->end_range_spinbutton));
#endif
    config.ratio_begin = begin;
    config.ratio_end = end;

    gtk_widget_set_sensitive (appGUI->opt->select_button, FALSE);

    if (begin > end) {
        g_snprintf (buffer, BUFFER_SIZE, "<i>%s</i>", _("no kanas found"));
        gtk_label_set_markup (GTK_LABEL(appGUI->opt->kanas_found_label), buffer);
        return;
    }

    hiragana_mode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(appGUI->opt->hiragana_radiobutton));
    config.hiragana_mode = hiragana_mode;

    if (select == TRUE) {
        kana_select_action (SELECT_NONE, appGUI);
    }

    n = 0;

    for (i=0; i < NUMBER_OF_SIGNS; i++) {

        if (hiragana_mode == TRUE) {
            a = appGUI->sts->correct_hiragana_counters[i];
            b = appGUI->sts->hiragana_counters[i];
        } else {
            a = appGUI->sts->correct_katakana_counters[i];
            b = appGUI->sts->katakana_counters[i];
        }

        if (b > 0) {
            ratio = a / b * 100.0;
        } else {
            ratio = 0;
        }

        if (ratio >= begin && ratio <= end) {
            if (select == TRUE) {
#ifdef MAEMO
                hildon_check_button_set_active (HILDON_CHECK_BUTTON (appGUI->opt->check_buttons[i]), TRUE);
#else
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[i]), TRUE);
#endif
            } else {
                g_snprintf (buffer, BUFFER_SIZE, "%s ",
                            get_kana_sign(i, hiragana_mode == TRUE ? HIRAGANA:KATAKANA, TRUE));
                gtk_text_buffer_insert (textbuffer, &iter_start, buffer, -1);
                n++;
            }
        }
    }

    if (n > 0) {
        gtk_widget_set_sensitive (appGUI->opt->select_button, TRUE);
        g_snprintf (buffer, BUFFER_SIZE, "<i>%d %s</i>", n, ngettext("kana found", "kanas found", n));
    } else {
        g_snprintf (buffer, BUFFER_SIZE, "<i>%s</i>", _("no kanas found"));
    }

    gtk_label_set_markup (GTK_LABEL(appGUI->opt->kanas_found_label), buffer);
}

/*--------------------------------------------------------------------*/

void
auto_select_select_button_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    refresh_select_kanas (TRUE, appGUI);
    gtk_widget_destroy (appGUI->opt->auto_selection_window);
}

/*--------------------------------------------------------------------*/

void
auto_select_close_button_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
#ifdef MAEMO
    show_auto_selection_confirmation_note (appGUI);
    hildon_window_stack_pop_1 (hildon_window_stack_get_default());
    appGUI->opt->auto_selection_window = NULL;
#else    
    gtk_widget_destroy (appGUI->opt->auto_selection_window);
#endif    
}

/*--------------------------------------------------------------------*/

void
auto_select_delete_event_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    auto_select_close_button_cb (widget, user_data);
}

/*--------------------------------------------------------------------*/

gint
auto_select_key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    if (event->keyval == GDK_Escape) {
        auto_select_close_button_cb (widget, appGUI);
        return TRUE;
    }

    return FALSE;
}

/*--------------------------------------------------------------------*/

void
hiragana_mode_changed_cb (GtkToggleButton *button, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    refresh_select_kanas (FALSE, appGUI);
}

/*--------------------------------------------------------------------*/

void
spinbutton_value_changed_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
    refresh_select_kanas (FALSE, appGUI);
}

/*--------------------------------------------------------------------*/

void
create_auto_select_window (GtkWidget *widget, gpointer user_data) {

GtkWidget   *vbox1;
GtkWidget   *frame;
GtkWidget   *alignment;
GtkWidget   *hbox1;
GtkWidget   *hbox2;
GSList      *radiobutton_group = NULL;
GtkWidget   *katakana_radiobutton;
GtkWidget   *label;
GtkWidget   *scrolledwindow;
GtkWidget   *table;
GtkWidget   *hseparator;
#ifndef MAEMO
GtkWidget   *hbuttonbox;
GtkWidget   *cancel_button;
#endif
gchar buffer[BUFFER_SIZE];
#ifdef MAEMO
gint i;
gchar buf[10];
HildonTouchSelector *begin_range_selector;
HildonTouchSelector *end_range_selector;
#endif

    GUI *appGUI = (GUI *)user_data;

#ifdef MAEMO
    appGUI->opt->auto_selection_window = hildon_stackable_window_new ();    
    hildon_window_stack_push_1 (hildon_window_stack_get_default (), HILDON_STACKABLE_WINDOW (appGUI->opt->auto_selection_window));    
    gtk_window_set_title (GTK_WINDOW (appGUI->opt->auto_selection_window), _("Statistics based selection"));   
#else 
    appGUI->opt->auto_selection_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (appGUI->opt->auto_selection_window), _("Statistics based selection"));
    gtk_window_set_resizable (GTK_WINDOW (appGUI->opt->auto_selection_window), TRUE);
    gtk_window_set_transient_for (GTK_WINDOW(appGUI->opt->auto_selection_window), GTK_WINDOW(appGUI->opt->options_window));
    gtk_window_set_modal (GTK_WINDOW(appGUI->opt->auto_selection_window), TRUE);
    gtk_window_set_default_size (GTK_WINDOW(appGUI->opt->auto_selection_window), 300, 400);
    gtk_window_move (GTK_WINDOW (appGUI->opt->auto_selection_window),
                     config.options_window_x + 100, config.options_window_y + 80);
#endif
    g_signal_connect (G_OBJECT (appGUI->opt->auto_selection_window), "delete_event",
                      G_CALLBACK(auto_select_delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->opt->auto_selection_window), "key_press_event",
                        G_CALLBACK(auto_select_key_press_cb), appGUI);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (appGUI->opt->auto_selection_window), vbox1);
    gtk_container_set_border_width (GTK_CONTAINER (vbox1), 8);

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox1), frame, FALSE, FALSE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 8, 4, 12, 0);

    hbox1 = gtk_hbox_new (FALSE, 16);
    gtk_widget_show (hbox1);
    gtk_container_add (GTK_CONTAINER (alignment), hbox1);

    appGUI->opt->hiragana_radiobutton = gtk_radio_button_new_with_mnemonic (NULL, _("Hiragana"));
#ifdef MAEMO
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON (appGUI->opt->hiragana_radiobutton), FALSE);
#endif
    gtk_widget_show (appGUI->opt->hiragana_radiobutton);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->opt->hiragana_radiobutton, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->opt->hiragana_radiobutton, GTK_CAN_FOCUS);
#endif
    gtk_box_pack_start (GTK_BOX (hbox1), appGUI->opt->hiragana_radiobutton, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (appGUI->opt->hiragana_radiobutton), radiobutton_group);
    radiobutton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (appGUI->opt->hiragana_radiobutton));
    g_signal_connect (G_OBJECT (appGUI->opt->hiragana_radiobutton), "toggled",
                      G_CALLBACK (hiragana_mode_changed_cb), appGUI);

    katakana_radiobutton = gtk_radio_button_new_with_mnemonic (NULL, _("Katakana"));
#ifdef MAEMO
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON (katakana_radiobutton), FALSE); 
#endif
    gtk_widget_show (katakana_radiobutton);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (katakana_radiobutton, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (katakana_radiobutton, GTK_CAN_FOCUS);
#endif
    gtk_box_pack_start (GTK_BOX (hbox1), katakana_radiobutton, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (katakana_radiobutton), radiobutton_group);
    radiobutton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (katakana_radiobutton));

    g_snprintf (buffer, BUFFER_SIZE, "<b>%s</b>", _("Statistics"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox1), frame, FALSE, FALSE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 8, 4, 12, 0);

#ifdef MAEMO
    table = gtk_table_new (1, 4, FALSE);
    gtk_widget_show (table);
    gtk_container_add (GTK_CONTAINER (alignment), table);
    gtk_table_set_col_spacings (GTK_TABLE(table), 8);

    appGUI->opt->begin_range_spinbutton = hildon_picker_button_new (HILDON_SIZE_AUTO,
                                                         HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
    hildon_button_set_title (HILDON_BUTTON (appGUI->opt->begin_range_spinbutton), _("From"));
    begin_range_selector = HILDON_TOUCH_SELECTOR (hildon_touch_selector_new_text ());
                                
    for (i = MIN_BEGIN_RANGE_VAL; i <= MAX_BEGIN_RANGE_VAL; i++) {
        g_snprintf (buf, sizeof(buf), "%d", i);
        hildon_touch_selector_append_text (begin_range_selector, buf);
    }
                                
    hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (appGUI->opt->begin_range_spinbutton), begin_range_selector);
    hildon_picker_button_set_active (HILDON_PICKER_BUTTON (appGUI->opt->begin_range_spinbutton), config.ratio_begin);

    gtk_widget_show (appGUI->opt->begin_range_spinbutton);
    g_signal_connect (G_OBJECT (appGUI->opt->begin_range_spinbutton), "value-changed",
                                G_CALLBACK (spinbutton_value_changed_cb), appGUI);

    gtk_table_attach_defaults (GTK_TABLE (table), appGUI->opt->begin_range_spinbutton, 0, 1, 0, 1);

    label = gtk_label_new ("%");
    gtk_widget_show (label);

    gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);

    appGUI->opt->end_range_spinbutton = hildon_picker_button_new (HILDON_SIZE_AUTO,
                                            HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
    hildon_button_set_title (HILDON_BUTTON (appGUI->opt->end_range_spinbutton), _("To"));
    end_range_selector = HILDON_TOUCH_SELECTOR (hildon_touch_selector_new_text ());

    for (i = MIN_END_RANGE_VAL; i <= MAX_END_RANGE_VAL; i++) {
        g_snprintf (buf, sizeof(buf), "%d", i);
        hildon_touch_selector_append_text (end_range_selector, buf);
    }

    hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (appGUI->opt->end_range_spinbutton), end_range_selector);
    hildon_picker_button_set_active (HILDON_PICKER_BUTTON (appGUI->opt->end_range_spinbutton), config.ratio_end-1);

    gtk_widget_show (appGUI->opt->end_range_spinbutton);
    g_signal_connect (G_OBJECT (appGUI->opt->end_range_spinbutton), "value-changed",
                                G_CALLBACK (spinbutton_value_changed_cb), appGUI);

    gtk_table_attach_defaults (GTK_TABLE (table), appGUI->opt->end_range_spinbutton, 2, 3, 0, 1);

    label = gtk_label_new ("%");
    gtk_widget_show (label);

    gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);
#else
    table = gtk_table_new (1, 7, FALSE);
    gtk_widget_show (table);
    gtk_container_add (GTK_CONTAINER (alignment), table);
    gtk_table_set_col_spacings (GTK_TABLE(table), 8);

    g_snprintf (buffer, BUFFER_SIZE, "%s:", _("From"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);

    appGUI->opt->begin_range_spinbutton_adj = gtk_adjustment_new (config.ratio_begin, 0, 99, 1, 10, 0);
    appGUI->opt->begin_range_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (appGUI->opt->begin_range_spinbutton_adj), 1, 0);
    g_signal_connect (appGUI->opt->begin_range_spinbutton, "value-changed", 
                      G_CALLBACK (spinbutton_value_changed_cb), appGUI);
    gtk_widget_show (appGUI->opt->begin_range_spinbutton);
    gtk_table_attach (GTK_TABLE (table), appGUI->opt->begin_range_spinbutton, 1, 2, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);

    label = gtk_label_new ("%");
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);

    label = gtk_label_new (NULL);
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    g_snprintf (buffer, BUFFER_SIZE, "%s:", _("To"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);

    appGUI->opt->end_range_spinbutton_adj = gtk_adjustment_new (config.ratio_end, 1, 100, 1, 10, 0);
    appGUI->opt->end_range_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (appGUI->opt->end_range_spinbutton_adj), 1, 0);
    g_signal_connect (appGUI->opt->end_range_spinbutton, "value-changed", 
                      G_CALLBACK (spinbutton_value_changed_cb), appGUI);
    gtk_widget_show (appGUI->opt->end_range_spinbutton);
    gtk_table_attach (GTK_TABLE (table), appGUI->opt->end_range_spinbutton, 5, 6, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);

    label = gtk_label_new ("%");
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1,
                      (GtkAttachOptions) 0,
                      (GtkAttachOptions) 0, 0, 0);
#endif
    g_snprintf (buffer, BUFFER_SIZE, "<b>%s</b>", _("Ratio range"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox1), frame, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 4, 0, 12, 0);

#ifdef MAEMO
    scrolledwindow = hildon_pannable_area_new ();
#else
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
#endif
    gtk_widget_show (scrolledwindow);
    gtk_container_add (GTK_CONTAINER (alignment), scrolledwindow);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

#ifdef MAEMO
    appGUI->opt->kanas_textview = hildon_text_view_new ();
#else
    appGUI->opt->kanas_textview = gtk_text_view_new ();
#endif    
    gtk_widget_show (appGUI->opt->kanas_textview);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), appGUI->opt->kanas_textview);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (appGUI->opt->kanas_textview), FALSE);
    gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (appGUI->opt->kanas_textview), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (appGUI->opt->kanas_textview), FALSE);
    gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW(appGUI->opt->kanas_textview), 2);
    gtk_text_view_set_left_margin (GTK_TEXT_VIEW(appGUI->opt->kanas_textview), 2);
    gtk_text_view_set_right_margin (GTK_TEXT_VIEW(appGUI->opt->kanas_textview), 2);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (appGUI->opt->kanas_textview), GTK_WRAP_WORD);

    g_snprintf (buffer, BUFFER_SIZE, "<b>%s</b>", _("Kanas"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    hbox2 = gtk_hbox_new (FALSE, 16);
    gtk_widget_show (hbox2);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, FALSE, 0);

    appGUI->opt->kanas_found_label = gtk_label_new (NULL);
    gtk_widget_show (appGUI->opt->kanas_found_label);
    gtk_box_pack_end (GTK_BOX (hbox2), appGUI->opt->kanas_found_label, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (appGUI->opt->kanas_found_label), TRUE);

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_box_pack_start (GTK_BOX (vbox1), hseparator, FALSE, FALSE, 4);
#ifndef MAEMO
    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox, FALSE, FALSE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing (GTK_BOX (hbuttonbox), 8);

    cancel_button = gtk_button_new_from_stock ("gtk-cancel");
    gtk_widget_show (cancel_button);
    g_signal_connect (G_OBJECT (cancel_button), "clicked",
                        G_CALLBACK (auto_select_close_button_cb), appGUI);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), cancel_button);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default (cancel_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);
#endif

    appGUI->opt->select_button = gui_stock_label_button(_("Select"), GTK_STOCK_INDEX);
    gtk_widget_show (appGUI->opt->select_button);
    g_signal_connect (G_OBJECT (appGUI->opt->select_button), "clicked",
                        G_CALLBACK (auto_select_select_button_cb), appGUI);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->opt->select_button);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default(appGUI->opt->select_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (appGUI->opt->select_button, GTK_CAN_DEFAULT);
#endif
#else
    appGUI->opt->select_button = gtk_button_new_with_label ("dummy");   
#endif
    if (config.hiragana_mode == TRUE) {
#ifdef MAEMO
        hildon_check_button_set_active (HILDON_CHECK_BUTTON (appGUI->opt->hiragana_radiobutton), TRUE);
#else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->hiragana_radiobutton), TRUE);
#endif
    } else {
#ifdef MAEMO
        hildon_check_button_set_active (HILDON_CHECK_BUTTON (katakana_radiobutton), TRUE);
#else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(katakana_radiobutton), TRUE);
#endif
    }

    refresh_select_kanas (FALSE, appGUI);

    gtk_widget_show (appGUI->opt->auto_selection_window);

}


/*--------------------------------------------------------------------*/

void
options_create_window (GUI *appGUI) {

GtkWidget       *vbox1;
GtkWidget       *vbox2;
GtkWidget       *vbox_n;
GtkWidget       *vbox_l;
GtkWidget       *hbox;
#ifndef MAEMO
GtkWidget       *hseparator;
GtkWidget       *hbuttonbox;
GtkWidget       *hbuttonbox_s;
#endif
GtkWidget       *alignment;
#ifndef MAEMO
GtkWidget       *close_button;
#endif
GtkWidget       *frame;
GtkWidget       *label;
GtkWidget       *combobox_ca_timeout;
GtkWidget       *combobox_dtf;
GtkWidget       *font_button;
GtkWidget       *table;
GdkColor        color;
GtkWidget       *frames[NUMBER_OF_SIGNS];
GtkWidget       *scrolledwindow;
GtkWidget       *viewport;
#ifndef MAEMO
GtkWidget       *auto_select_button;
GtkWidget       *select_all_button;
GtkWidget       *select_none_button;
GtkWidget       *invert_selection_button;
#endif
gint            i, j, pos;
gchar           buffer[BUFFER_SIZE];
static          MESSAGE msg[CHART_ROWS*CHART_COLUMNS];
static          MESSAGE msg2[CHART_ROWS];

#ifdef MAEMO
    appGUI->opt->options_window = hildon_stackable_window_new ();    
    hildon_window_stack_push_1 (hildon_window_stack_get_default (), HILDON_STACKABLE_WINDOW (appGUI->opt->options_window));  
    gtk_window_set_title (GTK_WINDOW (appGUI->opt->options_window), _("Options"));

    g_signal_connect (G_OBJECT (appGUI->opt->options_window), "delete_event",
                      G_CALLBACK(delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->opt->options_window), "key_press_event",
                        G_CALLBACK(options_key_press_cb), appGUI);
#else
    appGUI->opt->options_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_transient_for (GTK_WINDOW(appGUI->opt->options_window),GTK_WINDOW(appGUI->main_window));
    gtk_window_set_resizable (GTK_WINDOW (appGUI->opt->options_window), TRUE);
    gtk_window_set_modal (GTK_WINDOW(appGUI->opt->options_window), TRUE);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->opt->options_window), 12);
    gtk_window_set_title (GTK_WINDOW (appGUI->opt->options_window), _("Options"));
    gtk_window_set_default_size (GTK_WINDOW(appGUI->opt->options_window), 560, 460);

    g_signal_connect (G_OBJECT (appGUI->opt->options_window), "delete_event",
                      G_CALLBACK(delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->opt->options_window), "key_press_event",
                        G_CALLBACK(options_key_press_cb), appGUI);

    gtk_window_move (GTK_WINDOW (appGUI->opt->options_window),
                                config.options_window_x, config.options_window_y);
#endif
    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (appGUI->opt->options_window), vbox1);

    appGUI->opt->notebook = gtk_notebook_new();
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->opt->notebook, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->opt->notebook, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (appGUI->opt->notebook);
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK(appGUI->opt->notebook), GTK_POS_TOP);
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->opt->notebook, TRUE, TRUE, 0);

    label = gtk_label_new (_("General"));
    vbox_n = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_n);
    gtk_container_set_border_width (GTK_CONTAINER (vbox_n), 4);
    gtk_notebook_append_page (GTK_NOTEBOOK(appGUI->opt->notebook), vbox_n, label);

#ifdef MAEMO
    g_signal_connect (G_OBJECT (appGUI->opt->notebook), "switch-page",
                      G_CALLBACK (toggle_page_menu_cb), appGUI);
    scrolledwindow = hildon_pannable_area_new ();
    gtk_widget_show (scrolledwindow);
    gtk_box_pack_start (GTK_BOX (vbox_n), scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (viewport), vbox2);
#endif

    /*-----------------------------------------------------------------*/
    /* Drill options */

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
#ifdef MAEMO
    gtk_box_pack_start (GTK_BOX (vbox2), frame, FALSE, FALSE, 2);
#else
    gtk_box_pack_start (GTK_BOX (vbox_n), frame, FALSE, FALSE, 2);
#endif
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    g_snprintf (buffer, BUFFER_SIZE, "<b>%s:</b>", _("Drill options"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (alignment), vbox2);

    appGUI->opt->rna_radio_button = gtk_radio_button_new_with_label(NULL,
                                                                    _("Display each kana once"));
    gtk_box_pack_start (GTK_BOX (vbox2),appGUI->opt->rna_radio_button, TRUE, TRUE, 0);
#ifdef MAEMO
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON (appGUI->opt->rna_radio_button), FALSE);
#endif
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->opt->rna_radio_button), 8);
    gtk_widget_show (appGUI->opt->rna_radio_button);
    gtk_widget_set_tooltip_text (appGUI->opt->rna_radio_button, 
                                 _("When this option is enabled the kanatest will display every question once"));
    g_signal_connect (G_OBJECT (appGUI->opt->rna_radio_button), "toggled",
                      G_CALLBACK (options_repeat_mode_changed_cb), appGUI);

    appGUI->opt->rwa_radio_button = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(appGUI->opt->rna_radio_button),
                                                                                 _("Repeat wrongly-answered questions"));
    gtk_box_pack_start (GTK_BOX (vbox2),appGUI->opt->rwa_radio_button, TRUE, TRUE, 0);
#ifdef MAEMO
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON (appGUI->opt->rwa_radio_button), FALSE);
#endif
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->opt->rwa_radio_button), 8);
    gtk_widget_show (appGUI->opt->rwa_radio_button);
    gtk_widget_set_tooltip_text (appGUI->opt->rwa_radio_button,
                                 _("When this option is enabled the kanatest will repeat all wrongly answered questions at the end of test"));
    g_signal_connect (G_OBJECT (appGUI->opt->rwa_radio_button), "toggled",
                      G_CALLBACK (options_repeat_mode_changed_cb), appGUI);

    appGUI->opt->raa_radio_button = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(appGUI->opt->rna_radio_button),
                                                                                 _("Repeat all forever"));
    gtk_box_pack_start (GTK_BOX (vbox2),appGUI->opt->raa_radio_button, TRUE, TRUE, 0);
#if MAEMO
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON (appGUI->opt->raa_radio_button), FALSE);
#endif
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->opt->raa_radio_button), 8);
    gtk_widget_show (appGUI->opt->raa_radio_button);
    gtk_widget_set_tooltip_text (appGUI->opt->raa_radio_button,
                                 _("When this option is enabled the kanatest will repeat all questions forever"));
    g_signal_connect (G_OBJECT (appGUI->opt->raa_radio_button), "toggled",
                      G_CALLBACK (options_repeat_mode_changed_cb), appGUI);

#ifdef MAEMO
    table = gtk_table_new (1, 2, FALSE);
#else
    table = gtk_table_new (2, 1, FALSE);
#endif
    gtk_widget_show (table);
    gtk_container_set_border_width (GTK_CONTAINER (table), 12);
    gtk_box_pack_start (GTK_BOX (vbox2), table, FALSE, FALSE, 0);

    g_snprintf (buffer, BUFFER_SIZE, "%s:", _("Correct answer display timeout"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
#ifdef MAEMO
    gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);

    combobox_ca_timeout = hildon_picker_button_new (HILDON_SIZE_AUTO,
                                        HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
    label = hildon_touch_selector_new_text();

    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label),  _("Disabled"));    
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label),  _("1 sec"));    
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label),  _("2 sec"));  
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label),  _("3 sec"));
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label),  _("Until any key will be pressed"));
        
    hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (combobox_ca_timeout),
                            HILDON_TOUCH_SELECTOR (label));
    hildon_picker_button_set_active (HILDON_PICKER_BUTTON(combobox_ca_timeout), 0);

    gtk_widget_show (combobox_ca_timeout);

    g_signal_connect (G_OBJECT (combobox_ca_timeout), "value-changed",
        G_CALLBACK (options_combobox_ca_timeout_handler_cb), NULL);

    gtk_table_attach_defaults (GTK_TABLE (table), combobox_ca_timeout, 1, 2, 0, 1);

    hildon_touch_selector_set_active (HILDON_TOUCH_SELECTOR (label), 0, config.ca_timeout);
#else
    gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

#if GTK_CHECK_VERSION(2,24,0)
    combobox_ca_timeout = gtk_combo_box_text_new ();
#else
    combobox_ca_timeout = gtk_combo_box_new_text ();
#endif

#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (combobox_ca_timeout, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (combobox_ca_timeout, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (combobox_ca_timeout);
    g_signal_connect (G_OBJECT (combobox_ca_timeout), "changed",
                      G_CALLBACK (options_combobox_ca_timeout_handler_cb), NULL);
    gtk_table_attach (GTK_TABLE (table), combobox_ca_timeout, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
                      (GtkAttachOptions) (0), 8, 0);
#if GTK_CHECK_VERSION(2,24,0)
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_ca_timeout), _("Disabled"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_ca_timeout), _("1 sec"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_ca_timeout), _("2 sec"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_ca_timeout), _("3 sec"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_ca_timeout), _("Until any key will be pressed"));
#else
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_ca_timeout), _("Disabled"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_ca_timeout), _("1 sec"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_ca_timeout), _("2 sec"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_ca_timeout), _("3 sec"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_ca_timeout), _("Until any key will be pressed"));
#endif /*GTK_CHECK_VERSION(2,24,0)*/

    gtk_combo_box_set_active (GTK_COMBO_BOX (combobox_ca_timeout), config.ca_timeout);
#endif
    

    /*-----------------------------------------------------------------*/
    /* Display options */

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
#ifdef MAEMO
    gtk_box_pack_start (GTK_BOX (vbox2), frame, FALSE, FALSE, 2);
#else
    gtk_box_pack_start (GTK_BOX (vbox_n), frame, FALSE, FALSE, 2);
#endif
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    g_snprintf (buffer, BUFFER_SIZE, "<b>%s:</b>", _("Appearance"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (alignment), vbox2);
#ifdef MAEMO
    table = gtk_table_new (3, 2, FALSE);
#else
    table = gtk_table_new (6, 1, FALSE);
#endif  
    
    gtk_widget_show (table);
    gtk_container_set_border_width (GTK_CONTAINER (table), 12);
    gtk_box_pack_start (GTK_BOX (vbox2), table, TRUE, TRUE, 0);

    g_snprintf (buffer, BUFFER_SIZE, "%s:", _("Kana color"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
#ifdef MAEMO
    gtk_table_attach_defaults(GTK_TABLE (table), label, 0, 1, 0, 1);
#else
    gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
#endif
    gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

    appGUI->opt->kana_colorbutton = gtk_color_button_new ();
#ifdef MAEMO
    hildon_gtk_widget_set_theme_size(appGUI->opt->kana_colorbutton, HILDON_SIZE_FINGER_HEIGHT);
#endif
    gtk_widget_show (appGUI->opt->kana_colorbutton);

#ifdef MAEMO
    gtk_table_attach_defaults(GTK_TABLE (table), appGUI->opt->kana_colorbutton, 1, 2, 0, 1);
#else
    gtk_table_attach (GTK_TABLE (table), appGUI->opt->kana_colorbutton, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 6, 0);
#endif
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->opt->kana_colorbutton), 3);

    gdk_color_parse (config.kana_color, &color);
    gtk_color_button_set_color (GTK_COLOR_BUTTON (appGUI->opt->kana_colorbutton), &color);

    g_snprintf (buffer, BUFFER_SIZE, "%s:", _("Romaji color"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
#ifdef MAEMO
    gtk_table_attach_defaults(GTK_TABLE (table), label, 0, 1, 1, 2);
#else
    gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
#endif
    gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

    appGUI->opt->romaji_colorbutton = gtk_color_button_new ();
    gtk_widget_show (appGUI->opt->romaji_colorbutton);
#ifdef MAEMO
    hildon_gtk_widget_set_theme_size(appGUI->opt->romaji_colorbutton, HILDON_SIZE_FINGER_HEIGHT);
    gtk_table_attach_defaults (GTK_TABLE (table), appGUI->opt->romaji_colorbutton, 1, 2, 1, 2);
#else
    gtk_table_attach (GTK_TABLE (table), appGUI->opt->romaji_colorbutton, 3, 4, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 6, 0);
#endif
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->opt->romaji_colorbutton), 3);

    gdk_color_parse (config.romaji_color, &color);
    gtk_color_button_set_color (GTK_COLOR_BUTTON (appGUI->opt->romaji_colorbutton), &color);

    g_snprintf (buffer, BUFFER_SIZE, "%s:", _("Timer color"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
#ifdef MAEMO
    gtk_table_attach_defaults(GTK_TABLE (table), label, 1, 2, 1, 2);
#else
    gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
#endif
    gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

    appGUI->opt->timer_colorbutton = gtk_color_button_new ();
    gtk_widget_show (appGUI->opt->timer_colorbutton);
#ifdef MAEMO
    hildon_gtk_widget_set_theme_size(appGUI->opt->timer_colorbutton, HILDON_SIZE_FINGER_HEIGHT);
    gtk_table_attach_defaults (GTK_TABLE (table), appGUI->opt->timer_colorbutton, 2, 3, 1, 2);
#else
    gtk_table_attach (GTK_TABLE (table), appGUI->opt->timer_colorbutton, 5, 6, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 6, 0);
#endif
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->opt->timer_colorbutton), 3);

    gdk_color_parse (config.timer_color, &color);
    gtk_color_button_set_color (GTK_COLOR_BUTTON (appGUI->opt->timer_colorbutton), &color);

#ifdef MAEMO
    g_snprintf (buffer, BUFFER_SIZE, "%s:", _("Select kana font"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);

    gtk_table_attach_defaults(GTK_TABLE (table), label, 0, 1, 2, 3);
    gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

    font_button = hildon_button_new_with_text (HILDON_SIZE_AUTO_WIDTH |
                                          HILDON_SIZE_FINGER_HEIGHT, HILDON_BUTTON_ARRANGEMENT_VERTICAL,
                                          "A B C", "");
    gtk_button_set_alignment (GTK_BUTTON (font_button), 0.5, 0.5);
    hildon_button_set_title_alignment (HILDON_BUTTON (font_button), 0.5, 0.5);
    gtk_table_attach_defaults(GTK_TABLE (table), font_button, 1, 2, 2, 3);
#else
    font_button = gui_stock_label_button(_("Select kana font"), GTK_STOCK_SELECT_FONT);
#endif
    g_signal_connect (G_OBJECT (font_button), "clicked",
                      G_CALLBACK (options_font_select_handler_cb), appGUI);
    gtk_widget_show (font_button);

#ifdef MAEMO
    gtk_table_attach_defaults (GTK_TABLE (table), font_button, 0, 2, 2, 3);
#else
    gtk_table_attach (GTK_TABLE (table), font_button, 0, 2, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 8);
#endif
    /*-----------------------------------------------------------------*/
    /* User-defined lesson TAB */

    label = gtk_label_new(_("User-defined lesson"));
    vbox_l = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_l);
    gtk_container_set_border_width (GTK_CONTAINER (vbox_l), 4);
    gtk_notebook_append_page (GTK_NOTEBOOK(appGUI->opt->notebook), vbox_l, label);

#ifdef MAEMO
    scrolledwindow = hildon_pannable_area_new ();
#else
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
#endif
    gtk_widget_show (scrolledwindow);
    gtk_box_pack_start (GTK_BOX (vbox_l), scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (viewport), vbox2);

    table = gtk_table_new (CHART_ROWS, CHART_COLUMNS+2, FALSE);     /* 2 - S and C buttons */
    gtk_widget_show (table);
    gtk_box_pack_start (GTK_BOX (vbox2), table, FALSE, TRUE, 0);

    pos = 0;
    appGUI->opt->disable_toggles = TRUE;

    for(j=0; j < CHART_ROWS; j++) {
        for(i=0; i < CHART_COLUMNS; i++) {

            appGUI->opt->row_pos[j][i] = -1;
            if(get_kana_sign_mask (j, i)) {

                frames[pos] = gtk_frame_new (NULL);
                gtk_widget_show (frames[pos]);
                gtk_table_attach (GTK_TABLE (table), frames[pos], i, i+1, j, j+1,
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
                gtk_container_set_border_width (GTK_CONTAINER (frames[pos]), 2);
                gtk_frame_set_shadow_type (GTK_FRAME (frames[pos]), GTK_SHADOW_ETCHED_OUT);

                g_snprintf (buffer, BUFFER_SIZE, "%s", get_kana_sign (pos, ROMAJI, TRUE));

#ifdef MAEMO
                appGUI->opt->check_buttons[pos] = hildon_check_button_new (HILDON_SIZE_AUTO);        
                gtk_button_set_label (GTK_BUTTON (appGUI->opt->check_buttons[pos]), buffer);
#else
                appGUI->opt->check_buttons[pos] = gtk_check_button_new_with_mnemonic (buffer);
#endif

                gtk_widget_show (appGUI->opt->check_buttons[pos]);

                msg[pos].data = (gpointer)((size_t)pos);
                msg[pos].appGUI = appGUI;
                g_signal_connect (G_OBJECT (appGUI->opt->check_buttons[pos]), "toggled",
                                  G_CALLBACK (kana_selected_cb), &msg[pos]);

#if GTK_CHECK_VERSION(2,17,5)
                gtk_widget_set_can_focus (appGUI->opt->check_buttons[pos], FALSE);
#else
                GTK_WIDGET_UNSET_FLAGS (appGUI->opt->check_buttons[pos], GTK_CAN_FOCUS);
#endif
                gtk_container_add (GTK_CONTAINER (frames[pos]), appGUI->opt->check_buttons[pos]);

                if(config.user_defined_lesson[pos] == '+') {
#ifdef MAEMO
                    hildon_check_button_set_active (HILDON_CHECK_BUTTON(appGUI->opt->check_buttons[pos]), TRUE);
#else
                    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->check_buttons[pos]), TRUE);
#endif
                }
                appGUI->opt->row_pos[j][i] = pos;
                pos++;
            }
        }

        i++;

        appGUI->opt->row_button_s[j] = gui_stock_label_button (NULL, GTK_STOCK_APPLY);
        gtk_button_set_relief (GTK_BUTTON (appGUI->opt->row_button_s[j]), GTK_RELIEF_NONE);
#if GTK_CHECK_VERSION(2,17,5)
        gtk_widget_set_can_focus (appGUI->opt->row_button_s[j], FALSE);
#else
        GTK_WIDGET_UNSET_FLAGS (appGUI->opt->row_button_s[j], GTK_CAN_FOCUS);
#endif
        gtk_widget_set_tooltip_text (appGUI->opt->row_button_s[j], _("Select all kanas in row"));
        gtk_widget_show (appGUI->opt->row_button_s[j]);
        gtk_table_attach (GTK_TABLE (table), appGUI->opt->row_button_s[j], i, i+1, j, j+1,
                          (GtkAttachOptions) (GTK_FILL),
                          (GtkAttachOptions) (GTK_FILL), 0, 0);

        msg2[j].data = (gpointer)((size_t)j);
        msg2[j].appGUI = appGUI;
        g_signal_connect (G_OBJECT (appGUI->opt->row_button_s[j]), "clicked",
                          G_CALLBACK (row_button_select_cb), &msg2[j]);

        i++;

        appGUI->opt->row_button_c[j] = gui_stock_label_button (NULL, GTK_STOCK_CLEAR);
        gtk_button_set_relief (GTK_BUTTON (appGUI->opt->row_button_c[j]), GTK_RELIEF_NONE);
#if GTK_CHECK_VERSION(2,17,5)
        gtk_widget_set_can_focus (appGUI->opt->row_button_c[j], FALSE);
#else
        GTK_WIDGET_UNSET_FLAGS (appGUI->opt->row_button_c[j], GTK_CAN_FOCUS);
#endif
        gtk_widget_set_tooltip_text (appGUI->opt->row_button_c[j], _("Deselect all kanas in row"));
        gtk_widget_show (appGUI->opt->row_button_c[j]);
        gtk_table_attach (GTK_TABLE (table), appGUI->opt->row_button_c[j], i, i+1, j, j+1,
                          (GtkAttachOptions) (GTK_FILL),
                          (GtkAttachOptions) (GTK_FILL), 0, 0);

        g_signal_connect (G_OBJECT (appGUI->opt->row_button_c[j]), "clicked",
                            G_CALLBACK (row_button_deselect_cb), &msg2[j]);
    }

    appGUI->opt->disable_toggles = FALSE;

    hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox_l), hbox, FALSE, TRUE, 4);

    g_snprintf (buffer, BUFFER_SIZE, "<b>%s:</b>", _("Selected kanas"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
    gtk_misc_set_padding (GTK_MISC (label), 4, 0);

    appGUI->opt->number_of_kanas_label = gtk_label_new ("");
    gtk_widget_show (appGUI->opt->number_of_kanas_label);
    gtk_box_pack_start (GTK_BOX (hbox), appGUI->opt->number_of_kanas_label, FALSE, FALSE, 0);
    gtk_misc_set_padding (GTK_MISC (appGUI->opt->number_of_kanas_label), 4, 0);
#ifdef MAEMO
    combobox_dtf = hildon_picker_button_new (HILDON_SIZE_AUTO,
                                        HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
    hildon_button_set_title (HILDON_BUTTON (combobox_dtf), _("Display as"));
    label =  hildon_touch_selector_new_text ();
    
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label), _("Romaji"));
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label), _("Hiragana"));
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label), _("Katakana"));
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label), _("Hiragana + Romaji"));
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (label), _("Katakana + Romaji"));
    
    hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (combobox_dtf),
                            HILDON_TOUCH_SELECTOR (label));

    hildon_picker_button_set_active (HILDON_PICKER_BUTTON(combobox_dtf), 0);

    g_signal_connect (G_OBJECT (combobox_dtf), "value-changed",
             G_CALLBACK (options_combobox_dtf_handler_cb), appGUI);

    hildon_touch_selector_set_active (HILDON_TOUCH_SELECTOR (label), 0, 0);

    gtk_widget_show (combobox_dtf);
    gtk_box_pack_end (GTK_BOX (hbox), combobox_dtf, FALSE, FALSE, 0);
#else
    auto_select_button = gtk_button_new_with_label (_("Statistics based selection"));
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (auto_select_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (auto_select_button, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (auto_select_button);
    gtk_box_pack_end (GTK_BOX (hbox), auto_select_button, FALSE, FALSE, 0);
    g_signal_connect (G_OBJECT (auto_select_button), "clicked",
                        G_CALLBACK (create_auto_select_window), appGUI);

    hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox_l), hbox, FALSE, TRUE, 4);

    g_snprintf (buffer, BUFFER_SIZE, "<b>%s:</b>", _("Display as"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
    gtk_misc_set_padding (GTK_MISC (label), 4, 0);

#if GTK_CHECK_VERSION(2,24,0)
    combobox_dtf = gtk_combo_box_text_new ();
#else
    combobox_dtf = gtk_combo_box_new_text ();
#endif

#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (combobox_dtf, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (combobox_dtf, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (combobox_dtf);
    gtk_box_pack_start (GTK_BOX (hbox), combobox_dtf, FALSE, FALSE, 0);
    g_signal_connect (G_OBJECT (combobox_dtf), "changed",
                      G_CALLBACK (options_combobox_dtf_handler_cb), appGUI);

#if GTK_CHECK_VERSION(2,24,0)
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_dtf), _("Romaji"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_dtf), _("Hiragana"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_dtf), _("Katakana"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_dtf), _("Hiragana + Romaji"));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox_dtf), _("Katakana + Romaji"));
#else
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_dtf), _("Romaji"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_dtf), _("Hiragana"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_dtf), _("Katakana"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_dtf), _("Hiragana + Romaji"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_dtf), _("Katakana + Romaji"));
#endif /*GTK_CHECK_VERSION(2,24,0)*/

    gtk_combo_box_set_active (GTK_COMBO_BOX (combobox_dtf), 0);

    hbuttonbox_s = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox_s);
    gtk_box_pack_end (GTK_BOX (hbox), hbuttonbox_s, FALSE, TRUE, 0);
    gtk_box_set_spacing (GTK_BOX (hbuttonbox_s), 4);
    gtk_button_box_set_layout (GTK_BUTTON_BOX(hbuttonbox_s), GTK_BUTTONBOX_END);

    g_snprintf (buffer, BUFFER_SIZE, "<b>%s:</b>", _("Select"));
    label = gtk_label_new (buffer);
    gtk_widget_show (label);
    gtk_box_pack_end (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
    gtk_misc_set_padding (GTK_MISC (label), 4, 0);

    select_all_button = gtk_button_new_with_label (_("All"));
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (select_all_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (select_all_button, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (select_all_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox_s), select_all_button);
    g_signal_connect (G_OBJECT (select_all_button), "clicked",
                      G_CALLBACK (select_all_action_cb), appGUI);

    select_none_button = gtk_button_new_with_label (_("None"));
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (select_none_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (select_none_button, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (select_none_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox_s), select_none_button);
    g_signal_connect (G_OBJECT (select_none_button), "clicked",
                      G_CALLBACK (select_none_action_cb), appGUI);

    invert_selection_button = gtk_button_new_with_label (_("Invert"));
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (invert_selection_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (invert_selection_button, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (select_none_button);
    gtk_widget_show (invert_selection_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox_s), invert_selection_button);
    g_signal_connect (G_OBJECT (invert_selection_button), "clicked",
                      G_CALLBACK (select_invert_action_cb), appGUI);

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_box_pack_start (GTK_BOX (vbox1), hseparator, FALSE, FALSE, 8);

    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_END);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox, FALSE, FALSE, 0);

    close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    gtk_widget_show (close_button);
    g_signal_connect (G_OBJECT (close_button), "clicked",
                      G_CALLBACK (options_close_button_cb), appGUI);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), close_button);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default (close_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
#endif
#endif
    switch(config.repeat_mode) {
        case REPEAT_ALL:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->raa_radio_button), TRUE);
            break;
        case REPEAT_WRONG:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->rwa_radio_button), TRUE);
            break;
        default:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(appGUI->opt->rna_radio_button), TRUE);
    }

    update_selected_kanas (appGUI);

    if (appGUI->opt->active_tab != -1) {
        gtk_notebook_set_current_page (GTK_NOTEBOOK (appGUI->opt->notebook), appGUI->opt->active_tab);
    }

    gtk_widget_show (appGUI->opt->options_window);


#ifdef MAEMO
    if(gtk_notebook_get_current_page (GTK_NOTEBOOK (appGUI->opt->notebook)))
        create_options_window_menu (appGUI);
#else
    gtk_widget_grab_focus (close_button);
#endif
}

/*--------------------------------------------------------------------*/

