
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

#ifndef _MAIN_H
#define _MAIN_H

typedef enum {
    ROMAJI = 0,     /* don't change order ! */
    HIRAGANA,
    KATAKANA,
    MIXED,
    STAT
} tmode;

enum {
  REPEAT_NONE = 0,
  REPEAT_WRONG = 1, /* to ensure compatability with old stats */
  REPEAT_ALL
};

typedef enum {
    NORMAL = 0,
    REVERSE,
} mode;

#endif /* _MAIN_H */


