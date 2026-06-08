/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "theme.h"

namespace view {

ThemePalette palette(bool dark_mode) {
    if (dark_mode) {
        return {
            lv_color_hex(0x101214),
            lv_color_hex(0x15181c),
            lv_color_hex(0x181b1f),
            lv_color_hex(0x2a2f35),
            lv_color_hex(0x30363d),
            lv_color_hex(0xf4f4f5),
        };
    }

    return {
        lv_color_hex(0xf8f9fa),
        lv_color_hex(0xffffff),
        lv_color_hex(0xf0f0f0),
        lv_color_hex(0xe7e7e7),
        lv_color_hex(0xd0d7de),
        lv_color_hex(0x1f2328),
    };
}

void apply_lvgl_theme(lv_display_t* display, bool dark_mode) {
    if (!display) {
        return;
    }

    lv_theme_t* theme = lv_theme_default_init(display,
                                              lv_color_hex(0x3a7afe),
                                              lv_color_hex(0x5f6b7a),
                                              dark_mode,
                                              LV_FONT_DEFAULT);
    lv_display_set_theme(display, theme);
}

} // namespace view
