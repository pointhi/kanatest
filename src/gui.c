
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

#include "gui_utils.h"
#include "about.h"
#include "main.h"
#include "test.h"
#include "prefs.h"
#include "stats.h"
#include "chart.h"
#include "i18n.h"
#include "options.h"
#include "graphics.h"
#include "stock_icons.h"
#ifdef MAEMO
#include <hildon/hildon.h>

static void
splash_screen_cb(gpointer data)
{
     gtk_widget_destroy(GTK_WIDGET (data));
}

void
show_splash_screen(void)
{
GtkWidget *window;
GdkPixbuf *pixbuf;
GdkPixmap *pixmap;
gint width;
gint height;

     gtk_window_set_auto_startup_notification (FALSE);

     pixbuf = gdk_pixbuf_new_from_inline (-1, kanatest_logo, FALSE, NULL);

     if (!pixbuf)
             return;

     width = gdk_pixbuf_get_width (pixbuf);
     height = gdk_pixbuf_get_height (pixbuf);

     window = gtk_window_new (GTK_WINDOW_POPUP);

     gtk_widget_set_app_paintable (window, TRUE);
     gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
     gtk_widget_set_size_request (window, width, height);
     gtk_widget_show (window);
     pixmap = gdk_pixmap_new (window->window, width, height, -1);
     gdk_draw_pixbuf (pixmap, window->style->fg_gc[GTK_STATE_NORMAL], pixbuf,
                        0, 0, 0, 0, width, height,
                        GDK_RGB_DITHER_NORMAL, 0, 0);

     gdk_window_set_back_pixmap (window->window, pixmap, FALSE);
     gdk_window_clear (window->window);
     g_object_unref (pixbuf);
     g_object_unref (pixmap);

     while (gtk_events_pending ())
          gtk_main_iteration ();
 
     gtk_timeout_add (3000, splash_screen_cb, (gpointer)window);

     gtk_window_set_auto_startup_notification (TRUE);
}
#endif

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
toggle_reverse_mode_cb (GtkToggleButton *togglebutton, gpointer user_data) {
    config.test_mode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (togglebutton));
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
#ifdef MAEMO
    g_snprintf (buffer, BUFFER_SIZE, "<span font_desc='20' color='%s'><tt>%02d:%02d</tt></span>", 
                config.timer_color, appGUI->time_counter / 60, appGUI->time_counter % 60);
#else    
    g_snprintf (buffer, BUFFER_SIZE, "<span font_desc='16' color='%s'><tt>%02d:%02d</tt></span>", 
                config.timer_color, appGUI->time_counter / 60, appGUI->time_counter % 60);
#endif                
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

gchar buffer[BUFFER_SIZE];

    GUI *appGUI = (GUI *)user_data;

    appGUI->tst->test_state = TRUE;
    g_timeout_add (1000, (GSourceFunc)time_handler, appGUI);

    gtk_widget_show (appGUI->char_label);
#ifdef MAEMO
    gtk_widget_show (appGUI->frame_ro);
    gtk_widget_hide (appGUI->combobox_kana_mode);
    gtk_widget_hide (appGUI->label_ka);
    gtk_widget_hide (appGUI->combobox_lesson);
    gtk_widget_hide (appGUI->label_le);
#else
    gtk_widget_hide (appGUI->logo_area);
#endif
    gui_set_widgets_status (FALSE, appGUI);

    gtk_widget_grab_focus (appGUI->romaji_entry);

    test_init (appGUI);
    test_generate_tables (appGUI->tst->max_entries_in_test, appGUI);
    if (config.test_mode == NORMAL) {
        gui_next_text_question (appGUI->tst->questions_table[appGUI->tst->question_counter], config.kana_mode, appGUI);
    } else {
        gui_display_kana_choices (appGUI->tst->questions_table[appGUI->tst->question_counter], config.kana_mode, appGUI);
    }
    gui_set_progress (appGUI);

    appGUI->time_counter = 0;
#ifdef MAEMO
    g_snprintf (buffer, BUFFER_SIZE, "<span font_desc='20' color='%s'><tt>00:00</tt></span>", config.timer_color);
    gtk_label_set_markup (GTK_LABEL (appGUI->timer_label), buffer);
#else    
    g_snprintf (buffer, BUFFER_SIZE, "<span font_desc='16' color='%s'><tt>00:00</tt></span>", config.timer_color);
    gtk_label_set_markup (GTK_LABEL (appGUI->timer_label), buffer);
#endif    
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
#ifdef MAEMO
    gtk_widget_hide (appGUI->frame_ro);
    gtk_widget_hide (appGUI->timer_label);
    gtk_widget_show (appGUI->combobox_kana_mode);
    gtk_widget_show (appGUI->label_ka);
    gtk_widget_show (appGUI->combobox_lesson);
    gtk_widget_show (appGUI->label_le);
#else
    gtk_widget_show (appGUI->logo_area);
#endif
    gtk_widget_hide (appGUI->char_label);
    gtk_widget_hide (appGUI->kana_choices_area);

    gui_set_widgets_status (TRUE, appGUI);

    gtk_widget_show (GTK_WIDGET(appGUI->progressbar));
    gtk_widget_show (GTK_WIDGET(appGUI->hseparator_up));
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (appGUI->progressbar), 0.0);
    gtk_entry_set_text (GTK_ENTRY(appGUI->romaji_entry), "");

    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (appGUI->progressbar),
                                _("Select test mode, kana set and press START"));
#if GTK_CHECK_VERSION(2,90,3)
    gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (appGUI->progressbar), TRUE);
#endif
}

/*--------------------------------------------------------------------*/

void
gui_set_widgets_status (gboolean mode, GUI *appGUI) {

    gtk_widget_set_sensitive (appGUI->romaji_entry, !mode);
    gtk_widget_set_sensitive (appGUI->frame_ro, !mode);
    
    if (mode) {
        gtk_widget_hide (appGUI->frame_ro);
        gtk_widget_show (appGUI->start_button);
        gtk_widget_show (appGUI->quit_button);
        gtk_widget_hide (appGUI->stop_button);

        gtk_widget_show (appGUI->reverse_button);
        gtk_widget_show (appGUI->vseparator_r);
        gtk_widget_show (appGUI->stat_button);
        gtk_widget_show (appGUI->about_button);
        gtk_widget_show (appGUI->prefs_button);
        gtk_widget_show (appGUI->chart_button);

        gtk_widget_hide (appGUI->timer_label);

    } else {
        gtk_widget_show (appGUI->frame_ro);
        gtk_widget_hide (appGUI->start_button);
        gtk_widget_hide (appGUI->quit_button);
        gtk_widget_show (appGUI->stop_button);

        gtk_widget_hide (appGUI->reverse_button);
        gtk_widget_hide (appGUI->vseparator_r);
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
/* number is the character number */

void 
gui_show_correct_answer(gint number, GUI *appGUI) {

gchar tmp_a[BUFFER_SIZE], tmp_b[BUFFER_SIZE];

    appGUI->tst->any_key = FALSE;
    gtk_widget_set_sensitive(GTK_WIDGET(appGUI->stop_button), FALSE);

    if (config.test_mode != NORMAL) {
        gtk_widget_show (appGUI->info_label);
    }

    gui_display_kana (number, ROMAJI, appGUI);
    while (g_main_context_iteration (NULL, FALSE));

    if (config.ca_timeout != TO_ANYKEY) {

        g_usleep (config.ca_timeout * 1000000);

    } else {    /* waiting for any key */

        g_strlcpy (tmp_a, gtk_entry_get_text (GTK_ENTRY(appGUI->romaji_entry)), BUFFER_SIZE);
#ifdef MAEMO
        g_snprintf (tmp_b, BUFFER_SIZE, "%s <-", tmp_a);
#else                
        g_snprintf (tmp_b, BUFFER_SIZE, "%s (Press any key)", tmp_a);
#endif                                
        gtk_entry_set_max_length (GTK_ENTRY(appGUI->romaji_entry), 32);

        while (appGUI->tst->any_key != TRUE && appGUI->tst->test_state != FALSE) {
            gtk_entry_set_text (GTK_ENTRY(appGUI->romaji_entry), tmp_b);
            while (g_main_context_iteration (NULL, FALSE));
            g_usleep (250000);
            if (appGUI->tst->any_key) {
                break;
            }
            gtk_entry_set_text (GTK_ENTRY(appGUI->romaji_entry), tmp_a);
            while (g_main_context_iteration (NULL, FALSE));
            g_usleep (200000);
        }

        gtk_entry_set_max_length (GTK_ENTRY(appGUI->romaji_entry), 5);
        gtk_entry_set_text (GTK_ENTRY(appGUI->romaji_entry), tmp_a);
        while (g_main_context_iteration (NULL, FALSE));
    }

    gtk_widget_set_sensitive(GTK_WIDGET(appGUI->stop_button), TRUE);

    if (config.test_mode != NORMAL) {
        gtk_widget_hide (appGUI->info_label);
    }
}

/*--------------------------------------------------------------------*/

void 
gui_set_button_kana (gint button_number, gint kana_number, gint mode, GUI *appGUI) {
    gchar *character;
    GtkWidget *button;
    gint *number_ptr;
    GtkWidget *label;
    gchar tmpbuf[BUFFER_SIZE];
    
    switch (mode) {
        case HIRAGANA:
            if (kana_number >= MIXED_SEPARATOR) {
                kana_number -= MIXED_SEPARATOR;
            }
            character = get_kana_sign(kana_number, HIRAGANA, TRUE);
            appGUI->old_kana_type = HIRAGANA;
            break;
        case KATAKANA:
            if (kana_number >= MIXED_SEPARATOR) {
                kana_number -= MIXED_SEPARATOR;
            }
            character = get_kana_sign(kana_number, KATAKANA, TRUE);
            appGUI->old_kana_type = KATAKANA;
            break;
        case MIXED:
            if (kana_number >= MIXED_SEPARATOR) {
                character = get_kana_sign((kana_number-MIXED_SEPARATOR), KATAKANA, TRUE);
                appGUI->old_kana_type = KATAKANA;
            } else {
                character = get_kana_sign(kana_number, HIRAGANA, TRUE);
                appGUI->old_kana_type = HIRAGANA;
            }
            break;
        default:
            g_warning ("Invalid mode selected.");
            break;
    }

    button = appGUI->kana_choices[button_number];
    number_ptr = (gint*)g_malloc(sizeof(gint));
    *number_ptr = kana_number;
    
    g_object_set_data_full(G_OBJECT(button), "kana_number", number_ptr, (GDestroyNotify)g_free);
    
    switch (mode) {
        case HIRAGANA:
            if (kana_number >= MIXED_SEPARATOR) {
                kana_number -= MIXED_SEPARATOR;
            }
            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='25' face='%s' color='%s'>%s</span>",
                        config.kana_font_face,
                        config.kana_color,
                        get_kana_sign(kana_number, HIRAGANA, TRUE));
            appGUI->old_kana_type = HIRAGANA;
            break;
        case KATAKANA:
            if (kana_number >= MIXED_SEPARATOR) {
                kana_number -= MIXED_SEPARATOR;
            }
            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='25' face='%s' color='%s'>%s</span>",
                        config.kana_font_face,
                        config.kana_color,
                        get_kana_sign(kana_number, KATAKANA, TRUE));
            appGUI->old_kana_type = KATAKANA;
            break;
       case MIXED:
            if (kana_number >= MIXED_SEPARATOR) {
                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='25' face='%s' color='%s'>%s</span>",
                                                 config.kana_font_face,
                                                 config.kana_color,
                                                 get_kana_sign((kana_number-MIXED_SEPARATOR), KATAKANA, TRUE));
                appGUI->old_kana_type = KATAKANA;
            } else {
                g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='25' face='%s' color='%s'>%s</span>",
                                                 config.kana_font_face,
                                                 config.kana_color,
                                                 get_kana_sign(kana_number, HIRAGANA, TRUE));
                appGUI->old_kana_type = HIRAGANA;
            }
            break;
        default:
            g_warning ("Invalid mode selected.");
            break;
    }
    
    gtk_button_set_label(GTK_BUTTON(button), character);
    label = gtk_bin_get_child(GTK_BIN(button));
    gtk_label_set_markup (GTK_LABEL (label), tmpbuf);
    
    gtk_widget_show(appGUI->kana_choices[button_number]);
}

/*--------------------------------------------------------------------*/

void 
gui_disable_buttons(gint first_button, GUI *appGUI) {
    gint i;
    for (i = first_button; i < KANA_MAX_CHOICES; i++) {
        gtk_widget_hide(appGUI->kana_choices[i]);
    }
}


/*--------------------------------------------------------------------*/
/* number is the global character number */
void
gui_display_kana (gint number, gint mode, GUI *appGUI) {

gchar tmpbuf[BUFFER_SIZE], letbuf[BUFFER_SIZE];
gint original_kana_set;

    if (number >= MIXED_SEPARATOR) {
        number -= MIXED_SEPARATOR;
        original_kana_set = KATAKANA;
    } else {
        original_kana_set = HIRAGANA;
    }

    switch (mode) {
        case HIRAGANA:
            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                        config.kana_font_face,
                        config.kana_color,
                        get_kana_sign(number, HIRAGANA, TRUE));
            appGUI->old_kana_type = HIRAGANA;
            break;
        case KATAKANA:
            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                        config.kana_font_face,
                        config.kana_color,
                        get_kana_sign(number, KATAKANA, TRUE));
            appGUI->old_kana_type = KATAKANA;
            break;
        case ROMAJI:
            g_snprintf (letbuf, BUFFER_SIZE, "%s", get_kana_sign(number, ROMAJI, TRUE));

            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='60' face='%s' color='%s'>%s </span>"
                                             "<span font_desc='40' face='%s' color='%s'>(%s)</span>",
                                             config.kana_font_face,
                                             config.kana_color,
                                             get_kana_sign(number, original_kana_set, TRUE),
                                             config.kana_font_face,
                                             config.romaji_color,
                                             letbuf);
            break;
        case MIXED:
            g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='80' face='%s' color='%s'>%s</span>",
                                             config.kana_font_face,
                                             config.kana_color,
                                             get_kana_sign(number, original_kana_set, TRUE));
            appGUI->old_kana_type = HIRAGANA;
            break;
        default:
            g_warning ("Invalid mode selected.");
            break;
    }
    gtk_widget_hide (appGUI->kana_choices_area);
    gtk_label_set_markup (GTK_LABEL (appGUI->char_label), tmpbuf);
}

/*--------------------------------------------------------------------*/

void
gui_display_kana_choices (gint number, gint mode, GUI *appGUI) {
    gchar tmpbuf[BUFFER_SIZE];
    gchar set1[] = { "hiragana" }, set2[] = { "katakana" };
    gchar *kana_str = NULL;

    if (mode != MIXED) {
        kana_str = (mode == HIRAGANA) ? set1 : set2;
    } else {
        kana_str = (number >= MIXED_SEPARATOR) ? set2 : set1;
    }

    if (number >= MIXED_SEPARATOR) {
        g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='60' face='%s' color='%s'>%s"
                                         "\n<span font_desc='15'>(%s)</span></span>",
                                         config.kana_font_face,
                                         config.kana_color,
                                         get_kana_sign(number - MIXED_SEPARATOR, ROMAJI, TRUE), kana_str);
    } else {
        g_snprintf (tmpbuf, BUFFER_SIZE, "<span font_desc='60' face='%s' color='%s'>%s"
                                         "\n<span font_desc='15'>(%s)</span></span>",
                                         config.kana_font_face,
                                         config.kana_color,
                                         get_kana_sign(number, ROMAJI, TRUE), kana_str);
    }
    
    test_generate_choices(number, appGUI);
    
    gtk_widget_hide(GTK_WIDGET(appGUI->frame_ro));
    gtk_widget_show(appGUI->kana_choices_area);
    gtk_label_set_markup (GTK_LABEL (appGUI->char_label), tmpbuf);
}


/*--------------------------------------------------------------------*/

void
gui_next_text_question (gint number, gint mode, GUI *appGUI) {
    gui_display_kana (number, mode, appGUI);
    gtk_widget_show (GTK_WIDGET (appGUI->frame_ro) );
    gtk_widget_hide(appGUI->kana_choices_area);   
}

/*--------------------------------------------------------------------*/

void
gui_close_window_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;
#ifdef MAEMO    
     (void)appGUI; /* just to remove compile warning */
#else
    gtk_window_get_size (GTK_WINDOW(appGUI->main_window),
                        &config.window_size_x, &config.window_size_y);
    gdk_window_get_root_origin (gtk_widget_get_window(appGUI->main_window),
                                &config.window_x, &config.window_y);
#endif
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
#ifdef MAEMO
          case GDK_KP_Enter:
#endif
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
#ifdef MAEMO
               case GDK_KP_Enter:
#endif
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

gint
gui_mw_button_press_cb (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    if ((appGUI->tst->test_state == TRUE) && (appGUI->tst->any_key == FALSE)) {
        appGUI->tst->any_key = TRUE;
        return TRUE;
    }

    return FALSE;
}
/*--------------------------------------------------------------------*/

void gui_kana_choice_cb(GtkWidget *widget, gpointer user_data) {
    GUI *appGUI = (GUI *)user_data;
    gint *number = (gint*)g_object_get_data(G_OBJECT(widget), "kana_number");

    if (number) {
        test_check_choice(*number, appGUI);
    }
}
/*--------------------------------------------------------------------*/

void
gui_combobox_kana_handler_cb (GtkComboBox *widget, gpointer user_data) {
#ifdef MAEMO
gint index;
HildonTouchSelector *selector;

     selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (widget));
     index = hildon_touch_selector_get_active (HILDON_TOUCH_SELECTOR(selector), 0);
     config.kana_mode = HIRAGANA + index;
#else
    config.kana_mode = HIRAGANA + gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
#endif
}

/*--------------------------------------------------------------------*/

void
gui_combobox_kana_set_handler_cb (GtkComboBox *widget, gpointer user_data) {
#ifdef MAEMO
gint index;
HildonTouchSelector *selector;

     selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (widget));
     index = hildon_touch_selector_get_active (HILDON_TOUCH_SELECTOR(selector), 0);
     config.kana_set = index;
#else
    config.kana_set = gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
#endif
}

/*--------------------------------------------------------------------*/

void
gui_create_window (GUI *appGUI) {

GtkWidget       *vbox0, *vbox1;
GtkWidget       *hseparator_down;
GtkWidget       *frame, *frame1, *frame2;
GtkWidget       *hbox1, *hbox1a;
GtkWidget       *hbox2;
GtkWidget       *empty_hbox;
GtkWidget       *hbuttonbox;
GtkWidget       *label;
GtkWidget       *alignment;
gint            i;
gchar           buffer[BUFFER_SIZE];
GtkWidget       *button;
#ifdef MAEMO
HildonGtkInputMode input_mode;

     kanatest_register_stock_icons ();
     appGUI->main_window = hildon_stackable_window_new ();

     sprintf (buffer, "Kanatest-%s%s", VERSION, MAEMO_VERSION);
     gtk_window_set_title (GTK_WINDOW (appGUI->main_window), buffer);

     g_signal_connect (G_OBJECT (appGUI->main_window), "destroy",
                         G_CALLBACK (gtk_main_quit), NULL);
#else
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

#if GTK_CHECK_VERSION(2,24,0)
    gtk_widget_add_events (GTK_WIDGET (appGUI->main_window),
                                 GDK_BUTTON_PRESS_MASK);
#else
    gtk_widget_add_events (GTK_WINDOW (appGUI->main_window),
                                 GDK_BUTTON_PRESS_MASK);
#endif /*GTK_CHECK_VERSION(2,24,0) */

#endif
    g_signal_connect (G_OBJECT (appGUI->main_window), "delete_event",
                        G_CALLBACK(gui_delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->main_window), "key_press_event",
                        G_CALLBACK(gui_mw_key_press_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->main_window), "button-press-event",
                        G_CALLBACK(gui_mw_button_press_cb), appGUI);

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

#ifndef MAEMO
    appGUI->logo_area = gtk_image_new();
    appGUI->logo = gdk_pixbuf_new_from_inline (-1, kanatest_logo, FALSE, NULL);
    gtk_widget_show(appGUI->logo_area);
    gtk_widget_set_size_request (appGUI->logo_area, -1, 255);  /* logo height */
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->logo_area, TRUE, FALSE, 0);
    gtk_image_set_from_pixbuf (GTK_IMAGE (appGUI->logo_area), appGUI->logo);

    appGUI->char_label = gtk_label_new (NULL);
    gtk_widget_set_size_request (appGUI->char_label, -1, 255);  /* logo height */
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->char_label, TRUE, FALSE, 0);
#else
    appGUI->char_label = gtk_label_new (NULL);
    gtk_widget_set_size_request (appGUI->char_label, -1, 150);
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->char_label, FALSE, FALSE, 0);
#endif

    appGUI->info_label = gtk_label_new (_("Press any key or mouse button to continue..."));
    gtk_box_pack_start(GTK_BOX(vbox1), appGUI->info_label, TRUE, TRUE, 0);

    appGUI->kana_choices_area = gtk_vbox_new(TRUE, 0);
    
    hbuttonbox = gtk_hbox_new(TRUE, 0);
    gtk_widget_show(GTK_WIDGET(hbuttonbox));
    gtk_box_pack_start(GTK_BOX(appGUI->kana_choices_area), hbuttonbox, TRUE, TRUE, 0);

    for (i = 0; i < KANA_MAX_CHOICES; i++) {
        button = gtk_button_new();
#if GTK_CHECK_VERSION(2,17,5)
        gtk_widget_set_can_focus (button, FALSE);
#else
        GTK_WIDGET_UNSET_FLAGS (button, GTK_CAN_FOCUS);
#endif 
        gtk_widget_show(GTK_WIDGET(button));
        appGUI->kana_choices[i] = button;
        g_signal_connect(G_OBJECT(button), "clicked",
                         G_CALLBACK(gui_kana_choice_cb), appGUI);
        gtk_box_pack_start(GTK_BOX(hbuttonbox), button, TRUE, TRUE, 0);
    }

    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->kana_choices_area, FALSE, FALSE, 0);

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
#ifdef MAEMO
     appGUI->romaji_entry = hildon_entry_new (HILDON_SIZE_FINGER_HEIGHT);
     input_mode = hildon_gtk_entry_get_input_mode (GTK_ENTRY (appGUI->romaji_entry));

     /* unset the autocap and dictionary input flags from the HildonEntry */   
     input_mode =  input_mode & ~(HILDON_GTK_INPUT_MODE_AUTOCAP | HILDON_GTK_INPUT_MODE_DICTIONARY);

     hildon_gtk_entry_set_input_mode (GTK_ENTRY (appGUI->romaji_entry), input_mode);
#else
    appGUI->romaji_entry = gtk_entry_new();
#endif
    gtk_entry_set_max_length (GTK_ENTRY(appGUI->romaji_entry), 5);
    g_signal_connect (G_OBJECT(appGUI->romaji_entry), "key_press_event",
                        G_CALLBACK(gui_rm_key_press_cb), appGUI);
    gtk_widget_set_size_request (appGUI->romaji_entry, 140, -1);
    gtk_widget_show (appGUI->romaji_entry);
    gtk_container_add (GTK_CONTAINER (alignment), appGUI->romaji_entry);

    empty_hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (empty_hbox);
    gtk_box_pack_start (GTK_BOX (hbox1), empty_hbox, TRUE, TRUE, 0);

    hbox1a = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1a);
    gtk_box_pack_end (GTK_BOX (hbox1), hbox1a, FALSE, FALSE, 12);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_show (frame2);
    gtk_box_pack_start (GTK_BOX (hbox1a), frame2, FALSE, FALSE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame2), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

#ifndef MAEMO
    label = gtk_label_new (NULL);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME(frame2), label);
#endif

    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox2);
    gtk_container_add (GTK_CONTAINER (alignment), hbox2);

    appGUI->timer_label = gtk_label_new (NULL);
    gtk_widget_set_size_request (appGUI->timer_label, -1, 34);  /* icon height */
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->timer_label, FALSE, FALSE, 0);

    /* Create interface buttons, but not put them inside layouts yet. */

    appGUI->reverse_button = gui_stock_label_togglebutton(NULL, GTK_STOCK_REFRESH);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->reverse_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->reverse_button, GTK_CAN_FOCUS);
#endif
    gtk_widget_set_tooltip_text (appGUI->reverse_button, _("Reverse mode"));
    g_signal_connect (G_OBJECT (appGUI->reverse_button), "toggled",
                        G_CALLBACK (toggle_reverse_mode_cb), appGUI);
    gtk_widget_show (appGUI->reverse_button);

    appGUI->stat_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_STATISTICS);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->stat_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->stat_button, GTK_CAN_FOCUS);
#endif 
    gtk_widget_set_tooltip_text (appGUI->stat_button, _("Statistics"));
    g_signal_connect (G_OBJECT (appGUI->stat_button), "clicked",
                        G_CALLBACK (show_statistics_window_cb), appGUI);
    gtk_widget_show (appGUI->stat_button);


#ifndef MAEMO
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->reverse_button, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->reverse_button), 2);

    appGUI->vseparator_r = gtk_vseparator_new ();
    gtk_widget_show (appGUI->vseparator_r);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->vseparator_r, FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->stat_button, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->stat_button), 2);

    appGUI->chart_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_CHART);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->chart_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->chart_button, GTK_CAN_FOCUS);
#endif 
    g_signal_connect (G_OBJECT (appGUI->chart_button), "clicked",
                        G_CALLBACK (show_chart_window_cb), appGUI);
    gtk_widget_show (appGUI->chart_button);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->chart_button, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->chart_button), 2);
    gtk_widget_set_tooltip_text (appGUI->chart_button, _("Kana chart"));

    appGUI->prefs_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_OPTIONS);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->prefs_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->prefs_button, GTK_CAN_FOCUS);
#endif 
    g_signal_connect (G_OBJECT (appGUI->prefs_button), "clicked",
                        G_CALLBACK (show_options_window_cb), appGUI);
    gtk_widget_show (appGUI->prefs_button);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->prefs_button, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->prefs_button), 2);
    gtk_widget_set_tooltip_text (appGUI->prefs_button, _("Options"));

    appGUI->about_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_ABOUT);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->about_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->about_button, GTK_CAN_FOCUS);
#endif 
    g_signal_connect (G_OBJECT (appGUI->about_button), "clicked",
                        G_CALLBACK (show_about_window_cb), appGUI);
    gtk_widget_show (appGUI->about_button);
    gtk_box_pack_start (GTK_BOX (hbox2), appGUI->about_button, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->about_button), 2);
    gtk_widget_set_tooltip_text (appGUI->about_button, _("About"));
#endif
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
#ifndef MAEMO

#if GTK_CHECK_VERSION(2,24,0)
    appGUI->combobox_kana_mode = gtk_combo_box_text_new ();
#else
     appGUI->combobox_kana_mode = gtk_combo_box_new_text ();
#endif

    gtk_widget_show (appGUI->combobox_kana_mode);
    g_signal_connect (G_OBJECT (appGUI->combobox_kana_mode), "changed",
                      G_CALLBACK (gui_combobox_kana_handler_cb), NULL);
    gtk_container_add (GTK_CONTAINER (alignment), appGUI->combobox_kana_mode);
    gtk_container_set_border_width (GTK_CONTAINER (alignment), 4);

    for(i=0; i < KANA_MODE_NAMES; i++) {
#if GTK_CHECK_VERSION(2,24,0)
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(appGUI->combobox_kana_mode), get_test_mode_name(i+1));
#else
        gtk_combo_box_append_text (GTK_COMBO_BOX (appGUI->combobox_kana_mode), get_test_mode_name(i+1));
#endif
    }

    appGUI->label_ka = gtk_label_new (NULL);
    gtk_widget_show (appGUI->label_ka);
    gtk_frame_set_label_widget (GTK_FRAME(frame), appGUI->label_ka);
    g_snprintf (buffer, BUFFER_SIZE, "<b>%s: </b>", _("Test mode"));
    gtk_label_set_markup (GTK_LABEL (appGUI->label_ka), buffer);
#else
     appGUI->combobox_kana_mode = hildon_picker_button_new (HILDON_SIZE_AUTO, HILDON_BUTTON_ARRANGEMENT_VERTICAL);
     hildon_button_set_title (HILDON_BUTTON (appGUI->combobox_kana_mode), _("Test mode"));
     appGUI->label_ka = hildon_touch_selector_new_text ();

     for(i=0; i < KANA_MODE_NAMES; i++) {
         hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (appGUI->label_ka), get_test_mode_name(i+1));
     }

     hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (appGUI->combobox_kana_mode),
                                             HILDON_TOUCH_SELECTOR (appGUI->label_ka));

     hildon_picker_button_set_active (HILDON_PICKER_BUTTON(appGUI->combobox_kana_mode), 0);

     gtk_widget_show (appGUI->combobox_kana_mode);
     gtk_container_add (GTK_CONTAINER (alignment), appGUI->combobox_kana_mode);
     gtk_container_set_border_width (GTK_CONTAINER (alignment), 4);

     g_signal_connect (G_OBJECT (appGUI->combobox_kana_mode), "value-changed",
                         G_CALLBACK (gui_combobox_kana_handler_cb), NULL);
     hildon_touch_selector_set_active (HILDON_TOUCH_SELECTOR (appGUI->label_ka), 0, config.kana_mode - HIRAGANA);
#endif

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_end (GTK_BOX (hbox1), frame, FALSE, FALSE, 8);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
    
#ifndef MAEMO

#if GTK_CHECK_VERSION(2,24,0)
    appGUI->combobox_lesson = gtk_combo_box_text_new ();
#else
    appGUI->combobox_lesson = gtk_combo_box_new_text ();
#endif

    gtk_widget_show (appGUI->combobox_lesson);
    g_signal_connect (G_OBJECT (appGUI->combobox_lesson), "changed",
                      G_CALLBACK (gui_combobox_kana_set_handler_cb), NULL);
    gtk_container_add (GTK_CONTAINER (alignment), appGUI->combobox_lesson);
    gtk_container_set_border_width (GTK_CONTAINER (alignment), 4);

    for(i=0; i < KANA_SET_NAMES; i++) {
#if GTK_CHECK_VERSION(2,24,0)
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (appGUI->combobox_lesson), get_kana_set_name(i));
#else
        gtk_combo_box_append_text (GTK_COMBO_BOX (appGUI->combobox_lesson), get_kana_set_name(i));
#endif
    }

    appGUI->label_le = gtk_label_new (NULL);
    gtk_widget_show (appGUI->label_le);
    gtk_frame_set_label_widget (GTK_FRAME(frame), appGUI->label_le);
    g_snprintf (buffer, BUFFER_SIZE, "<b>%s: </b>", _("Kana set"));
    gtk_label_set_markup (GTK_LABEL (appGUI->label_le), buffer);
#else
     appGUI->combobox_lesson = hildon_picker_button_new (HILDON_SIZE_AUTO,
                                                                      HILDON_BUTTON_ARRANGEMENT_VERTICAL);
     hildon_button_set_title (HILDON_BUTTON (appGUI->combobox_lesson), _("Kana set"));
     appGUI->label_le = hildon_touch_selector_new_text ();

     for(i=0; i < KANA_SET_NAMES; i++) {
          hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (appGUI->label_le), get_kana_set_name(i));
     }

     hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (appGUI->combobox_lesson),
                                             HILDON_TOUCH_SELECTOR (appGUI->label_le));

     hildon_picker_button_set_active (HILDON_PICKER_BUTTON(appGUI->combobox_lesson), 0);

     gtk_widget_show (appGUI->combobox_lesson);
     gtk_container_add (GTK_CONTAINER (alignment), appGUI->combobox_lesson);
     gtk_container_set_border_width (GTK_CONTAINER (alignment), 4);

     g_signal_connect (G_OBJECT (appGUI->combobox_lesson), "value-changed",
                         G_CALLBACK (gui_combobox_kana_set_handler_cb), NULL);

     hildon_touch_selector_set_active (HILDON_TOUCH_SELECTOR (appGUI->label_le), 0, config.kana_set);
#endif

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_end (GTK_BOX (hbox1), frame, FALSE, FALSE, 8);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox0), hbuttonbox, FALSE, TRUE, 0);
    gtk_box_set_spacing (GTK_BOX (hbuttonbox), 5);

    appGUI->start_button = gui_stock_label_button(_("Start"), GTK_STOCK_EXECUTE);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->start_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->start_button, GTK_CAN_FOCUS);
#endif
    g_signal_connect (G_OBJECT (appGUI->start_button), "clicked",
                        G_CALLBACK (start_test_cb), appGUI);
    gtk_widget_show (appGUI->start_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->start_button);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->start_button), 4);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default (appGUI->start_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (appGUI->start_button, GTK_CAN_DEFAULT);
#endif
    gtk_widget_set_tooltip_text (appGUI->start_button, _("Press to begin testing..."));
#ifdef MAEMO
     gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->reverse_button);
     gtk_container_set_border_width (GTK_CONTAINER (appGUI->reverse_button), 4);
     GTK_WIDGET_SET_FLAGS (appGUI->reverse_button, GTK_CAN_DEFAULT);
    
     gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->stat_button);
     gtk_container_set_border_width (GTK_CONTAINER (appGUI->stat_button), 4);
     GTK_WIDGET_SET_FLAGS (appGUI->stat_button, GTK_CAN_DEFAULT);

     appGUI->chart_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_CHART);
     GTK_WIDGET_UNSET_FLAGS (appGUI->chart_button, GTK_CAN_FOCUS);
     g_signal_connect (G_OBJECT (appGUI->chart_button), "clicked",
                         G_CALLBACK (show_chart_window_cb), appGUI);
     gtk_widget_show (appGUI->chart_button);
     gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->chart_button);
     gtk_container_set_border_width (GTK_CONTAINER (appGUI->chart_button), 4);
     GTK_WIDGET_SET_FLAGS (appGUI->chart_button, GTK_CAN_DEFAULT);
     gtk_widget_set_tooltip_text (appGUI->chart_button, _("Kana chart"));

     appGUI->prefs_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_OPTIONS);
     GTK_WIDGET_UNSET_FLAGS (appGUI->prefs_button, GTK_CAN_FOCUS);
     g_signal_connect (G_OBJECT (appGUI->prefs_button), "clicked",
                         G_CALLBACK (show_options_window_cb), appGUI);
     gtk_widget_show (appGUI->prefs_button);
     gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->prefs_button);
     gtk_container_set_border_width (GTK_CONTAINER (appGUI->prefs_button), 4);
     GTK_WIDGET_SET_FLAGS (appGUI->prefs_button, GTK_CAN_DEFAULT);
     gtk_widget_set_tooltip_text (appGUI->prefs_button, _("Options"));

     appGUI->about_button = gui_stock_label_button(NULL, KANATEST_STOCK_BUTTON_ABOUT);
     GTK_WIDGET_UNSET_FLAGS (appGUI->about_button, GTK_CAN_FOCUS);
     g_signal_connect (G_OBJECT (appGUI->about_button), "clicked",
                         G_CALLBACK (show_about_window_cb), appGUI);
     gtk_widget_show (appGUI->about_button);
     gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->about_button);
     gtk_container_set_border_width (GTK_CONTAINER (appGUI->about_button), 4);
     GTK_WIDGET_SET_FLAGS (appGUI->about_button, GTK_CAN_DEFAULT);

     gtk_widget_set_tooltip_text (appGUI->about_button, _("About"));
#endif
    appGUI->stop_button = gui_stock_label_button(_("Stop"), GTK_STOCK_STOP);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->stop_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->stop_button, GTK_CAN_FOCUS);
#endif
    g_signal_connect (G_OBJECT (appGUI->stop_button), "clicked",
                        G_CALLBACK (stop_test_cb), appGUI);
    gtk_widget_show (appGUI->stop_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->stop_button);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->stop_button), 4);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default (appGUI->stop_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (appGUI->stop_button, GTK_CAN_DEFAULT);
#endif
    gtk_widget_set_tooltip_text (appGUI->stop_button, _("Press to stop testing..."));

    appGUI->quit_button = gui_stock_label_button(_("Quit"), GTK_STOCK_QUIT);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus (appGUI->quit_button, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS (appGUI->quit_button, GTK_CAN_FOCUS);
#endif
    g_signal_connect (G_OBJECT (appGUI->quit_button), "clicked",
                        G_CALLBACK (gui_close_window_cb), appGUI);
    gtk_widget_show (appGUI->quit_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), appGUI->quit_button);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->quit_button), 4);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default (appGUI->quit_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (appGUI->quit_button, GTK_CAN_DEFAULT);
#endif
    gtk_widget_set_tooltip_text (appGUI->quit_button, _("Exit!"));

    gui_disable_test (appGUI);
#ifndef MAEMO
    gtk_combo_box_set_active (GTK_COMBO_BOX (appGUI->combobox_lesson), config.kana_set);
    gtk_combo_box_set_active (GTK_COMBO_BOX (appGUI->combobox_kana_mode), config.kana_mode - HIRAGANA);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (appGUI->reverse_button), config.test_mode);
#endif
    gui_url_initialize(appGUI);
}

/*--------------------------------------------------------------------*/

