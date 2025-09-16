#pragma once
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TRANS_RGB_TO_CMY = 0,
    TRANS_CMY_TO_RGB,
    TRANS_CMY_TO_CMYK,
    TRANS_CMYK_TO_CMY,
    TRANS_RGB_TO_YIQ,
    TRANS_YIQ_TO_RGB,
    TRANS_RGB_TO_HSI,
    TRANS_HSI_TO_RGB,
    TRANS_RGB_TO_HSV,
    TRANS_HSV_TO_RGB,
    TRANS_COUNT
} TransformType;

typedef enum {
    DISPLAY_GRAY = 0,
    DISPLAY_TINT,
    DISPLAY_PSEUDOCOLOR
} DisplayMode;

void on_app_activate(GtkApplication *app, gpointer user_data);

#ifdef __cplusplus
}
#endif
