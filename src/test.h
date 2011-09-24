
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

#ifndef _TEST_H
#define _TEST_H

gchar *     get_test_mode_name      (gint mode);
gchar *     get_kana_set_name       (gint set);
gchar *     get_mode_name           (gint mode);
gchar *     get_kana_sign           (gint index, gint kana_type, gboolean translated);

void        test_init               (GUI *appGUI);
void        test_info               (GUI *appGUI);
void        test_check_answer       (gchar *user_answer, GUI *appGUI);
void        test_check_choice       (gint user_choice, GUI *appGUI);
void        test_generate_tables    (gint number_of_questions, GUI *appGUI);
void        test_generate_choices   (gint kana_number, GUI *appGUI);
gchar *     test_sec2str            (guint seconds, gboolean mshort);

#endif /* _TEST_H */

