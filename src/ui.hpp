#include <Arduino.h>
#include <lvgl.h>

#include "conf.h"

LV_FONT_DECLARE(BakBakOne192);
LV_FONT_DECLARE(OswaldSemiBold64);
LV_FONT_DECLARE(OswaldSemiBold72);
LV_FONT_DECLARE(RussoOne36);

LV_IMAGE_DECLARE(bootscreen);
LV_IMAGE_DECLARE(bg);

// Boot screen objects
lv_obj_t *version_label;

// Main labels
lv_obj_t *top_label;

lv_obj_t *speed_label;

lv_obj_t *clock_label;
lv_obj_t *laps_label;
lv_obj_t *laptime_label;
lv_obj_t *en_eff_label;

lv_obj_t *battery_bar;
lv_obj_t *battery_voltage;
lv_obj_t *battery_percentage;
lv_obj_t *battery_status;

lv_obj_t *notification_box;
lv_obj_t *notification_label;

typedef enum 
{
    NOTIFY_WARNING,
    NOTIFY_INFO,
    NOTIFY_PRAISE
} notification_level;

bool notification_displayed = false;

void ui_bootscreen(lv_obj_t *scr)
{
    lv_obj_set_style_bg_image_src(scr, &bootscreen, LV_PART_MAIN);
    
    version_label = lv_label_create(scr);
    lv_obj_align(version_label, LV_ALIGN_CENTER, 0, 160);
    lv_obj_set_style_text_align(version_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_long_mode(version_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_font(version_label, &RussoOne36, LV_PART_MAIN);
    lv_obj_set_style_text_color(version_label, lv_color_hex(0x808080), LV_PART_MAIN);
    lv_label_set_text_fmt(version_label, "%s\nLength: %dm\nOptimal: %dm%02ds", tracks[currentTrack].name, tracks[currentTrack].trackLength, tracks[currentTrack].optimalLapTime / 60, tracks[currentTrack].optimalLapTime % 60);
}

void ui_create(lv_obj_t *scr)
{
    // Clean up boot screen
    if (version_label)
        lv_obj_delete(version_label);

    lv_obj_set_style_bg_image_src(scr, &bg, LV_PART_MAIN);

    top_label = lv_label_create(scr);
    lv_obj_align(top_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_label_set_recolor(top_label, true);
    lv_label_set_text(top_label, "");

    speed_label = lv_label_create(scr);
    lv_obj_align(speed_label, LV_ALIGN_RIGHT_MID, -105, -160);
    lv_obj_set_style_text_font(speed_label, &BakBakOne192, LV_PART_MAIN);
    lv_label_set_text(speed_label, "56");
    lv_obj_set_style_text_color(speed_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    clock_label = lv_label_create(scr);
    lv_obj_align(clock_label, LV_ALIGN_CENTER, -80, 0);
    lv_obj_set_style_text_font(clock_label, &OswaldSemiBold72, LV_PART_MAIN);
    lv_label_set_text(clock_label, "23:01");
    lv_obj_set_style_text_color(clock_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    laps_label = lv_label_create(scr);
    lv_obj_align(laps_label, LV_ALIGN_CENTER, 80, 0);
    lv_obj_set_style_text_font(laps_label, &OswaldSemiBold72, LV_PART_MAIN);
    lv_label_set_text(laps_label, "280");
    lv_obj_set_style_text_color(laps_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    laptime_label = lv_label_create(scr);
    lv_obj_align(laptime_label, LV_ALIGN_CENTER, -80, 120);
    lv_obj_set_style_text_font(laptime_label, &OswaldSemiBold72, LV_PART_MAIN);
    lv_label_set_text(laptime_label, "2:00");
    lv_obj_set_style_text_color(laptime_label, lv_color_hex(0x00FF00), LV_PART_MAIN);

    en_eff_label = lv_label_create(scr);
    lv_obj_align(en_eff_label, LV_ALIGN_CENTER, 80, 120);
    lv_obj_set_style_text_font(en_eff_label, &OswaldSemiBold64, LV_PART_MAIN);
    lv_label_set_text(en_eff_label, "GOOD");
    lv_obj_set_style_text_color(en_eff_label, lv_color_hex(0x00FF00), LV_PART_MAIN);

    battery_bar = lv_bar_create(scr);
    lv_obj_set_size(battery_bar, 300, 60);
    lv_obj_align(battery_bar, LV_ALIGN_TOP_LEFT, 10, 410);
    lv_obj_set_style_radius(battery_bar, 10, LV_PART_MAIN);
    lv_obj_set_style_radius(battery_bar, 10, LV_PART_INDICATOR);
    lv_bar_set_orientation(battery_bar, LV_BAR_ORIENTATION_HORIZONTAL);
    lv_bar_set_range(battery_bar, 0, 100);
    lv_bar_set_value(battery_bar, 50, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(battery_bar, lv_color_hex(((uint32_t)map(lv_bar_get_value(battery_bar), 0, 100, 255, 0) << 16) | ((uint32_t)map(lv_bar_get_value(battery_bar), 0, 100, 0, 255) << 8)), LV_PART_INDICATOR);

    battery_voltage = lv_label_create(battery_bar);
    lv_obj_align(battery_voltage, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_text_font(battery_voltage, &RussoOne36, LV_PART_MAIN);
    lv_label_set_text(battery_voltage, "48.2V");
    lv_obj_set_style_text_color(battery_voltage, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    battery_percentage = lv_label_create(battery_bar);
    lv_obj_align(battery_percentage, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_text_font(battery_percentage, &RussoOne36, LV_PART_MAIN);
    lv_label_set_text(battery_percentage, "100%");
    lv_obj_set_style_text_color(battery_percentage, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    battery_status = lv_label_create(battery_bar);
    lv_obj_align(battery_status, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(battery_status, &RussoOne36, LV_PART_MAIN);
    lv_label_set_text(battery_status, "DISCONNECTED");
    lv_obj_set_style_text_color(battery_status, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_add_flag(battery_status, LV_OBJ_FLAG_HIDDEN);

    notification_box = lv_obj_create(scr);
    lv_obj_set_align(notification_box, LV_ALIGN_CENTER);
    lv_obj_set_size(notification_box, 320, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(notification_box, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(notification_box, LV_OPA_90, LV_PART_MAIN);
    lv_obj_remove_flag(notification_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(notification_box, LV_OBJ_FLAG_HIDDEN);

    notification_label = lv_label_create(notification_box);
    lv_label_set_long_mode(notification_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_align(notification_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_align(notification_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(notification_label, &OswaldSemiBold72, LV_PART_MAIN);
    lv_label_set_text(notification_label, "MOTOR HOT");
    lv_obj_set_width(notification_label, 320);
    lv_obj_set_style_text_color(notification_label, lv_color_hex(0xFF0000), LV_PART_MAIN);
}

void ui_notify(bool show, notification_level level = NOTIFY_INFO, char *text = (char *)"")
{
    if (!show)
    {
        lv_obj_add_flag(notification_box, LV_OBJ_FLAG_HIDDEN);

        notification_displayed = false;

        return;
    }
    else
    {
        switch (level)
        {
            case NOTIFY_INFO:
                lv_obj_set_style_text_color(notification_label, lv_color_white(), LV_PART_MAIN);
                break;
            case NOTIFY_PRAISE:
                lv_obj_set_style_text_color(notification_label, lv_color_hex(0x00FF00), LV_PART_MAIN);
                break;
            case NOTIFY_WARNING:
                lv_obj_set_style_text_color(notification_label, lv_color_hex(0xFF0000), LV_PART_MAIN);
                break;
            default:
                break;
        }
        
        lv_label_set_text_fmt(notification_label, "%s", text);
    }

    if (!notification_displayed)
    {
        lv_obj_remove_flag(notification_box, LV_OBJ_FLAG_HIDDEN);

        notification_displayed = true;
    }
}

void ui_set_battery(float voltage)
{
    if (!vesc_connected)
    {
        lv_obj_add_flag(battery_voltage, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_percentage, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(battery_status, LV_OBJ_FLAG_HIDDEN);

        lv_bar_set_value(battery_bar, 100, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(battery_bar, lv_color_hex(0xFF0000), LV_PART_INDICATOR);

        return;
    }

    lv_obj_remove_flag(battery_voltage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(battery_percentage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(battery_status, LV_OBJ_FLAG_HIDDEN);

    lv_bar_set_value(battery_bar, static_cast<int>((voltage - 46.0f) * 100.0f / (50.4f - 46.0f)), LV_ANIM_OFF);
    lv_obj_set_style_bg_color(battery_bar, lv_color_hex(((uint32_t)map(lv_bar_get_value(battery_bar), 0, 100, 255, 0) << 16) | ((uint32_t)map(lv_bar_get_value(battery_bar), 0, 100, 0, 255) << 8)), LV_PART_INDICATOR);

    unsigned int volts = static_cast<unsigned int>(voltage);
    unsigned int tenths = static_cast<unsigned int>((voltage - volts) * 10.0f + 0.5f);

    if (tenths >= 10)
    {
        ++volts;
        tenths = 0;
    }

    lv_label_set_text_fmt(battery_voltage, "%u.%uV", volts, tenths);

    lv_label_set_text_fmt(battery_percentage, "%d%%", lv_bar_get_value(battery_bar));
}
