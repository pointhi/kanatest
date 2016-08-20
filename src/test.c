
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
#include "main.h"
#include "test.h"
#include "prefs.h"
#include "stats.h"
#include "options.h"
#include "i18n.h"
#ifdef MAEMO
#include <hildon/hildon.h>
#endif

#if defined(WIN32) || defined(WIN64)
#define _LRAND48(x) rand() % (x)
#else
#define _LRAND48(x) lrand48() % (x)
#endif

/*--------------------------------------------------------------------*/

gchar *
get_test_mode_name (gint mode) {

gchar *test_mode_name[] = {
    NULL, N_("Hiragana"), N_("Katakana"), N_("Mixed")
};

    return gettext(test_mode_name[mode]);
}

/*--------------------------------------------------------------------*/

gchar *
get_mode_name (gint mode) {

gchar *mode_name[] = {
    N_("Normal"), N_("Reverse")
};

    return gettext(mode_name[mode]);
}

/*--------------------------------------------------------------------*/

gchar *
get_kana_set_name (gint set) {

gchar *kana_set_name[] = {
    N_("All kanas"),
    N_("Basic kanas"),
    N_("A-I-U-E-O"),
    N_("KA-KI-KU-KE-KO"),
    N_("SA-SHI-SU-SE-SO"),
    N_("TA-CHI-TSU-TE-TO"),
    N_("NA-NI-NU-NE-NO"),
    N_("HA-HI-FU-HE-HO"),
    N_("MA-MI-MU-ME-MO"),
    N_("YA-YU-YO"),
    N_("RA-RI-RU-RE-RO"),
    N_("WA-WO-N"),
    N_("Combo letters (part 1)"),
    N_("Combo letters (part 2)"),
    N_("User-defined lesson")
};

    return gettext(kana_set_name[set]);
}

/*--------------------------------------------------------------------*/
/* index is the number of kana (index of the character within its set) */
gchar *
get_kana_sign (gint index, gint kana_type, gboolean translated) {

/* romaji | hiragana | katakana */

gchar *kana_signs[] = {

    /* TRANSLATORS: if there exist several possible transliterations for kanas, they must be separated by comma */

    /* 0 */                 /* 1 */                     /* 2 */                     /* 3 */                 /* 4 */
    N_("a"),  "あ", "ア",   N_("i"),   "い", "イ",      N_("u"),   "う", "ウ",      N_("e"),  "え", "エ",   N_("o"),  "お", "オ",
    /* 5 */                 /* 6 */                     /* 7 */                     /* 8 */                 /* 9 */
    N_("ka"), "か", "カ",   N_("ki"),  "き", "キ",      N_("ku"),  "く", "ク",      N_("ke"), "け", "ケ",   N_("ko"), "こ", "コ",
    /* 10 */                /* 11 */                    /* 12 */                    /* 13 */                /* 14 */
    N_("sa"), "さ", "サ",   N_("shi"), "し", "シ",      N_("su"),  "す", "ス",      N_("se"), "せ", "セ",   N_("so"), "そ", "ソ",
    /* 15 */                /* 16 */                    /* 17 */                    /* 18 */                /* 19 */
    N_("ta"), "た", "タ",   N_("chi"), "ち", "チ",      N_("tsu"), "つ", "ツ",      N_("te"), "て", "テ",   N_("to"), "と", "ト",

    /* 20 */                /* 21 */                    /* 22 */                    /* 23 */                /* 24 */
    N_("na"), "な", "ナ",   N_("ni"), "に", "ニ",       N_("nu"), "ぬ", "ヌ",       N_("ne"), "ね", "ネ",   N_("no"), "の", "ノ",
    /* 25 */                /* 26 */                    /* 27 */                    /* 28 */                /* 29 */
    N_("ha"), "は", "ハ",   N_("hi"), "ひ", "ヒ",       N_("fu"), "ふ", "フ",       N_("he"), "へ", "ヘ",   N_("ho"), "ほ", "ホ",
    /* 30 */                /* 31 */                    /* 32 */                    /* 33 */                /* 34 */
    N_("ma"), "ま", "マ",   N_("mi"), "み", "ミ",       N_("mu"), "む", "ム",       N_("me"), "め", "メ",   N_("mo"), "も", "モ",
    /* 35 */                                            /* 36 */                                            /* 37 */
    N_("ya"), "や", "ヤ",                               N_("yu"), "ゆ", "ユ",                               N_("yo"), "よ", "ヨ",
    /* 38 */                /* 39 */                    /* 40 */                    /* 41 */                /* 42 */
    N_("ra"), "ら", "ラ",   N_("ri"), "り", "リ",       N_("ru"), "る", "ル",       N_("re"), "れ", "レ",   N_("ro"), "ろ", "ロ",

    /* 43 */                                                                                                /* 44 */
    N_("wa"), "わ", "ワ",                                                                                   N_("wo"), "を", "ヲ",
    /* 45 */
    N_("n"),  "ん", "ン",

    /* 46 */                /* 47 */                    /* 48 */                    /* 49 */                /* 50 */
    N_("ga"), "が", "ガ",   N_("gi"), "ぎ", "ギ",       N_("gu"), "ぐ", "グ",       N_("ge"), "げ", "ゲ",   N_("go"), "ご", "ゴ",
    /* 51 */                /* 52 */                    /* 53 */                    /* 54 */                /* 55 */
    N_("za"), "ざ", "ザ",   N_("ji"), "じ", "ジ",       N_("zu"), "ず", "ズ",       N_("ze"), "ぜ", "ゼ",   N_("zo"), "ぞ", "ゾ",
    /* 56 */                /* 57 */                    /* 58 */                    /* 59 */                /* 60 */
    N_("da"), "だ", "ダ",   N_("di,ji"), "ぢ", "ヂ",    N_("du,zu"), "づ", "ヅ",    N_("de"), "で", "デ",   N_("do"), "ど", "ド",
    /* 61 */                /* 62 */                    /* 63 */                    /* 64 */                /* 65 */
    N_("ba"), "ば", "バ",   N_("bi"), "び", "ビ",       N_("bu"), "ぶ", "ブ",       N_("be"), "べ", "ベ",   N_("bo"), "ぼ", "ボ",
    /* 66 */                /* 67 */                    /* 68 */                    /* 69 */                /* 70 */
    N_("pa"), "ぱ", "パ",   N_("pi"), "ぴ", "ピ",       N_("pu"), "ぷ", "プ",       N_("pe"), "ぺ", "ペ",   N_("po"), "ぽ", "ポ",

    /* 71 */                                            /* 72 */                                            /* 73 */
    N_("kya"), "きゃ", "キャ",                          N_("kyu"), "きゅ", "キュ",                          N_("kyo"), "きょ", "キョ",
    /* 74 */                                            /* 75 */                                            /* 76 */
    N_("sha"), "しゃ", "シャ",                          N_("shu"), "しゅ", "シュ",                          N_("sho"), "しょ", "ショ",
    /* 77 */                                            /* 78 */                                            /* 79 */
    N_("cha"), "ちゃ", "チャ",                          N_("chu"), "ちゅ", "チュ",                          N_("cho"), "ちょ", "チョ",
    /* 80 */                                            /* 81 */                                            /* 82 */
    N_("nya"), "にゃ", "ニャ",                          N_("nyu"), "にゅ", "ニュ",                          N_("nyo"), "にょ", "ニョ",
    /* 83 */                                            /* 84 */                                            /* 85 */
    N_("hya"), "ひゃ", "ヒャ",                          N_("hyu"), "ひゅ", "ヒュ",                          N_("hyo"), "ひょ", "ヒョ",
    /* 86 */                                            /* 87 */                                            /* 88 */
    N_("mya"), "みゃ", "ミャ",                          N_("myu"), "みゅ", "ミュ",                          N_("myo"), "みょ", "ミョ",
    /* 89 */                                            /* 90 */                                            /* 91 */
    N_("rya"), "りゃ", "リャ",                          N_("ryu"), "りゅ", "リュ",                          N_("ryo"), "りょ", "リョ",

    /* 92 */                                            /* 93 */                                            /* 94 */
    N_("gya"), "ぎゃ", "ギャ",                          N_("gyu"),  "ぎゅ", "ギュ",                         N_("gyo"), "ぎょ", "ギョ", 
    /* 95 */                                            /* 96 */                                            /* 97 */
    N_("ja"),  "じゃ", "ジャ",                          N_("ju"),   "じゅ", "ジュ",                         N_("jo"),  "じょ", "ジョ", 
    /* 98 */                                            /* 99 */                                            /* 100 */
    N_("bya"), "びゃ", "ビャ",                          N_("byu"),  "びゅ", "ビュ",                         N_("byo"), "びょ", "ビョ", 
    /* 101 */                                           /* 102 */                                           /* 103 */
    N_("pya"), "ぴゃ", "ピャ",                          N_("pyu"),  "ぴゅ", "ピュ",                         N_("pyo"), "ぴょ", "ピョ"

};

    if (translated == TRUE) {
        return gettext(kana_signs[3 * index + kana_type]);
    } else {
        return kana_signs[3 * index + kana_type];
    }
}

/*--------------------------------------------------------------------*/

void
test_init (GUI *appGUI) {

    gint i, pos;

    pos = 0;

    for (i=0; i < NUMBER_OF_SIGNS; i++) {
        appGUI->tst->kana_signs_set_0_idx[i] = i;

        if (config.user_defined_lesson[i] == '+') {
            appGUI->tst->kana_signs_set_14_idx[pos++] = i;
        }
    }

    appGUI->tst->kana_set_len[0] = KANA_SET_0_LEN;
    appGUI->tst->kana_set_len[1] = KANA_SET_1_LEN;
    appGUI->tst->kana_set_len[2] = KANA_SET_2_LEN;
    appGUI->tst->kana_set_len[3] = KANA_SET_3_LEN;
    appGUI->tst->kana_set_len[4] = KANA_SET_4_LEN;
    appGUI->tst->kana_set_len[5] = KANA_SET_5_LEN;
    appGUI->tst->kana_set_len[6] = KANA_SET_6_LEN;
    appGUI->tst->kana_set_len[7] = KANA_SET_7_LEN;
    appGUI->tst->kana_set_len[8] = KANA_SET_8_LEN;
    appGUI->tst->kana_set_len[9] = KANA_SET_9_LEN;
    appGUI->tst->kana_set_len[10] = KANA_SET_10_LEN;
    appGUI->tst->kana_set_len[11] = KANA_SET_11_LEN;
    appGUI->tst->kana_set_len[12] = KANA_SET_12_LEN;
    appGUI->tst->kana_set_len[13] = KANA_SET_13_LEN;

    appGUI->tst->kana_set_len[USER_DEFINED_LEN_OFFSET] = pos;   /* replace -1 with number of selected kanas */

    appGUI->tst->max_entries_in_test = appGUI->tst->kana_set_len[config.kana_set] * ((config.kana_mode == MIXED) ? 2 : 1);

    appGUI->tst->question_counter = 0;
    appGUI->tst->wrong_answer_counter = 0;
    appGUI->tst->right_answer_counter = 0;
}

/*--------------------------------------------------------------------*/

void
test_generate_tables (gint number_of_questions, GUI *appGUI) {

gint kana_signs_set_1_idx[] = {
    0, 1, 2, 3, 4,                  /* A-I-U-E-O */
    5, 6, 7, 8, 9,                  /* KA-KI-KU-KE-KO */
    10, 11, 12, 13, 14,             /* SA-SHI-SU-SE-SO */
    15, 16, 17, 18, 19,             /* TA-CHI-TSU-TE-TO */
    20, 21, 22, 23, 24,             /* NA-NI-NU-NE-NO */
    25, 26, 27, 28, 29,             /* HA-HI-FU-HE-HO */
    30, 31, 32, 33, 34,             /* MA-MI-MU-ME-MO */
    35, 36, 37,                     /* YA-YU-YO */
    38, 39, 40, 41, 42,             /* RA-RI-RU-RE-RO */
    43, 44, 45                      /* WA-WO-N */
};

gint kana_signs_set_2_idx[] = {     /* A-I-U-E-O */
    0, 1, 2, 3, 4
};

gint kana_signs_set_3_idx[] = {     /* KA-KI-KU-KE-KO */
    5, 6, 7, 8, 9
};

gint kana_signs_set_4_idx[] = {     /* SA-SHI-SU-SE-SO */
    10, 11, 12, 13, 14
};

gint kana_signs_set_5_idx[] = {     /* TA-CHI-TSU-TE-TO */
    15, 16, 17, 18, 19
};

gint kana_signs_set_6_idx[] = {     /* NA-NI-NU-NE-NO */
    20, 21, 22, 23, 24
};

gint kana_signs_set_7_idx[] = {     /* HA-HI-FU-HE-HO */
    25, 26, 27, 28, 29
};

gint kana_signs_set_8_idx[] = {     /* MA-MI-MU-ME-MO */
    30, 31, 32, 33, 34
};

gint kana_signs_set_9_idx[] = {     /* YA-YU-YO */
    35, 36, 37
};

gint kana_signs_set_10_idx[] = {    /* RA-RI-RU-RE-RO */
    38, 39, 40, 41, 42
};

gint kana_signs_set_11_idx[] = {    /* WA-WO-N */
    43, 44, 45
};

gint kana_signs_set_12_idx[] = {    /* Combo letters (part 1) */
    46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
    66, 67, 68, 69, 70
};

gint kana_signs_set_13_idx[] = {    /* Combo letters (part 2) */
    71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
    91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
    101, 102, 103
};

gint *kana_signs_set_tabs[] = {
    NULL,                   /* kana_signs_set_0_idx */
    kana_signs_set_1_idx,
    kana_signs_set_2_idx,
    kana_signs_set_3_idx,
    kana_signs_set_4_idx,
    kana_signs_set_5_idx,
    kana_signs_set_6_idx,
    kana_signs_set_7_idx,
    kana_signs_set_8_idx,
    kana_signs_set_9_idx,
    kana_signs_set_10_idx,
    kana_signs_set_11_idx,
    kana_signs_set_12_idx,
    kana_signs_set_13_idx,
    NULL                    /* kana_signs_set_14_idx */
};

gint i, tmp, pos;
gint *table;

    if (config.kana_set == 0) {
        table = appGUI->tst->kana_signs_set_0_idx;
    } else if (config.kana_set == USER_DEFINED_LEN_OFFSET) {
        table = appGUI->tst->kana_signs_set_14_idx;
    } else {
        table = kana_signs_set_tabs[config.kana_set];
    }

    /* fill questions and answers tables */
    for (i = 0; i < number_of_questions; i++) {
        gint kana_number;
        if (i >= appGUI->tst->kana_set_len[config.kana_set]) {
            kana_number = table[i - appGUI->tst->kana_set_len[config.kana_set]] + MIXED_SEPARATOR;
        } else {
            kana_number = table[i];
            if (config.kana_mode == KATAKANA) kana_number += MIXED_SEPARATOR;
        }
        appGUI->tst->questions_table[i] = appGUI->tst->kana_set_table[i] = kana_number;
    }

    /* do a decent random permutation */
    for(i = number_of_questions-1; i > 0; --i) {
        pos = _LRAND48(i+1);
        tmp = appGUI->tst->questions_table[i];
        appGUI->tst->questions_table[i] = appGUI->tst->questions_table[pos];
        appGUI->tst->questions_table[pos] = tmp;
    }
}

/*--------------------------------------------------------------------*/
/* kana_number is the global character ID */
void 
test_generate_choices(gint kana_number, GUI *appGUI) {
    gint i, tmp, pos;
    gint num_choices;
    gint num_answers;
    gint *table;
    gint answers_table[MAX_NUMBER_OF_SIGNS];
    gboolean done;
    
    table = appGUI->tst->kana_set_table;
    
    num_answers = appGUI->tst->max_entries_in_test;
    num_choices = num_answers;
    if (num_choices > KANA_MAX_CHOICES) {
        num_choices = KANA_MAX_CHOICES;
    }

    /* fill answers table */
    for (i = 0; i < num_answers; i++) {
        answers_table[i] = table[i];
    }

    /* do a decent random permutation */
    for(i = num_answers-1; i > 0; --i) {
        pos = _LRAND48(i+1);
        tmp = answers_table[i];
        answers_table[i] = answers_table[pos];
        answers_table[pos] = tmp;
    }
    
    done = FALSE;
    /* if target kana is in the answers, do nothing */
    for (i = 0; i < num_choices; i++) {
        if (answers_table[i] == kana_number) {
            done = TRUE;
            break;
        }
    }
    
    /* otherwise, insert correct answer */
    if (done == FALSE) {
        answers_table[_LRAND48(num_choices)] = kana_number;
    }
    
    /* render all buttons */
    for (i = 0; i < num_choices; i++) {
        gui_set_button_kana(i, answers_table[i], config.kana_mode, appGUI);
    }

    gui_disable_buttons(num_choices, appGUI);
}

/*--------------------------------------------------------------------*/

gchar*
test_sec2str (guint seconds, gboolean mshort) {

static gchar obuffer[BUFFER_SIZE];
guint minutes;

    minutes = seconds / 60;
    seconds = seconds % 60;

    if (mshort == TRUE) {
        g_snprintf (obuffer, BUFFER_SIZE, "%02d:%02d", minutes, seconds);
    } else {
        if (minutes > 0) {
            g_snprintf (obuffer, BUFFER_SIZE, "%d %s %d %s", 
                        minutes, ngettext ("minute and", "minutes and", minutes),
                        seconds, ngettext ("second", "seconds", seconds));
        } else {
            g_snprintf (obuffer, BUFFER_SIZE, "%d %s", seconds, ngettext ("second", "seconds", seconds));
        }
    }

    return obuffer;
}

/*--------------------------------------------------------------------*/

gboolean 
test_check_end(GUI *appGUI) {
    gint old_counter = appGUI->tst->question_counter;
    gboolean end = FALSE;

    while (!end) {

        appGUI->tst->question_counter = (appGUI->tst->question_counter + 1) % appGUI->tst->max_entries_in_test;    /* next question */

        if (config.repeat_mode == REPEAT_WRONG) {

            if (appGUI->tst->questions_table[appGUI->tst->question_counter] != -1) {
                break;          /* this has not yet been answered correctly */
            }

            if (old_counter == appGUI->tst->question_counter) {
                end = TRUE;     /* every question is answered correctly */
            }

        }  else if (0 == appGUI->tst->question_counter) { /* all questions ? */

            if (config.repeat_mode == REPEAT_ALL) {
                test_generate_tables (appGUI->tst->max_entries_in_test, appGUI);
            } else {
                end = TRUE;
            }

        }  else {
            break;
        }
    }
    return end;
}

/*--------------------------------------------------------------------*/
/* kana_number is the unique identifier of the character in this function. */
void 
test_update_answer_stats(gint kana_number, gboolean is_correct, GUI *appGUI) {
    if (config.kana_mode == HIRAGANA) {
        appGUI->sts->hiragana_counters[kana_number]++;
    } else if (config.kana_mode == KATAKANA) {
        appGUI->sts->katakana_counters[kana_number % MIXED_SEPARATOR]++;
    } else if (config.kana_mode == MIXED) {
        if (kana_number >= MIXED_SEPARATOR) {
            appGUI->sts->katakana_counters[kana_number - MIXED_SEPARATOR]++;
        } else {
            appGUI->sts->hiragana_counters[kana_number]++;
        }
    }
    
    if (is_correct) {
        if (config.kana_mode == HIRAGANA) {
            appGUI->sts->correct_hiragana_counters[kana_number]++;
        } else if (config.kana_mode == KATAKANA) {
            appGUI->sts->correct_katakana_counters[kana_number % MIXED_SEPARATOR]++;
        } else if (config.kana_mode == MIXED) {
            if (kana_number >= MIXED_SEPARATOR) {
                appGUI->sts->correct_katakana_counters[kana_number - MIXED_SEPARATOR]++;
            } else {
                appGUI->sts->correct_hiragana_counters[kana_number]++;
            }
        }
        appGUI->tst->right_answer_counter++;
    } else {
        appGUI->tst->wrong_answer_counter++;
    }
}

/*--------------------------------------------------------------------*/
/* selected_number is the unique identifier of the character user selected */
void 
test_check_choice (gint selected_number, GUI *appGUI) {
    gint kana_number = appGUI->tst->questions_table[appGUI->tst->question_counter];
    gboolean correct_answer;
    correct_answer = FALSE;

    if ((kana_number % MIXED_SEPARATOR) == (selected_number % MIXED_SEPARATOR)) {
        correct_answer = TRUE;
    }
    
    test_update_answer_stats(kana_number, correct_answer, appGUI);
    
    if (correct_answer == FALSE) {
        if (config.ca_timeout != TO_DISABLED) {
            gui_show_correct_answer(kana_number, appGUI);
            gtk_widget_set_sensitive (appGUI->stop_button, FALSE);
            gtk_widget_set_sensitive (appGUI->stop_button, TRUE);
        }
    } else {

        appGUI->tst->questions_table[appGUI->tst->question_counter] = -1;
    }
    if (test_check_end(appGUI) == TRUE) {
        test_info (appGUI); /* finish! */
    } else {
        gui_set_progress (appGUI);
        gui_display_kana_choices (appGUI->tst->questions_table[appGUI->tst->question_counter], config.kana_mode, appGUI);
    }
}

/*--------------------------------------------------------------------*/

void
test_check_answer (gchar *user_answer, GUI *appGUI) {
gint kana_number;
gboolean correct_answer;
gchar **r_answers;
gint n_answ;

    kana_number = appGUI->tst->questions_table[appGUI->tst->question_counter]; // unique identifier of the character
    r_answers = g_strsplit (get_kana_sign (kana_number % MIXED_SEPARATOR, ROMAJI, TRUE), ANSWER_DELIMITER, MAX_RANSWERS);
    n_answ = 0;
    correct_answer = FALSE;

    while (r_answers[n_answ]) {
        if (strcmp (r_answers[n_answ], user_answer)==0) {
            correct_answer = TRUE;
            break;
        }
        n_answ++;
    }

    g_strfreev (r_answers);

    test_update_answer_stats(kana_number, correct_answer, appGUI);

    if (correct_answer == FALSE) {

        /* WRONG ANSWER */

        if (config.ca_timeout != TO_DISABLED) {
            gtk_widget_set_sensitive (appGUI->romaji_entry, FALSE);
            gtk_widget_set_sensitive (appGUI->stop_button, FALSE);
        
            gui_show_correct_answer(appGUI->tst->questions_table[appGUI->tst->question_counter], appGUI);
            
            if (appGUI->tst->test_state == FALSE) return;

            gtk_widget_set_sensitive (appGUI->romaji_entry, TRUE);
            gtk_widget_set_sensitive (appGUI->stop_button, TRUE);

            gtk_widget_grab_focus (appGUI->romaji_entry);

        }
    } else {
        appGUI->tst->questions_table[appGUI->tst->question_counter] = -1;
    }

    if (test_check_end(appGUI) == TRUE) {
        test_info (appGUI); /* finish! */
    } else {
        gui_set_progress (appGUI);
        gui_next_text_question (appGUI->tst->questions_table[appGUI->tst->question_counter], config.kana_mode, appGUI);
    }
}

/*--------------------------------------------------------------------*/

void
test_info(GUI *appGUI) {

    GtkWidget *info_dialog;
    gchar   message[BUFFER_SIZE];
    struct tm   *timer;
    time_t      tmm;
    guint answer_counter = appGUI->tst->right_answer_counter + appGUI->tst->wrong_answer_counter;
    stats_entry *entry = NULL;

    if (appGUI->tst->test_state == FALSE) return;

    gui_disable_test (appGUI);

    if (answer_counter == 0) return;
#ifdef MAEMO
    if (config.repeat_mode == REPEAT_ALL) {

        g_snprintf (message, BUFFER_SIZE,
                    "%s\n\n"
                    "%s: %d (%d %s)\n"
                    "%s: %d\n%s: %d\n"
                    "\n%s: %.f %%\n"
                    "\n%s: %s\n",
                    _("Test has finished!"), _("Total questions"),
                    answer_counter,
                    appGUI->tst->max_entries_in_test,
                    _("unique"),
                    _("Correct answers"),
                    appGUI->tst->right_answer_counter,
                    _("Wrong answers"),
                    appGUI->tst->wrong_answer_counter,
                    _("Correctness ratio"),
                    (gfloat)(appGUI->tst->right_answer_counter) / answer_counter * 100.0,
                    _("Drilling-time"),
                    test_sec2str (appGUI->time_counter, FALSE));
    } else {

        g_snprintf (message, BUFFER_SIZE,
                    "%s\n\n"
                    "%s: %d\n"
                    "%s: %d\n%s: %d\n"
                    "\n%s: %.f %%\n"
                    "\n%s: %s\n",
                    _("Test has finished!"), _("Total questions"),
                    answer_counter,
                    _("Correct answers"),
                    appGUI->tst->right_answer_counter,
                    _("Wrong answers"),
                    appGUI->tst->wrong_answer_counter,
                    _("Correctness ratio"),
                    (gfloat)(appGUI->tst->right_answer_counter) / answer_counter * 100.0,
                    _("Drilling-time"),
                    test_sec2str (appGUI->time_counter, FALSE));
    }
    
   info_dialog = hildon_note_new_information  (GTK_WINDOW(appGUI->main_window), message);  
#else
    if (config.repeat_mode == REPEAT_ALL) {

        g_snprintf (message, BUFFER_SIZE,
                    "<span font_desc='16'><b>%s</b></span>\n\n"
                    "<span font_desc='12'>%s: %d (%d %s)\n"
                    "%s: %d\n%s: %d\n"
                    "\n%s: %.f %%\n"
                    "\n%s: %s\n</span>",
                    _("Test has finished!"), _("Total questions"),
                    answer_counter,
                    appGUI->tst->max_entries_in_test,
                    _("unique"),
                    _("Correct answers"),
                    appGUI->tst->right_answer_counter,
                    _("Wrong answers"),
                    appGUI->tst->wrong_answer_counter,
                    _("Correctness ratio"),
                    (gfloat)(appGUI->tst->right_answer_counter) / answer_counter * 100.0,
                    _("Drilling-time"),
                    test_sec2str (appGUI->time_counter, FALSE));
    } else {

        g_snprintf (message, BUFFER_SIZE,
                    "<span font_desc='16'><b>%s</b></span>\n\n"
                    "<span font_desc='12'>%s: %d\t\t\t\n"
                    "%s: %d\n%s: %d\n"
                    "\n%s: %.f %%\n"
                    "\n%s: %s\n</span>",
                    _("Test has finished!"), _("Total questions"),
                    answer_counter,
                    _("Correct answers"),
                    appGUI->tst->right_answer_counter,
                    _("Wrong answers"),
                    appGUI->tst->wrong_answer_counter,
                    _("Correctness ratio"),
                    (gfloat)(appGUI->tst->right_answer_counter) / answer_counter * 100.0,
                    _("Drilling-time"),
                    test_sec2str (appGUI->time_counter, FALSE));
    }

    info_dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(appGUI->main_window),
                                                      GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                      GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, message, NULL);

    gtk_window_set_title (GTK_WINDOW(info_dialog), _("Information"));
    gtk_window_set_position (GTK_WINDOW(info_dialog), GTK_WIN_POS_MOUSE);
    gtk_widget_show (info_dialog);
#endif
    gtk_dialog_run (GTK_DIALOG(info_dialog));
    gtk_widget_destroy (info_dialog);

    entry = g_new0 (stats_entry, 1);

    if (entry != NULL) {

        tmm = time (NULL);
        timer = localtime (&tmm);

        entry->date_day = timer->tm_mday;
        entry->date_month = timer->tm_mon;
        entry->date_year = timer->tm_year+1900;
        entry->date_hour = timer->tm_hour;
        entry->date_minute = timer->tm_min;
        entry->test_time = appGUI->time_counter;
        entry->test_type = config.test_mode;
        entry->test_mode = config.kana_mode;
        entry->test_kana_set = config.kana_set;
        entry->test_questions = answer_counter;
        entry->test_correct_answers = appGUI->tst->right_answer_counter;
        entry->test_repeat_mode = config.repeat_mode;

        stats_add_entry (entry, appGUI);
        stats_write_list (STATS_FILENAME, CONFIG_DIRNAME, appGUI);
    }
}

/*--------------------------------------------------------------------*/


