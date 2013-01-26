
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

#include <string.h>
#include <gtk/gtk.h>

#include "gui_utils.h"


/*------------------------------------------------------------------------------*/

void
gui_stock (GtkWidget *button, gchar *blabel, const gchar *bstock) {

GtkWidget   *alignment;
GtkWidget   *hbox;
GtkWidget   *image;

    alignment = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
    hbox = gtk_hbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (alignment), hbox);

    image = gtk_image_new_from_stock (bstock, GTK_ICON_SIZE_BUTTON);

    if (image) {
        gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, TRUE, 0);
    }

    if (blabel) {
        gtk_box_pack_start (GTK_BOX (hbox),
            g_object_new (GTK_TYPE_LABEL, "label", blabel, "use_underline", TRUE, NULL), FALSE, TRUE, 0);
    }

#ifdef MAEMO
    hildon_gtk_widget_set_theme_size(button, HILDON_SIZE_THUMB_HEIGHT);
#endif
    gtk_widget_show_all (alignment);
    gtk_container_add (GTK_CONTAINER (button), alignment);
}


GtkWidget*
gui_stock_label_button (gchar *blabel, const gchar *bstock) {

GtkWidget   *button;

    button = g_object_new (GTK_TYPE_BUTTON, "visible", TRUE, NULL);
    gui_stock (button, blabel, bstock);

    return button;
}

/*------------------------------------------------------------------------------*/

GtkWidget*
gui_stock_label_togglebutton (gchar *blabel, const gchar *bstock) {

GtkWidget   *button;

    button = gtk_toggle_button_new ();
    gui_stock (button, blabel, bstock);

    return button;
}

/*------------------------------------------------------------------------------*/

gchar *
gui_get_www_browser (void) {

static gchar browser_cmd[PATH_MAX];

gchar *browsers[] = {
    "xdg-open",    
    "firefox",      
    "opera",        
    "x-www-browser",    
    "htmlview",
    "epiphany",     
    "konqueror",    
    "netscape"
};

gint i;
gchar *found_path;
gboolean found = FALSE;

    for (i=0; i < (sizeof(browsers) / sizeof(browsers[0])); i++) {
        found_path = g_find_program_in_path (browsers[i]);
        if (found_path != NULL) {
            found = TRUE;
            g_free (found_path);
            break;
        }
    }

    i = found ? i : 0;
    g_snprintf (browser_cmd, PATH_MAX, "%s ", browsers[i]);

    return browser_cmd;
}

/*------------------------------------------------------------------------------*/

gchar *
gui_get_email_client (void) {

static gchar client_cmd[PATH_MAX];

gchar *clients[] = {
    "xdg-email",    "",
    "claws-mail",   "--compose",
    "thunderbird",  "mailto:",
    "sylpheed",     "--compose",
    "evolution",    "mailto:",
    "kmail",        ""
};

gint i;
gchar *found_path;
gboolean found = FALSE;

    for (i=0; i < (sizeof(clients) / sizeof(clients[0]) / 2); i++) {
        found_path = g_find_program_in_path (clients[i*2+0]);
        if (found_path != NULL) {
            found = TRUE;
            g_free (found_path);
            break;
        }
    }

    i = found ? i : 0;

    if (strcmp(clients[i*2+0], "mailto:")) {
        g_snprintf (client_cmd, PATH_MAX, "%s %s ", clients[i*2+0], clients[i*2+1]);
    } else {
        g_snprintf (client_cmd, PATH_MAX, "%s %s", clients[i*2+0], clients[i*2+1]);
    }
    return client_cmd;
}

/*------------------------------------------------------------------------------*/

gboolean
gui_run_helper(gchar *parameter, gint helper) {

gchar *cmdline[4], command[PATH_MAX];

    if (helper == EMAIL) {
        g_snprintf (command, PATH_MAX, "%s%s", gui_get_email_client(), parameter);
    } else if (helper == WWW) {
        g_snprintf (command, PATH_MAX, "%s%s", gui_get_www_browser(), parameter);
    } else {
        return FALSE;
    }

    cmdline[0] = "sh";
    cmdline[1] = "-c";
    cmdline[2] = command;
    cmdline[3] = 0;
    g_spawn_async (NULL, (gchar **)cmdline, NULL, G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL, 
                   NULL, NULL, NULL, FALSE);

    return TRUE;
}

/*------------------------------------------------------------------------------*/

void
gui_url_initialize(GUI *appGUI) {

    appGUI->hand_cursor = gdk_cursor_new (GDK_HAND2);
    appGUI->regular_cursor = gdk_cursor_new (GDK_XTERM);

    appGUI->hovering_over_link = FALSE;
    appGUI->gui_url_tag = NULL;
}

/*------------------------------------------------------------------------------*/

void 
gui_url_insert_link (GSList *links_list, gint *link_index, GtkWidget *textview, 
                     GtkTextIter *iter, gchar *color, gint font_size, gchar *text, gboolean center, GUI *appGUI) {

    if (font_size == 0 && color == NULL) {
        if (center == TRUE) {
            appGUI->gui_url_tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)),
                                                              NULL, "foreground", "blue", 
                                                              "underline", PANGO_UNDERLINE_SINGLE,
                                                              "justification", GTK_JUSTIFY_CENTER, NULL);
        } else {
            appGUI->gui_url_tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)),
                                                              NULL, "foreground", "blue", 
                                                              "underline", PANGO_UNDERLINE_SINGLE, NULL);
        }
    } else {
        if (center == TRUE) {
            appGUI->gui_url_tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)),
                                                              NULL, "foreground", color, 
                                                              "underline", PANGO_UNDERLINE_SINGLE, 
                                                              "size", font_size * PANGO_SCALE,
                                                              "justification", GTK_JUSTIFY_CENTER, NULL);
        } else {
            appGUI->gui_url_tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)),
                                                              NULL, "foreground", color, 
                                                              "underline", PANGO_UNDERLINE_SINGLE, 
                                                              "size", font_size * PANGO_SCALE, NULL);
        }
    }

    g_object_set_data (G_OBJECT (appGUI->gui_url_tag), "link", GINT_TO_POINTER (*link_index));

    links_list = g_slist_append (links_list, g_strdup(text));
    gtk_text_buffer_insert_with_tags (gtk_text_view_get_buffer (GTK_TEXT_VIEW(textview)), 
                                      iter, text, -1, appGUI->gui_url_tag, NULL);
    (*link_index)++;
}

/*------------------------------------------------------------------------------*/

void
gui_url_remove_links (GSList *links_list, gint *link_index) {

gint i;
gchar *link;
GSList *lnode;

        *link_index = 1;

        if (links_list != NULL) {

            for (i = 0, lnode = links_list; lnode != NULL; lnode = lnode->next, i++) {
                link = g_slist_nth_data (links_list, i);
                if (link != NULL) {
                    g_free (link);
                }
            }

            for (i = 0, lnode = links_list; lnode != NULL; lnode = lnode->next, i++) {
                links_list = g_slist_remove_link (links_list, g_slist_nth(links_list, i));
            }

        }
}


/*------------------------------------------------------------------------------*/

void
gui_url_set_cursor_if_appropriate (GtkTextView *textview, gint x, gint y, GUI *appGUI) {

GSList *tags = NULL, *tagp = NULL;
GtkTextIter iter;
gboolean hovering = FALSE;
GtkTextTag *tag;
int *slink;

    gtk_text_view_get_iter_at_location (textview, &iter, x, y);

    tags = gtk_text_iter_get_tags (&iter);

    for (tagp = tags;  tagp != NULL;  tagp = tagp->next) {
        tag = tagp->data;
        slink = g_object_get_data (G_OBJECT (tag), "link");

        if (slink != 0) {
            hovering = TRUE;
            break;
        }
    }

    if (hovering != appGUI->hovering_over_link) {
        appGUI->hovering_over_link = hovering;

        if (appGUI->hovering_over_link) {
            gdk_window_set_cursor (gtk_text_view_get_window (textview, GTK_TEXT_WINDOW_TEXT), appGUI->hand_cursor);
        } else {
            gdk_window_set_cursor (gtk_text_view_get_window (textview, GTK_TEXT_WINDOW_TEXT), appGUI->regular_cursor);
        }
    }

    if (tags) {
        g_slist_free (tags);
    }
}

/*------------------------------------------------------------------------------*/

gboolean
gui_url_event_after (GtkWidget *textview, GdkEvent *ev, GSList *links_list) {

GtkTextIter start, end, iter;
GtkTextBuffer *buffer;
GdkEventButton *event;
gint x, y;
GSList *tags = NULL, *tagp = NULL;
GtkTextTag *tag;
gchar *link;
size_t slink;

    if (ev->type != GDK_BUTTON_RELEASE) {
        return FALSE;
    }

    event = (GdkEventButton *)ev;

    if (event->button != 1) {
        return FALSE;
    }

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));

    gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
    if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end)) {
        return FALSE;
    }

    gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (textview), GTK_TEXT_WINDOW_WIDGET, 
                                           event->x, event->y, &x, &y);
    gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (textview), &iter, x, y);

    tags = gtk_text_iter_get_tags (&iter);

    for (tagp = tags;  tagp != NULL;  tagp = tagp->next) {

        tag = tagp->data;
        slink = (size_t)g_object_get_data (G_OBJECT (tag), "link");

        if (slink != 0) {
            link = g_slist_nth_data (links_list, slink);
            gui_run_helper (link, gui_get_link_type(link));
            break;
        }
    }

    if (tags) {
        g_slist_free (tags);
    }

    return FALSE;
}

/*------------------------------------------------------------------------------*/


gboolean
gui_url_motion_notify_event (GtkWidget *textview, GdkEventMotion *event, gpointer data) {

gint x, y;

    GUI *appGUI = (GUI *)data;

    gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (textview), GTK_TEXT_WINDOW_WIDGET, 
                                           event->x, event->y, &x, &y);
    gui_url_set_cursor_if_appropriate (GTK_TEXT_VIEW (textview), x, y, appGUI);
    gdk_window_get_pointer (gtk_widget_get_window(textview), NULL, NULL, NULL);
    return FALSE;
}

/*------------------------------------------------------------------------------*/

gboolean
gui_url_visibility_notify_event (GtkWidget *textview, GdkEventVisibility *event, gpointer data) {

gint wx, wy, bx, by;

    GUI *appGUI = (GUI *)data;

    gdk_window_get_pointer (gtk_widget_get_window(textview), &wx, &wy, NULL);
    gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (textview), GTK_TEXT_WINDOW_WIDGET, wx, wy, &bx, &by);
    gui_url_set_cursor_if_appropriate (GTK_TEXT_VIEW (textview), bx, by, appGUI);
    return FALSE;
}

/*------------------------------------------------------------------------------*/

void
gui_url_setup (GSList *links_list, gint *link_index, GtkWidget *textview, GUI *appGUI) {

    gui_url_remove_links (links_list, link_index);

    *link_index = 1;
    appGUI->hovering_over_link = FALSE;
    appGUI->gui_url_tag = NULL;

    g_signal_connect (textview, "event-after", G_CALLBACK (gui_url_event_after), links_list);
    g_signal_connect (textview, "motion-notify-event", G_CALLBACK (gui_url_motion_notify_event), appGUI);
    g_signal_connect (textview, "visibility-notify-event", G_CALLBACK (gui_url_visibility_notify_event), appGUI);

}

/*------------------------------------------------------------------------------*/

gint
gui_get_link_type (gchar *link) {

gint i, n;

    for(i=n=0; i < strlen(link); i++) {
        if(link[i] == '@') n++;
    }

    if (!strncasecmp(link, "http://", 7) || !strncasecmp(link, "www", 3)) {
        return WWW;
    } else if (n == 1) {
        return EMAIL;
    } else {
        return UNKNOWN;
    }

}

/*------------------------------------------------------------------------------*/


