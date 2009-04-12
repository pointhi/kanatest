
/*
 * Osmo - a handy personal organizer
 *
 * Copyright (C) 2007 Tomasz Maka <pasp@users.sourceforge.net>
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
#include "stock_icons.h"
#include "stock_icons_data.h"

/*------------------------------------------------------------------------------*/

void
kanatest_register_stock_icons(void) {

GdkPixbuf *pixbuf;
gint i;
GtkIconFactory *factory;
GtkIconSet *icon_set;
GtkIconSource *icon_source;

GtkStockItem kanatest_stock_items[] = {
    { (gchar*) KANATEST_STOCK_BUTTON_STATISTICS,    (gchar*) N_("Statistics"),  0, 0, TRANSLATION_DOMAIN },
    { (gchar*) KANATEST_STOCK_BUTTON_CHART,         (gchar*) N_("Chart"),       0, 0, TRANSLATION_DOMAIN },
    { (gchar*) KANATEST_STOCK_BUTTON_OPTIONS,       (gchar*) N_("Options"),     0, 0, TRANSLATION_DOMAIN },
    { (gchar*) KANATEST_STOCK_BUTTON_ABOUT,         (gchar*) N_("About"),       0, 0, TRANSLATION_DOMAIN },
};

const guint8* kanatest_stock_item_data[] = {
    (const guint8*) kanatest_stock_button_statistics,
    (const guint8*) kanatest_stock_button_chart,
    (const guint8*) kanatest_stock_button_options,
    (const guint8*) kanatest_stock_button_about,
};

    gtk_stock_add (kanatest_stock_items, G_N_ELEMENTS (kanatest_stock_items));

    factory = gtk_icon_factory_new ();
    gtk_icon_factory_add_default(factory);

    for(i = 0; i < G_N_ELEMENTS(kanatest_stock_item_data); i++) {
        pixbuf = gdk_pixbuf_new_from_inline(-1, kanatest_stock_item_data[i], FALSE, NULL);
        icon_source = gtk_icon_source_new ();
        gtk_icon_source_set_pixbuf (icon_source, pixbuf);

        icon_set = gtk_icon_set_new ();
        gtk_icon_set_add_source (icon_set, icon_source);

        gtk_icon_factory_add (factory, kanatest_stock_items[i].stock_id, icon_set);
        g_object_unref (pixbuf);
        gtk_icon_source_free (icon_source);
        gtk_icon_set_unref (icon_set);
    }

    g_object_unref (factory);

}

/*------------------------------------------------------------------------------*/


