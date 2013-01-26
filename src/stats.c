
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
#include "main.h"
#include "test.h"
#include "chart.h"
#include "prefs.h"
#include "stats.h"
#ifdef MAEMO
#include <hildon/hildon.h>

void stats_reset_cb (GtkWidget *widget, gpointer user_data);

void
create_stats_window_menu (GUI *appGUI){

    HildonAppMenu *menu = HILDON_APP_MENU (hildon_app_menu_new ());
    GtkWidget *button = gui_stock_label_button (_("Reset"), GTK_STOCK_CLEAR);
    
    g_signal_connect_after (button, "clicked", G_CALLBACK (stats_reset_cb), appGUI);
    hildon_app_menu_append (menu, GTK_BUTTON (button)); 
   
    hildon_window_set_app_menu (HILDON_WINDOW (appGUI->sts->stat_window), menu);    
    gtk_widget_show_all (GTK_WIDGET (menu));
}
#endif

/*--------------------------------------------------------------------*/

void
stats_window_close_cb (GtkWidget *widget, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    appGUI->sts->active_tab = gtk_notebook_get_current_page (GTK_NOTEBOOK (appGUI->sts->notebook));
    
#ifdef MAEMO
    hildon_window_stack_pop_1 (hildon_window_stack_get_default());
    appGUI->sts->stat_window = NULL;
#else
    gtk_window_get_size (GTK_WINDOW((appGUI->sts)->stat_window),
                        &config.stat_size_x, &config.stat_size_y);
    gdk_window_get_root_origin (gtk_widget_get_window(appGUI->sts->stat_window),
                                &config.stat_window_x, &config.stat_window_y);

    gtk_widget_destroy (appGUI->sts->stat_window);
#endif
}

/*--------------------------------------------------------------------*/

void
stats_delete_event_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    stats_window_close_cb (widget, user_data);
}

/*--------------------------------------------------------------------*/

gint
stats_key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    if (event->keyval == GDK_Escape) {
        stats_window_close_cb(widget, appGUI);
        return TRUE;
    }

    return FALSE;
}

/*--------------------------------------------------------------------*/

void
stats_reset_cb (GtkWidget *widget, gpointer user_data) {

GtkWidget *info_dialog, *label;
gint i, response;
GtkWidget *remove_lessons_check_button;
GtkWidget *remove_hiragana_stats_check_button;
GtkWidget *remove_katakana_stats_check_button;

    GUI *appGUI = (GUI *)user_data;

    info_dialog = gtk_message_dialog_new (GTK_WINDOW(appGUI->sts->stat_window),
                                          GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, 
                                          "\n%s:", _("Please select options"));

    gtk_window_set_title(GTK_WINDOW(info_dialog), _("Question"));
    gtk_widget_show (info_dialog);

    if (stats_get_records(appGUI) != 0) {
#ifdef MAEMO
        remove_lessons_check_button = hildon_check_button_new (HILDON_SIZE_FINGER_HEIGHT);
        gtk_button_set_label (GTK_BUTTON (remove_lessons_check_button), _("Remove lessons list"));
        gtk_widget_show (remove_lessons_check_button);
        gtk_box_pack_start (GTK_BOX(GTK_DIALOG(info_dialog)->vbox), remove_lessons_check_button, FALSE, TRUE, 2);
        hildon_check_button_set_active(HILDON_CHECK_BUTTON (remove_lessons_check_button ), TRUE);       
#else
        remove_lessons_check_button = gtk_check_button_new_with_mnemonic (_("Remove lessons list"));
        gtk_widget_show (remove_lessons_check_button);
        gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(info_dialog))), remove_lessons_check_button, FALSE, TRUE, 2);
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(remove_lessons_check_button ), TRUE);
#endif
    }

#ifdef MAEMO
    remove_hiragana_stats_check_button = hildon_check_button_new (HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label (GTK_BUTTON (remove_hiragana_stats_check_button), _("Remove Hiragana statistics"));
    gtk_widget_show (remove_hiragana_stats_check_button);
    gtk_box_pack_start (GTK_BOX(GTK_DIALOG(info_dialog)->vbox), remove_hiragana_stats_check_button, FALSE, TRUE, 2);
    hildon_check_button_set_active(HILDON_CHECK_BUTTON (remove_hiragana_stats_check_button ), TRUE);
#else
    remove_hiragana_stats_check_button = gtk_check_button_new_with_mnemonic (_("Remove Hiragana statistics"));
    gtk_widget_show (remove_hiragana_stats_check_button);
    gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(info_dialog))), remove_hiragana_stats_check_button, FALSE, TRUE, 2);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(remove_hiragana_stats_check_button ), TRUE);
#endif
    
#ifdef MAEMO
    remove_katakana_stats_check_button = hildon_check_button_new (HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label (GTK_BUTTON (remove_katakana_stats_check_button), _("Remove Katakana statistics"));
    gtk_widget_show (remove_katakana_stats_check_button);
    gtk_box_pack_start (GTK_BOX(GTK_DIALOG(info_dialog)->vbox), remove_katakana_stats_check_button, FALSE, TRUE, 2);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(remove_katakana_stats_check_button ), TRUE);
    hildon_check_button_set_active(HILDON_CHECK_BUTTON (remove_katakana_stats_check_button ), TRUE);
#else
    remove_katakana_stats_check_button = gtk_check_button_new_with_mnemonic (_("Remove Katakana statistics"));
    gtk_widget_show (remove_katakana_stats_check_button);
    gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(info_dialog))), remove_katakana_stats_check_button, FALSE, TRUE, 2);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(remove_katakana_stats_check_button ), TRUE);
#endif

    label = gtk_label_new (_("No further recovery will be possible. Continue ?"));
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(info_dialog))), label, TRUE, TRUE, 6);

    response = gtk_dialog_run(GTK_DIALOG(info_dialog));

    if (response == GTK_RESPONSE_YES) {

#ifdef MAEMO
        if (stats_get_records(appGUI) != 0 && hildon_check_button_get_active( HILDON_CHECK_BUTTON (remove_lessons_check_button )) ==  TRUE) {    
            stats_remove_list (appGUI);
        }

        if (hildon_check_button_get_active (HILDON_CHECK_BUTTON (remove_hiragana_stats_check_button )) ==  TRUE) {    
            for (i=0; i < NUMBER_OF_SIGNS; i++) {
                appGUI->sts->correct_hiragana_counters[i] = 0;
                appGUI->sts->hiragana_counters[i] = 0;
            }
        }

        if (hildon_check_button_get_active (HILDON_CHECK_BUTTON (remove_katakana_stats_check_button )) ==  TRUE) {    
            for (i=0; i < NUMBER_OF_SIGNS; i++) {
                appGUI->sts->correct_katakana_counters[i] = 0;
                appGUI->sts->katakana_counters[i] = 0;
            }
        }
#else
        if (stats_get_records(appGUI) != 0 && gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(remove_lessons_check_button )) ==  TRUE) {
            stats_remove_list (appGUI);
        }

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(remove_hiragana_stats_check_button )) ==  TRUE) {
            for (i=0; i < NUMBER_OF_SIGNS; i++) {
                appGUI->sts->correct_hiragana_counters[i] = 0;
                appGUI->sts->hiragana_counters[i] = 0;
            }
        }

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(remove_katakana_stats_check_button )) ==  TRUE) {
            for (i=0; i < NUMBER_OF_SIGNS; i++) {
                appGUI->sts->correct_katakana_counters[i] = 0;
                appGUI->sts->katakana_counters[i] = 0;
            }
        }
#endif
        gtk_widget_destroy(info_dialog);
        stats_window_close_cb(appGUI->sts->stat_window, appGUI);

    } else {
        gtk_widget_destroy(info_dialog);
    }
}

/*--------------------------------------------------------------------*/

void
stats_column_clicked_cb (GtkTreeViewColumn *treeviewcolumn, gpointer user_data) {

    MESSAGE *msg = (MESSAGE *)user_data;

    msg->appGUI->sts->old_column = config.stats_sort_column;
    config.stats_sort_column = (size_t) msg->data;

    if (msg->appGUI->sts->disable_dir == FALSE) {

        if (msg->appGUI->sts->old_column == config.stats_sort_column) {
            config.stats_sort_column_dir *= -1;
        } else {
            config.stats_sort_column_dir = 1;
        }
    }
}

/*--------------------------------------------------------------------*/

void
draw_statistics_graph (GUI *appGUI) {

gint width, height, i, k;
cairo_t *sts_cr = NULL;
double minY, maxY, hiragana_value, katakana_value;
double h_real_y_pos, k_real_y_pos, region, stepX, stepY;
double dash[] = { 1.0 };
gchar buffer[BUFFER_SIZE];

#if GTK_CHECK_VERSION(2,17,7)
    GtkAllocation dwdh;
    gtk_widget_get_allocation(appGUI->sts->graph_viewport, &dwdh);
    width = dwdh.width - 4;
    height = dwdh.height - 4;
#else
    width = appGUI->sts->graph_viewport->allocation.width - 4;
    height = appGUI->sts->graph_viewport->allocation.height - 4;
#endif

    sts_cr = gdk_cairo_create (gtk_widget_get_window(appGUI->sts->graph_drawing_area));

    cairo_set_source_rgb (sts_cr, 1.0, 1.0, 1.0);   /* white background */
    cairo_paint (sts_cr);

    cairo_set_line_width (sts_cr, 1.0);
    cairo_set_antialias (sts_cr, CAIRO_ANTIALIAS_DEFAULT);

    cairo_select_font_face (sts_cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (sts_cr, LEGEND_FONT_SIZE);

    cairo_set_source_rgb (sts_cr, 0.0, 0.0, 0.0);   /* black */

    /* arrows and scale */

    minY = height-Y_AXIS_MARGIN - Y_BORDER;
    maxY = height / 50.0 + ARROW_L * 2.5;
    region = fabs(minY - maxY);
    stepY = region / 4.0;

    /* scale */
    cairo_move_to (sts_cr, X_BORDER*1.5, minY);
    cairo_show_text (sts_cr, "0%");

    for (i=0; i < 4; i++) {
    
        cairo_set_source_rgb (sts_cr, 0.0, 0.0, 0.0);   /* black */

        g_snprintf (buffer, BUFFER_SIZE, "%d%%", 100/4 * (i+1));
        cairo_move_to (sts_cr, X_BORDER*1.5, minY+LEGEND_FONT_SIZE/2 - stepY*(i+1));
        cairo_show_text (sts_cr, buffer);

        cairo_set_dash (sts_cr, NULL, 0, 0);
        cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER-TICK_WIDTH/2, minY - stepY*(i+1));
        cairo_line_to (sts_cr, X_AXIS_MARGIN+X_BORDER+TICK_WIDTH/2, minY - stepY*(i+1));
        cairo_stroke (sts_cr);

        cairo_set_source_rgb (sts_cr, 0.7, 0.7, 0.7);   /* gray */
        cairo_set_dash (sts_cr, dash, 1, 0);
        cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER, minY - stepY*(i+1));
        cairo_line_to (sts_cr, width-X_AXIS_MARGIN, minY - stepY*(i+1));

        cairo_stroke (sts_cr);
    }

    /* draw graph */

    /* hiragana */

    cairo_set_dash (sts_cr, NULL, 0, 0);

    stepX = (width - X_AXIS_MARGIN) / NUMBER_OF_SIGNS;

    for (i = 0; i < NUMBER_OF_SIGNS; i++) {

        if (appGUI->sts->hiragana_counters[i] > 0) {
            hiragana_value = ((double)appGUI->sts->correct_hiragana_counters[i]/appGUI->sts->hiragana_counters[i])*100.0;
        } else {
            hiragana_value = 0;
        }

        h_real_y_pos = minY - region * hiragana_value / 100.0;

        if (appGUI->sts->katakana_counters[i] > 0) {
            katakana_value = ((double)appGUI->sts->correct_katakana_counters[i]/appGUI->sts->katakana_counters[i])*100.0;
        } else {
            katakana_value = 0;
        }

        k_real_y_pos = minY - region * katakana_value / 100.0;

        cairo_set_source_rgb (sts_cr, 0.9, 0.4, 0.0);
        cairo_rectangle (sts_cr, X_AXIS_MARGIN+X_BORDER+stepX*i, h_real_y_pos, 
                         stepX/2, (region * hiragana_value / 100.0));
        cairo_fill (sts_cr);

        cairo_set_source_rgb (sts_cr, 0.4, 0.0, 0.7);
        cairo_rectangle (sts_cr, X_AXIS_MARGIN+X_BORDER+stepX*i+stepX/2, k_real_y_pos, 
                         stepX/2, (region * katakana_value / 100.0));
        cairo_fill (sts_cr);

    }

    k = 0;

    cairo_set_source_rgb (sts_cr, 0.0, 0.0, 0.0);   /* black */

    for (i = 0; i < NUMBER_OF_SIGNS; i++) {
        cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER+stepX*i, minY+LEGEND_FONT_SIZE*(k+1));
        g_snprintf (buffer, BUFFER_SIZE, "%s", get_kana_sign (i, ROMAJI, FALSE));
        if (buffer[2] == ',') buffer[2] = '\0';  /* special case: di,ji */
        cairo_show_text (sts_cr, buffer);
        k = (k + 1) % 3;
    }

    /* legend */

    cairo_set_source_rgb (sts_cr, 0.9, 0.4, 0.0);
    cairo_rectangle (sts_cr, X_AXIS_MARGIN+X_BORDER+width/1.5, 15, 8*LEGEND_FONT_SIZE, 2*LEGEND_FONT_SIZE);
    cairo_fill (sts_cr);
    cairo_set_source_rgb (sts_cr, 0.4, 0.0, 0.7);
    cairo_rectangle (sts_cr, X_AXIS_MARGIN+X_BORDER+width/1.3, 15, 8*LEGEND_FONT_SIZE, 2*LEGEND_FONT_SIZE);
    cairo_fill (sts_cr);
    cairo_set_source_rgb (sts_cr, 1.0, 1.0, 1.0);   /* white */
    cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER+width/1.5+2, 25+2);
    cairo_show_text (sts_cr, "Hiragana");
    cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER+width/1.3+2, 25+2);
    cairo_show_text (sts_cr, "Katakana");

    /* vertical axis */
    cairo_set_source_rgb (sts_cr, 0.0, 0.0, 0.0);   /* black */
    cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER, height-Y_AXIS_MARGIN-Y_BORDER);
    cairo_line_to (sts_cr, X_AXIS_MARGIN+X_BORDER, Y_AXIS_MARGIN);

    /* arrow */
    cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER, Y_AXIS_MARGIN);
    cairo_line_to (sts_cr, X_AXIS_MARGIN+X_BORDER-ARROW_A, Y_AXIS_MARGIN+ARROW_L);
    cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER, Y_AXIS_MARGIN);
    cairo_line_to (sts_cr, X_AXIS_MARGIN+X_BORDER+ARROW_A, Y_AXIS_MARGIN+ARROW_L);

    /* horizontal axis */
    cairo_move_to (sts_cr, X_AXIS_MARGIN+X_BORDER, height-Y_AXIS_MARGIN-Y_BORDER);
    cairo_line_to (sts_cr, width-X_BORDER, height-Y_AXIS_MARGIN-Y_BORDER);

    /* arrow */
    cairo_move_to (sts_cr, width-X_BORDER, height-Y_AXIS_MARGIN-Y_BORDER);
    cairo_line_to (sts_cr, width-X_BORDER-ARROW_L, height-Y_AXIS_MARGIN-Y_BORDER-ARROW_A);
    cairo_move_to (sts_cr, width-X_BORDER, height-Y_AXIS_MARGIN-Y_BORDER);
    cairo_line_to (sts_cr, width-X_BORDER-ARROW_L, height-Y_AXIS_MARGIN-Y_BORDER+ARROW_A);

    cairo_stroke (sts_cr);

    /* destroy surface */

    cairo_destroy (sts_cr);
}

/*--------------------------------------------------------------------*/

gint
graph_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer user_data) {

    GUI *appGUI = (GUI *)user_data;

    draw_statistics_graph (appGUI);
    return TRUE;
}

#if !GTK_CHECK_VERSION(2,91,0)
gint
graph_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
#else
gint
graph_expose_event (GtkWidget *widget, cairo_t *event, gpointer user_data) {
#endif

    GUI *appGUI = (GUI *)user_data;

    draw_statistics_graph (appGUI);
    return FALSE;
}

/*--------------------------------------------------------------------*/

void
stats_create_window (GUI *appGUI) {

GtkWidget       *vbox1, *vbox2;
GtkWidget       *vbox3, *vbox4;
GtkWidget       *vbox5, *vbox6, *vbox7;
GtkWidget       *hbuttonbox;
#ifndef MAEMO
GtkWidget       *close_button;
GtkWidget       *reset_button;
#endif
GtkWidget       *hseparator;
GtkWidget       *stats_scrolledwindow;
GtkWidget       *stats_treeview;
GtkWidget       *label;
GtkWidget       *table_hiragana;
GtkWidget       *scrolledwindow_hiragana;
GtkWidget       *viewport_hiragana;
GtkWidget       *table_katakana;
GtkWidget       *scrolledwindow_katakana;
GtkWidget       *viewport_katakana;
GtkWidget       *frames_hiragana[CHART_ROWS*CHART_COLUMNS];
GtkWidget       *frames_katakana[CHART_ROWS*CHART_COLUMNS];
GtkListStore    *stats_store;
GtkTreeModel    *stats_model;
GtkCellRenderer *stats_renderer;
GtkTreeIter     stats_iter;
gint            i, j, cpos, pos;
GSList          *lnode;
gchar           buffer[BUFFER_SIZE];
static          MESSAGE msg[NUMBER_OF_COLUMNS];
gdouble         ratio;
gchar           color_str[BUFFER_SIZE];
stats_entry     *entry;

GtkTreeViewColumn   *stats_column[NUMBER_OF_COLUMNS];

gchar *column_names[NUMBER_OF_COLUMNS] = {
    _("Date"), _("Test type"), _("Test mode"), _("Test time"), _("Questions"),
    _("Correct"), _("Wrong"), _("Ratio (%)"), _("Kana set")
};

#ifdef MAEMO
    appGUI->sts->stat_window = hildon_stackable_window_new ();    
    hildon_window_stack_push_1 (hildon_window_stack_get_default (), HILDON_STACKABLE_WINDOW (appGUI->sts->stat_window));
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->sts->stat_window), 12);
    gtk_window_set_title (GTK_WINDOW (appGUI->sts->stat_window), _("Statistics"));

    g_signal_connect (G_OBJECT (appGUI->sts->stat_window), "delete_event",
                        G_CALLBACK(stats_delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->sts->stat_window), "key_press_event",
                        G_CALLBACK(stats_key_press_cb), appGUI);    
#else
    appGUI->sts->stat_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_transient_for(GTK_WINDOW(appGUI->sts->stat_window),GTK_WINDOW(appGUI->main_window));
    gtk_window_set_modal(GTK_WINDOW(appGUI->sts->stat_window), TRUE);
    gtk_container_set_border_width (GTK_CONTAINER (appGUI->sts->stat_window), 12);
    gtk_window_set_title (GTK_WINDOW (appGUI->sts->stat_window), _("Statistics"));

    g_signal_connect (G_OBJECT (appGUI->sts->stat_window), "delete_event",
                        G_CALLBACK(stats_delete_event_cb), appGUI);
    g_signal_connect (G_OBJECT(appGUI->sts->stat_window), "key_press_event",
                        G_CALLBACK(stats_key_press_cb), appGUI);

    gtk_window_move (GTK_WINDOW (appGUI->sts->stat_window),
                                config.stat_window_x, config.stat_window_y);

    gtk_window_set_default_size (GTK_WINDOW(appGUI->sts->stat_window),
                                config.stat_size_x, config.stat_size_y);

    gtk_window_set_resizable (GTK_WINDOW (appGUI->sts->stat_window), TRUE);
#endif

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (appGUI->sts->stat_window), vbox1);

    appGUI->sts->notebook = gtk_notebook_new ();
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_focus(appGUI->sts->notebook, FALSE);
#else
    GTK_WIDGET_UNSET_FLAGS(appGUI->sts->notebook, GTK_CAN_FOCUS);
#endif
    gtk_widget_show (appGUI->sts->notebook);
    gtk_box_pack_start (GTK_BOX (vbox1), appGUI->sts->notebook, TRUE, TRUE, 4);

    /* lessons stats */

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    label = gtk_label_new (_("Lessons"));
    if (stats_get_records(appGUI) != 0) {
        gtk_notebook_append_page(GTK_NOTEBOOK(appGUI->sts->notebook), vbox2, label);
    }

#ifdef MAEMO
    stats_scrolledwindow = hildon_pannable_area_new ();
    g_object_set (G_OBJECT (stats_scrolledwindow),
                   "mov-mode", HILDON_MOVEMENT_MODE_BOTH, NULL);
#else
    stats_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
#endif
    gtk_widget_show (stats_scrolledwindow);
    gtk_box_pack_start (GTK_BOX (vbox2), stats_scrolledwindow, TRUE, TRUE, 4);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (stats_scrolledwindow), GTK_SHADOW_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (stats_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    stats_store = gtk_list_store_new (NUMBER_OF_COLUMNS,
                                      G_TYPE_STRING,
                                      G_TYPE_STRING,
                                      G_TYPE_STRING,
                                      G_TYPE_STRING,
                                      G_TYPE_INT,
                                      G_TYPE_INT,
                                      G_TYPE_INT,
                                      G_TYPE_INT,
                                      G_TYPE_STRING);

    stats_model = GTK_TREE_MODEL(stats_store);

    stats_treeview = gtk_tree_view_new_with_model (stats_model);
#ifdef MAEMO
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (stats_treeview), TRUE);
#endif    
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (stats_treeview), TRUE);
    gtk_tree_view_set_search_column (GTK_TREE_VIEW (stats_treeview), 0);
    gtk_container_add(GTK_CONTAINER (stats_scrolledwindow), stats_treeview);
    gtk_widget_show (stats_treeview);

    stats_renderer = gtk_cell_renderer_text_new ();

    for (i=0; i < NUMBER_OF_COLUMNS; i++) {
        stats_column[i] = gtk_tree_view_column_new_with_attributes (column_names[i],
                                stats_renderer, "text", i, NULL);
        gtk_tree_view_column_set_sort_column_id (stats_column[i], i);
        gtk_tree_view_append_column (GTK_TREE_VIEW(stats_treeview), stats_column[i]);
        msg[i].data = (gpointer)((size_t)i);
        msg[i].appGUI = appGUI;
        g_signal_connect (G_OBJECT(stats_column[i]), "clicked",
                          G_CALLBACK(stats_column_clicked_cb), &msg[i]);
    }

    /* hiragana stats */

    vbox3 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox3);
    label = gtk_label_new (_("Hiragana"));
    gtk_notebook_append_page(GTK_NOTEBOOK(appGUI->sts->notebook), vbox3, label);

#ifdef MAEMO
    scrolledwindow_hiragana = hildon_pannable_area_new ();
    g_object_set (G_OBJECT (scrolledwindow_hiragana),
                   "mov-mode", HILDON_MOVEMENT_MODE_BOTH, NULL);
#else
    scrolledwindow_hiragana = gtk_scrolled_window_new (NULL, NULL);
#endif 
    gtk_widget_show (scrolledwindow_hiragana);
    gtk_box_pack_start (GTK_BOX (vbox3), scrolledwindow_hiragana, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_hiragana), /*GTK_POLICY_NEVER*/GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    viewport_hiragana = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport_hiragana);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_hiragana), viewport_hiragana);

    vbox4 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox4);
    gtk_container_add (GTK_CONTAINER (viewport_hiragana), vbox4);

    table_hiragana = gtk_table_new (CHART_ROWS, CHART_COLUMNS, FALSE);
    gtk_widget_show (table_hiragana);
    gtk_box_pack_start (GTK_BOX (vbox4), table_hiragana, FALSE, TRUE, 0);

    cpos = pos = 0;

    for(j=0; j < CHART_ROWS; j++) {
        for(i=0; i < CHART_COLUMNS; i++) {

            if (get_kana_sign_mask(j, i)) {

                frames_hiragana[pos] = gtk_frame_new (NULL);
                gtk_widget_show (frames_hiragana[pos]);
                gtk_table_attach (GTK_TABLE (table_hiragana), frames_hiragana[pos], i, i+1, j, j+1,
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
                gtk_container_set_border_width (GTK_CONTAINER (frames_hiragana[pos]), 2);
                gtk_frame_set_shadow_type (GTK_FRAME (frames_hiragana[pos]), GTK_SHADOW_ETCHED_OUT);

                appGUI->chr->labels[pos] = gtk_label_new (NULL);
                gtk_widget_show (appGUI->chr->labels[pos]);
                gtk_container_add (GTK_CONTAINER (frames_hiragana[pos]), appGUI->chr->labels[pos]);
                gtk_misc_set_padding (GTK_MISC (appGUI->chr->labels[pos]), 0, 4);

                if (appGUI->sts->hiragana_counters[cpos] > 0) {
                    ratio = ((double)appGUI->sts->correct_hiragana_counters[cpos]/appGUI->sts->hiragana_counters[cpos])*100.0;
                } else {
                    ratio = 0;
                }

                g_strlcpy (color_str, "#b00000", BUFFER_SIZE);     /* red */
                if (ratio > 50 && ratio < 80) {
                    g_strlcpy (color_str, "#0000b0", BUFFER_SIZE);     /* blue */
                } else if (ratio >= 80) {
                    g_strlcpy (color_str, "#008800", BUFFER_SIZE);     /* green */
                }

#ifdef MAEMO
                g_snprintf (buffer, BUFFER_SIZE, "<span font_desc='12' face='%s'>%s/%s</span>: <span  font_desc='12' color='%s'>%d/%d</span><span font_desc='12'> (%.f %%)</span>",
#else
                g_snprintf (buffer, BUFFER_SIZE, "<span face='%s'>%s/%s</span>: <span color='%s'>%d/%d</span> (%.f %%)",
#endif
				config.kana_font_face,
				get_kana_sign(cpos, HIRAGANA, TRUE), get_kana_sign(cpos, ROMAJI, TRUE), color_str, 
				appGUI->sts->correct_hiragana_counters[cpos], appGUI->sts->hiragana_counters[cpos], ratio);

                gtk_label_set_markup (GTK_LABEL (appGUI->chr->labels[pos]), buffer);
                cpos++;
            }

            pos++;
        }
    }

    /* katakana */

    vbox5 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox5);
    label = gtk_label_new (_("Katakana"));
    gtk_notebook_append_page(GTK_NOTEBOOK(appGUI->sts->notebook), vbox5, label);

#ifdef MAEMO
    scrolledwindow_katakana = hildon_pannable_area_new ();
    g_object_set (G_OBJECT (scrolledwindow_katakana),
                   "mov-mode", HILDON_MOVEMENT_MODE_BOTH, NULL);
#else
    scrolledwindow_katakana = gtk_scrolled_window_new (NULL, NULL);
#endif
    gtk_widget_show (scrolledwindow_katakana);
    gtk_box_pack_start (GTK_BOX (vbox5), scrolledwindow_katakana, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_katakana), /*GTK_POLICY_NEVER*/GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    viewport_katakana = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport_katakana);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_katakana), viewport_katakana);

    vbox6 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox6);
    gtk_container_add (GTK_CONTAINER (viewport_katakana), vbox6);

    table_katakana = gtk_table_new (CHART_ROWS, CHART_COLUMNS, FALSE);
    gtk_widget_show (table_katakana);
    gtk_box_pack_start (GTK_BOX (vbox6), table_katakana, FALSE, TRUE, 0);

    cpos = pos = 0;

    for(j=0; j < CHART_ROWS; j++) {
        for(i=0; i < CHART_COLUMNS; i++) {

            if(get_kana_sign_mask(j, i)) {

                frames_katakana[pos] = gtk_frame_new (NULL);
                gtk_widget_show (frames_katakana[pos]);
                gtk_table_attach (GTK_TABLE (table_katakana), frames_katakana[pos], i, i+1, j, j+1,
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
                gtk_container_set_border_width (GTK_CONTAINER (frames_katakana[pos]), 2);
                gtk_frame_set_shadow_type (GTK_FRAME (frames_katakana[pos]), GTK_SHADOW_ETCHED_OUT);


                appGUI->chr->labels[pos] = gtk_label_new (NULL);
                gtk_widget_show (appGUI->chr->labels[pos]);
                gtk_container_add (GTK_CONTAINER (frames_katakana[pos]), appGUI->chr->labels[pos]);
                gtk_misc_set_padding (GTK_MISC (appGUI->chr->labels[pos]), 0, 4);

                if (appGUI->sts->katakana_counters[cpos] > 0) {
                    ratio = ((double)appGUI->sts->correct_katakana_counters[cpos]/appGUI->sts->katakana_counters[cpos])*100.0;
                } else {
                    ratio = 0;
                }

                g_strlcpy (color_str, "#b00000", BUFFER_SIZE);     /* red */
                if (ratio > 50 && ratio < 80) {
                    g_strlcpy (color_str, "#0000b0", BUFFER_SIZE);     /* blue */
                } else if (ratio >= 80) {
                    g_strlcpy (color_str, "#008800", BUFFER_SIZE);     /* green */
                }

#ifdef MAEMO
                g_snprintf (buffer, BUFFER_SIZE, "<span font_desc='12' face='%s'>%s/%s</span>: <span  font_desc='12' color='%s'>%d/%d</span><span font_desc='12'> (%.f %%)</span>",
#else
                g_snprintf (buffer, BUFFER_SIZE, "<span face='%s'>%s/%s</span>: <span color='%s'>%d/%d</span> (%.f %%)",
#endif
				config.kana_font_face,
				get_kana_sign(cpos, KATAKANA, TRUE), get_kana_sign(cpos, ROMAJI, TRUE), color_str, 
				appGUI->sts->correct_katakana_counters[cpos], appGUI->sts->katakana_counters[cpos], ratio);

                gtk_label_set_markup (GTK_LABEL (appGUI->chr->labels[pos]), buffer);
                cpos++;
            }

            pos++;
        }
    }

    /* graph */

    vbox7 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox7);
    label = gtk_label_new (_("Graph"));
    gtk_notebook_append_page(GTK_NOTEBOOK(appGUI->sts->notebook), vbox7, label);

    appGUI->sts->graph_viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (appGUI->sts->graph_viewport);
    gtk_box_pack_start (GTK_BOX (vbox7), appGUI->sts->graph_viewport, TRUE, TRUE, 0);
    
    appGUI->sts->graph_drawing_area = gtk_drawing_area_new();
    gtk_widget_show (appGUI->sts->graph_drawing_area);
    gtk_container_add (GTK_CONTAINER(appGUI->sts->graph_viewport), appGUI->sts->graph_drawing_area);

    g_signal_connect (G_OBJECT(appGUI->sts->graph_drawing_area), "configure_event",
                      G_CALLBACK(graph_configure_event), appGUI);
#if !GTK_CHECK_VERSION(2,91,0)
    g_signal_connect (G_OBJECT(appGUI->sts->graph_drawing_area), "expose_event",
                      G_CALLBACK(graph_expose_event), appGUI);
#else
    g_signal_connect (G_OBJECT(appGUI->sts->graph_drawing_area), "draw",
                      G_CALLBACK(graph_expose_event), appGUI);
#endif

    /* buttons */

    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_EDGE);
    gtk_box_pack_end (GTK_BOX (vbox1), hbuttonbox, FALSE, TRUE, 0);

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_box_pack_end (GTK_BOX (vbox1), hseparator, FALSE, TRUE, 8);
#ifndef MAEMO
    reset_button = gui_stock_label_button(_("Reset"), GTK_STOCK_CLEAR);
    gtk_widget_show (reset_button);
    g_signal_connect (G_OBJECT (reset_button), "clicked",
                        G_CALLBACK (stats_reset_cb), appGUI);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), reset_button);

    close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    gtk_widget_show (close_button);
    g_signal_connect (G_OBJECT (close_button), "clicked",
                        G_CALLBACK (stats_window_close_cb), appGUI);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), close_button);
#if GTK_CHECK_VERSION(2,17,5)
    gtk_widget_set_can_default (close_button, TRUE);
#else
    GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
#endif
#endif
    if (appGUI->sts->active_tab != -1) {
        gtk_notebook_set_current_page (GTK_NOTEBOOK (appGUI->sts->notebook), appGUI->sts->active_tab);
    }

#ifndef MAEMO
   gtk_widget_grab_default (close_button);
#endif
    /*------------------------------------------------------------------------------*/

    if (appGUI->sts->stats_list != NULL) {

        for (i = 0, lnode = appGUI->sts->stats_list; lnode != NULL; lnode = lnode->next, i++) {

            entry = g_slist_nth_data (appGUI->sts->stats_list, i);

            gtk_list_store_append (stats_store, &stats_iter);

            g_snprintf (buffer, BUFFER_SIZE, "%4d.%02d.%02d, %02d:%02d", entry->date_year, entry->date_month+1, 
                        entry->date_day, entry->date_hour, entry->date_minute);
            gtk_list_store_set (stats_store, &stats_iter, 0, buffer, -1);

            switch(entry->test_repeat_mode) {
                case REPEAT_WRONG:
                    g_snprintf (buffer, BUFFER_SIZE, "%s (%s)", get_test_mode_name (entry->test_mode), _("RW"));
                    break;
                case REPEAT_ALL:
                    g_snprintf (buffer, BUFFER_SIZE, "%s (%s)", get_test_mode_name (entry->test_mode), _("RA"));
                    break;
                default:
                    g_snprintf (buffer, BUFFER_SIZE, "%s", get_test_mode_name (entry->test_mode));
                    break;
            }

            gtk_list_store_set (stats_store, &stats_iter, 1, buffer,
                                2, get_mode_name(entry->test_type),
                                3, test_sec2str(entry->test_time, TRUE), 4, entry->test_questions,
                                5, entry->test_correct_answers, 6, entry->test_questions-entry->test_correct_answers,
                                7, (gint)((gfloat)(entry->test_correct_answers)/entry->test_questions*100.0),
                                8, get_kana_set_name(entry->test_kana_set), -1);
        }

    }

    appGUI->sts->disable_dir = TRUE;

    g_signal_emit_by_name (stats_column[config.stats_sort_column], "clicked");

    if (config.stats_sort_column_dir == -1) {
        g_signal_emit_by_name (stats_column[config.stats_sort_column], "clicked");
    }

    appGUI->sts->disable_dir = FALSE;

    gtk_widget_show (appGUI->sts->stat_window);
#ifdef MAEMO
    create_stats_window_menu(appGUI);
#endif
}

/*--------------------------------------------------------------------*/

void
stats_free_list (GUI *appGUI) {

    if (appGUI->sts->stats_list != NULL) {
        g_slist_free(appGUI->sts->stats_list);
        appGUI->sts->stats_list = NULL;
    }
}

/*--------------------------------------------------------------------*/

void
stats_remove_list (GUI *appGUI) {

stats_entry *entry;
gint i;

    if (appGUI->sts->stats_list != NULL) {

        i = stats_get_records(appGUI);

        while (i >= 0) {
            entry = g_slist_nth_data (appGUI->sts->stats_list, i);
            appGUI->sts->stats_list = g_slist_remove (appGUI->sts->stats_list, entry);
            --i;
        }
    }
}

/*--------------------------------------------------------------------*/

gint
stats_get_records (GUI *appGUI) {

    gint i;
    GSList *lnode;

    if (appGUI->sts->stats_list != NULL) {

        for (i = 0, lnode = appGUI->sts->stats_list; lnode != NULL; lnode = lnode->next, i++);
        return i;

    } else {
        return 0;
    }
}

/*--------------------------------------------------------------------*/

void    
stats_add_entry (stats_entry *entry, GUI *appGUI) {

    if (entry != NULL) {
        appGUI->sts->stats_list = g_slist_append (appGUI->sts->stats_list, entry);
    } else {
        g_warning ("Cannot allocate memory.");
    }
}

/*--------------------------------------------------------------------*/

gchar *
get_kana_token (gint token, gint type) {

static gchar temp[BUFFER_SIZE];

    g_snprintf (temp, BUFFER_SIZE, "%s_%d", get_kana_sign (token, ROMAJI, FALSE), type);
    if (temp[2] == ',') temp[2] = '_';  /* special case: di,ji */

    return temp;
}

/*--------------------------------------------------------------------*/

void
stats_read_list (gchar *stats_filename, gchar *config_dirname, GUI *appGUI) {

xmlDocPtr doc;
xmlChar *key;
xmlNodePtr node, cnode, hiragana_node, katakana_node;
gchar *prop;
gint i, count, correct, id;
stats_entry *entry = NULL;

    appGUI->sts->stats_list = NULL;

    if((doc = xmlParseFile (prefs_get_config_filename (stats_filename, config_dirname)))) {

        if(!(node = xmlDocGetRootElement (doc))) {
            return;
        }

        if(!xmlStrcmp (node->name, (xmlChar *) STATS_NAME)) {

            /* read note */
            node = node->xmlChildrenNode;

            while (node != NULL) {

                if(!xmlStrcmp (node->name, (xmlChar *) "test_stats")) {

                    cnode = node->xmlChildrenNode;

                    entry = g_new0 (stats_entry, 1);
                    if (entry == NULL) continue;

                    entry->date_day = entry->date_month = entry->date_year = 0;
                    entry->date_hour = entry->date_minute = entry->test_time = 0;
                    entry->test_type = entry->test_mode = entry->test_kana_set = entry->test_questions = 0;
                    entry->test_correct_answers = entry->test_repeat_mode = 0;

                    while (cnode != NULL) {

                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "date_day"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->date_day = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "date_month"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->date_month = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "date_year"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->date_year = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "date_hour"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->date_hour = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "date_minute"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->date_minute = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_time"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->test_time = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_type"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->test_type = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_mode"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->test_mode = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_kana_set"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->test_kana_set = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_questions"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->test_questions = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_correct_answers"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                    entry->test_correct_answers = atoi ((gchar *) key);
                            xmlFree (key);
                        }
                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_rwaq"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                entry->test_repeat_mode = (!atoi ((gchar *) key)?REPEAT_NONE:REPEAT_WRONG);
                            xmlFree (key);
                        }

                        if ((!xmlStrcmp (cnode->name, (const xmlChar *) "test_repeat_mode"))) {
                            key = xmlNodeListGetString (doc, cnode->xmlChildrenNode, 1);
                            if (key != NULL)
                                entry->test_repeat_mode = atoi ((gchar *) key);
                            xmlFree (key);
                        }

                        cnode = cnode->next;
                    }

                    stats_add_entry (entry, appGUI);
                }

                node = node->next;
            }

        }

        xmlFree(node);
        xmlFreeDoc(doc);
    }

    i = 0;

    if((doc = xmlParseFile (prefs_get_config_filename (KANAS_FILENAME, config_dirname)))) {

        if(!(node = xmlDocGetRootElement (doc))) {
            return;
        }

        if(!xmlStrcmp (node->name, (xmlChar *) STATS_NAME)) {

            /* read hiragana stats */
            hiragana_node = node->xmlChildrenNode;

            while (hiragana_node != NULL) {

                if (i < NUMBER_OF_SIGNS) {
                    if(!xmlStrcmp (hiragana_node->name, (xmlChar *) get_kana_token (i, HIRAGANA))) {

                        prop = (gchar *) xmlGetProp (hiragana_node, (const xmlChar *) "count");
                        if (prop != NULL) {
                            count = atoi (prop);
                            xmlFree (prop);
                        }
                        prop = (gchar *) xmlGetProp (hiragana_node, (const xmlChar *) "correct");
                        if (prop != NULL) {
                            correct = atoi (prop);
                            xmlFree (prop);
                        }
                        prop = (gchar *) xmlGetProp (hiragana_node, (const xmlChar *) "id");
                        if (prop != NULL) {
                            id = atoi (prop);
                            xmlFree (prop);
                        }

                        if (id >= 0 && id < NUMBER_OF_SIGNS) {
                            appGUI->sts->hiragana_counters[id] = count;
                            appGUI->sts->correct_hiragana_counters[id] = correct;
                        }

                        i++;
                    }
                }

                hiragana_node = hiragana_node->next;
            }

            i = 0;

            /* read katakana stats */
            katakana_node = node->xmlChildrenNode;

            while (katakana_node != NULL) {

                if (i < NUMBER_OF_SIGNS) {
                    if(!xmlStrcmp (katakana_node->name, (xmlChar *) get_kana_token (i, KATAKANA))) {

                        prop = (gchar *) xmlGetProp (katakana_node, (const xmlChar *) "count");
                        if (prop != NULL) {
                            count = atoi (prop);
                            xmlFree (prop);
                        }
                        prop = (gchar *) xmlGetProp (katakana_node, (const xmlChar *) "correct");
                        if (prop != NULL) {
                            correct = atoi (prop);
                            xmlFree (prop);
                        }
                        prop = (gchar *) xmlGetProp (katakana_node, (const xmlChar *) "id");
                        if (prop != NULL) {
                            id = atoi (prop);
                            xmlFree (prop);
                        }

                        if (id >= 0 && id < NUMBER_OF_SIGNS) {
                            appGUI->sts->katakana_counters[id] = count;
                            appGUI->sts->correct_katakana_counters[id] = correct;
                        }

                        i++;
                    }
                }

                katakana_node = katakana_node->next;
            }
        }
    }
}

/*--------------------------------------------------------------------*/

void
stats_write_list (gchar *stats_filename, gchar *config_dirname, GUI *appGUI) {

GSList *lnode;
stats_entry *entry;
gint i;
xmlDocPtr doc;
xmlNodePtr node, stats_node, kana_node;
gchar temp[BUFFER_SIZE];

    /* write lessons stats */

    doc = xmlNewDoc ((const xmlChar *) "1.0");
    node = xmlNewNode (NULL, (const xmlChar *) STATS_NAME);
    xmlDocSetRootElement (doc, node);

    for (i = 0, lnode = appGUI->sts->stats_list; lnode; lnode = lnode->next, i++) {

        entry = g_slist_nth_data (appGUI->sts->stats_list, i);
        stats_node = xmlNewChild (node, NULL, (const xmlChar *) "test_stats", (xmlChar *) NULL);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->date_day);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "date_day", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->date_month);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "date_month", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->date_year);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "date_year", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->date_hour);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "date_hour", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->date_minute);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "date_minute", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->test_time);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "test_time", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->test_type);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "test_type", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->test_mode);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "test_mode", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->test_kana_set);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "test_kana_set", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->test_questions);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "test_questions", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->test_correct_answers);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "test_correct_answers", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", entry->test_repeat_mode);
        xmlNewChild (stats_node, NULL, (const xmlChar *) "test_repeat_mode", (xmlChar *) temp);
    }

    xmlSaveFormatFile (prefs_get_config_filename (stats_filename, config_dirname), doc, 1);
    xmlFreeDoc (doc);

    /* write kanas stats */

    doc = xmlNewDoc ((const xmlChar *) "1.0");
    node = xmlNewNode (NULL, (const xmlChar *) STATS_NAME);
    xmlDocSetRootElement (doc, node);

    for (i=0; i < NUMBER_OF_SIGNS; i++) {
        kana_node = xmlNewChild (node, NULL, (const xmlChar *) get_kana_token (i, HIRAGANA), NULL);

        g_snprintf (temp, BUFFER_SIZE, "%d", appGUI->sts->hiragana_counters[i]);
        xmlNewProp (kana_node, (const xmlChar *) "count", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", appGUI->sts->correct_hiragana_counters[i]);
        xmlNewProp (kana_node, (const xmlChar *) "correct", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", i);
        xmlNewProp (kana_node, (const xmlChar *) "id", (xmlChar *) temp);
    }

    for (i=0; i < NUMBER_OF_SIGNS; i++) {
        kana_node = xmlNewChild (node, NULL, (const xmlChar *) get_kana_token (i, KATAKANA), NULL);

        g_snprintf (temp, BUFFER_SIZE, "%d", appGUI->sts->katakana_counters[i]);
        xmlNewProp (kana_node, (const xmlChar *) "count", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", appGUI->sts->correct_katakana_counters[i]);
        xmlNewProp (kana_node, (const xmlChar *) "correct", (xmlChar *) temp);
        g_snprintf (temp, BUFFER_SIZE, "%d", i);
        xmlNewProp (kana_node, (const xmlChar *) "id", (xmlChar *) temp);
    }

    xmlSaveFormatFile (prefs_get_config_filename (KANAS_FILENAME, config_dirname), doc, 1);
    xmlFreeDoc (doc);
}


/*--------------------------------------------------------------------*/

