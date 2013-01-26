
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

#include "main.h"
#include "gui.h"
#include "prefs.h"

struct kanatest_prefs config;

/*--------------------------------------------------------------------*/

gchar*
prefs_get_config_filename (gchar *config_filename, gchar *config_dirname) {

static gchar filename[PATH_MAX];
gchar dirname[PATH_MAX];

    g_snprintf (dirname, PATH_MAX, "%s%c%s", g_get_home_dir(), G_DIR_SEPARATOR, config_dirname);

    if (g_file_test (dirname, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR) != TRUE) {
        g_mkdir (dirname, S_IRUSR | S_IWUSR | S_IXUSR);
    }

    g_snprintf (filename, PATH_MAX, "%s%c%s", dirname, G_DIR_SEPARATOR, config_filename);

    return filename;
}

/*--------------------------------------------------------------------*/

void
prefs_set_default_values (void) {
#ifndef MAEMO
    config.window_x = 180;
    config.window_y = 120;
    config.window_size_x = 365;
    config.window_size_y = 445;
#endif    
    config.repeat_mode = REPEAT_NONE;
    config.kana_mode = HIRAGANA;
    config.test_mode = NORMAL;
    config.kana_set = 0;            /* all kanas */
    config.ca_timeout = 1;          /* 1 sec */
#ifndef MAEMO
    config.stat_window_x = 45;
    config.stat_window_y = 120;
    config.stat_size_x = 760;
    config.stat_size_y = 450;
    config.chart_window_x = 200;
    config.chart_window_y = 60;
    config.about_window_x = 320;
    config.about_window_y = 165;
    config.options_window_x = 280;
    config.options_window_y = 150;
#endif    
    config.stats_sort_column = 0;
    config.stats_sort_column_dir = 1;
    config.hiragana_mode = 1;
    config.ratio_begin = 0;
    config.ratio_end = 50;
    g_strlcpy (config.kana_font_face, "Serif", MAX_FONTNAME_LEN);
#ifdef MAEMO 
    g_strlcpy (config.kana_color, "#FFFFFF", MAX_COLORNAME_LEN);
#else   
    g_strlcpy (config.kana_color, "#000000", MAX_COLORNAME_LEN);
#endif    
    g_strlcpy (config.romaji_color, "#BB1010", MAX_COLORNAME_LEN);
    g_strlcpy (config.timer_color, "#202080", MAX_COLORNAME_LEN);
    g_strlcpy (config.user_defined_lesson,
               "+++++----------------------------------------------------------------------------------------------------", NUMBER_OF_SIGNS);
}

/*--------------------------------------------------------------------*/

void
prefs_read_config (gchar *config_filename, gchar *config_dirname) {

gboolean cfg_file;
xmlDocPtr doc;
xmlNodePtr node;
xmlChar *key;

    cfg_file = g_file_test (prefs_get_config_filename (config_filename, config_dirname), G_FILE_TEST_IS_REGULAR);

    if (cfg_file == TRUE) {

        doc = xmlParseFile (prefs_get_config_filename (config_filename, config_dirname));
        if (doc == NULL)
                return;

        node = xmlDocGetRootElement (doc);
        if (node == NULL) {
                xmlFreeDoc (doc);
                return;
        }

        if (xmlStrcmp (node->name, (const xmlChar *) CONFIG_NAME)) {
                xmlFreeDoc (doc);
                return;
        }

        prefs_set_default_values();

        node = node->xmlChildrenNode;

        while (node != NULL) {
#ifndef MAEMO
                if ((!xmlStrcmp (node->name, (const xmlChar *) "window_x"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.window_x);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "window_y"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.window_y);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "window_size_x"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.window_size_x);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "window_size_y"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.window_size_y);
                    xmlFree (key);
                }
#endif
                if ((!xmlStrcmp (node->name, (const xmlChar *) "repeat_mode"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.repeat_mode);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "kana_mode"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.kana_mode);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "test_mode"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.test_mode);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "kana_set"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.kana_set);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "ca_timeout"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.ca_timeout);
                    xmlFree (key);
                }
#ifndef MAEMO
                if ((!xmlStrcmp (node->name, (const xmlChar *) "stat_window_x"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.stat_window_x);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "stat_window_y"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.stat_window_y);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "stat_size_x"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.stat_size_x);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "stat_size_y"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.stat_size_y);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "about_window_x"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.about_window_x);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "about_window_y"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.about_window_y);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "options_window_x"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.options_window_x);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "options_window_y"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.options_window_y);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "chart_window_x"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.chart_window_x);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "chart_window_y"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.chart_window_y);
                    xmlFree (key);
                }
#endif
                if ((!xmlStrcmp (node->name, (const xmlChar *) "stats_sort_column"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.stats_sort_column);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "stats_sort_column_dir"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.stats_sort_column_dir);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "hiragana_mode"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.hiragana_mode);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "ratio_begin"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.ratio_begin);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *) "ratio_end"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            sscanf ((gchar *) key, "%d", &config.ratio_end);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *)"kana_font_face"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            g_strlcpy (config.kana_font_face, (gchar *) key, MAX_FONTNAME_LEN);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *)"kana_color"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            g_strlcpy (config.kana_color, (gchar *) key, MAX_COLORNAME_LEN);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *)"romaji_color"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            g_strlcpy (config.romaji_color, (gchar *) key, MAX_COLORNAME_LEN);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *)"timer_color"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL)
                            g_strlcpy (config.timer_color, (gchar *) key, MAX_COLORNAME_LEN);
                    xmlFree (key);
                }

                if ((!xmlStrcmp (node->name, (const xmlChar *)"user_defined_lesson"))) {
                    key = xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
                    if (key != NULL) 
                            g_strlcpy (config.user_defined_lesson, (gchar *) key, sizeof(config.user_defined_lesson));
                    xmlFree (key);
                }

            node = node->next;
        }

        xmlFreeDoc(doc);

    } else {

        prefs_set_default_values();
        prefs_write_config (config_filename, config_dirname);
    }

}

/*--------------------------------------------------------------------*/

void
prefs_write_config(gchar *config_filename, gchar *config_dirname) {

xmlDocPtr doc;
xmlNodePtr node;
gchar temp[BUFFER_SIZE];

    doc = xmlNewDoc ((const xmlChar *) "1.0");
    node = xmlNewNode (NULL, (const xmlChar *) CONFIG_NAME);
    xmlDocSetRootElement (doc, node);
#ifndef MAEMO
    g_snprintf (temp, BUFFER_SIZE, "%d", config.window_x);
    xmlNewTextChild (node, NULL, (const xmlChar *) "window_x", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.window_y);
    xmlNewTextChild (node, NULL, (const xmlChar *) "window_y", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.window_size_x);
    xmlNewTextChild (node, NULL, (const xmlChar *) "window_size_x", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.window_size_y);
    xmlNewTextChild (node, NULL, (const xmlChar *) "window_size_y", (xmlChar *) temp);
#endif    
    g_snprintf (temp, BUFFER_SIZE, "%d", config.repeat_mode);
    xmlNewTextChild (node, NULL, (const xmlChar *) "repeat_mode", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.kana_mode);
    xmlNewTextChild (node, NULL, (const xmlChar *) "kana_mode", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.test_mode);
    xmlNewTextChild (node, NULL, (const xmlChar *) "test_mode", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.kana_set);
    xmlNewTextChild (node, NULL, (const xmlChar *) "kana_set", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.ca_timeout);
    xmlNewTextChild (node, NULL, (const xmlChar *) "ca_timeout", (xmlChar *) temp);
#ifndef MAEMO    
    g_snprintf (temp, BUFFER_SIZE, "%d", config.stat_window_x);
    xmlNewTextChild (node, NULL, (const xmlChar *) "stat_window_x", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.stat_window_y);
    xmlNewTextChild (node, NULL, (const xmlChar *) "stat_window_y", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.stat_size_x);
    xmlNewTextChild (node, NULL, (const xmlChar *) "stat_size_x", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.stat_size_y);
    xmlNewTextChild (node, NULL, (const xmlChar *) "stat_size_y", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.about_window_x);
    xmlNewTextChild (node, NULL, (const xmlChar *) "about_window_x", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.about_window_y);
    xmlNewTextChild (node, NULL, (const xmlChar *) "about_window_y", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.options_window_x);
    xmlNewTextChild (node, NULL, (const xmlChar *) "options_window_x", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.options_window_y);
    xmlNewTextChild (node, NULL, (const xmlChar *) "options_window_y", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.chart_window_x);
    xmlNewTextChild (node, NULL, (const xmlChar *) "chart_window_x", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.chart_window_y);
    xmlNewTextChild (node, NULL, (const xmlChar *) "chart_window_y", (xmlChar *) temp);
#endif    
    g_snprintf (temp, BUFFER_SIZE, "%d", config.stats_sort_column);
    xmlNewTextChild (node, NULL, (const xmlChar *) "stats_sort_column", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.stats_sort_column_dir);
    xmlNewTextChild (node, NULL, (const xmlChar *) "stats_sort_column_dir", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.hiragana_mode);
    xmlNewTextChild (node, NULL, (const xmlChar *) "hiragana_mode", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.ratio_begin);
    xmlNewTextChild (node, NULL, (const xmlChar *) "ratio_begin", (xmlChar *) temp);
    g_snprintf (temp, BUFFER_SIZE, "%d", config.ratio_end);
    xmlNewTextChild (node, NULL, (const xmlChar *) "ratio_end", (xmlChar *) temp);
    g_snprintf (temp, MAX_FONTNAME_LEN, "%s", config.kana_font_face);
    xmlNewTextChild (node, NULL, (const xmlChar *) "kana_font_face", (xmlChar *) temp);
    g_snprintf (temp, MAX_COLORNAME_LEN, "%s", config.kana_color);
    xmlNewTextChild (node, NULL, (const xmlChar *) "kana_color", (xmlChar *) temp);
    g_snprintf (temp, MAX_COLORNAME_LEN, "%s", config.romaji_color);
    xmlNewTextChild (node, NULL, (const xmlChar *) "romaji_color", (xmlChar *) temp);
    g_snprintf (temp, MAX_COLORNAME_LEN, "%s", config.timer_color);
    xmlNewTextChild (node, NULL, (const xmlChar *) "timer_color", (xmlChar *) temp);
    g_snprintf (temp, NUMBER_OF_SIGNS+1, "%s", config.user_defined_lesson);
    xmlNewTextChild (node, NULL, (const xmlChar *) "user_defined_lesson", (xmlChar *) temp);

    xmlSaveFormatFile (prefs_get_config_filename (config_filename, config_dirname), doc, 1);
    xmlFreeDoc (doc);
}

/*--------------------------------------------------------------------*/

