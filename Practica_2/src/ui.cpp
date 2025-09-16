#include "ui.hpp"
#include "image_utils.hpp"
#include "transform.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

typedef struct {
    GtkWidget *window;
    GtkWidget *combo;
    GtkWidget *mode_combo;
    GtkWidget *file_label;
    GtkWidget *orig_image;
    GtkWidget *steps_view;
    GtkWidget *channels_scroller;
    GtkWidget *channels_box; // vbox with sections
    GdkPixbuf *loaded_pixbuf;
    GdkPixbuf *preview_pixbuf;
    std::string filename;
    int tile_size_px;
    int last_alloc_w;
    DisplayMode mode;
} AppState;

static const char* kTransformNames[] = {
    "1. RGB → CMY",
    "2. CMY → RGB",
    "3. CMY → CMYK",
    "4. CMYK → CMY",
    "5. RGB → YIQ",
    "6. YIQ → RGB",
    "7. RGB → HSI",
    "8. HSI → RGB",
    "9. RGB → HSV",
    "10. HSV → RGB"
};

// Forward
static void render_channels(AppState *st);

static void clear_children(GtkWidget *container) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *l = children; l != NULL; l = l->next) {
        gtk_widget_destroy(GTK_WIDGET(l->data));
    }
    g_list_free(children);
}

static GdkPixbuf* scale_pixbuf_fit(GdkPixbuf* src, int max_w, int max_h) {
    if (!src) return NULL;
    int sw = gdk_pixbuf_get_width(src);
    int sh = gdk_pixbuf_get_height(src);
    if (sw <= 0 || sh <= 0) return NULL;
    double rx = (double)max_w / (double)sw;
    double ry = (double)max_h / (double)sh;
    double r = rx < ry ? rx : ry;
    if (r >= 1.0) r = 1.0; // don't upscale
    int tw = (int)(sw * r);
    int th = (int)(sh * r);
    if (tw < 1) tw = 1;
    if (th < 1) th = 1;
    return gdk_pixbuf_scale_simple(src, tw, th, GDK_INTERP_BILINEAR);
}

static GdkPixbuf* hue_plane_to_pixbuf(const ImageGray &h_plane){
    ImageHSV tmp; tmp.width=h_plane.width; tmp.height=h_plane.height;
    tmp.h = h_plane;
    tmp.s.width=tmp.v.width=h_plane.width; tmp.s.height=tmp.v.height=h_plane.height;
    tmp.s.data.assign(h_plane.width*h_plane.height, 1.0f);
    tmp.v.data.assign(h_plane.width*h_plane.height, 1.0f);
    ImageRGB rgb = hsv_to_rgb(tmp);
    return rgb_to_pixbuf(rgb);
}

static GtkWidget* make_section_box(const char* title){
    GtkWidget *v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *hdr = gtk_label_new(NULL);
    gchar *markup = g_markup_printf_escaped("<b>%s</b>", title);
    gtk_label_set_markup(GTK_LABEL(hdr), markup);
    g_free(markup);
    gtk_box_pack_start(GTK_BOX(v), hdr, FALSE, FALSE, 2);
    return v;
}
static GtkWidget* make_grid(){
    GtkWidget *g = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(g), 8);
    gtk_grid_set_column_spacing(GTK_GRID(g), 8);
    return g;
}

static GtkWidget* make_channel_tile(AppState* st, const char* caption, const ImageGray &plane, bool isHue=false){
    GdkPixbuf *pb = NULL;
    if (isHue){
        if (st->mode == DISPLAY_PSEUDOCOLOR) pb = pseudocolor_gray_to_pixbuf(plane);
        else pb = hue_plane_to_pixbuf(plane);
    } else {
        if (st->mode == DISPLAY_TINT){
            float rt=0, gt=0, bt=0;
            if (caption[0]=='R') rt=1;
            else if (caption[0]=='G') gt=1;
            else if (caption[0]=='B') bt=1;
            else if (caption[0]=='C') {gt=1; bt=1;}
            else if (caption[0]=='M') {rt=1; bt=1;}
            else if (caption[0]=='Y') {rt=1; gt=1;}
            else { rt=gt=bt=1; } // default gray
            pb = tint_gray_to_pixbuf(plane, rt, gt, bt);
        } else if (st->mode == DISPLAY_PSEUDOCOLOR){
            pb = pseudocolor_gray_to_pixbuf(plane);
        } else {
            pb = gray_to_pixbuf(plane);
        }
    }
    int ts = st->tile_size_px > 0 ? st->tile_size_px : 220;
    GdkPixbuf *scaled = scale_pixbuf_fit(pb, ts, ts);
    GtkWidget *img = gtk_image_new_from_pixbuf(scaled ? scaled : pb);
    if (scaled) g_object_unref(scaled);
    g_object_unref(pb);
    GtkWidget *lbl = gtk_label_new(caption);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start(GTK_BOX(box), img, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), lbl, FALSE, FALSE, 0);
    return box;
}

static void fill_steps_text(AppState* st, TransformType t){
    GtkTextBuffer* buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(st->steps_view));
    gtk_text_buffer_set_text(buf, "", -1);
    const char* txt = "";
    switch(t){
        case TRANS_RGB_TO_CMY:
            txt =
"RGB → CMY\n"
"• Normaliza R,G,B a [0,1]\n"
"• C = 1 - R\n"
"• M = 1 - G\n"
"• Y = 1 - B\n";
            break;
        case TRANS_CMY_TO_RGB:
            txt =
"CMY → RGB\n"
"• R = 1 - C\n"
"• G = 1 - M\n"
"• B = 1 - Y\n";
            break;
        case TRANS_CMY_TO_CMYK:
            txt =
"CMY → CMYK\n"
"• K = min(C, M, Y)\n"
"• Si K≈1 ⇒ C'=M'=Y'=0\n"
"• Si no: C'=(C-K)/(1-K), M'=(M-K)/(1-K), Y'=(Y-K)/(1-K)\n";
            break;
        case TRANS_CMYK_TO_CMY:
            txt =
"CMYK → CMY\n"
"• C = C'*(1-K) + K\n"
"• M = M'*(1-K) + K\n"
"• Y = Y'*(1-K) + K\n";
            break;
        case TRANS_RGB_TO_YIQ:
            txt =
"RGB → YIQ\n"
"[Y I Q]^T = M * [R G B]^T\n"
"M = [[0.299, 0.587, 0.114],\n"
"     [0.596,-0.274,-0.322],\n"
"     [0.211,-0.523, 0.312]]\n";
            break;
        case TRANS_YIQ_TO_RGB:
            txt =
"YIQ → RGB\n"
"[R G B]^T = M * [Y I Q]^T\n"
"M = [[1.0,  0.956,  0.621],\n"
"     [1.0, -0.272, -0.647],\n"
"     [1.0, -1.106,  1.703]]\n"
"• Recortar R,G,B a [0,1]\n";
            break;
        case TRANS_RGB_TO_HSI:
            txt =
"RGB → HSI\n"
"• I = (R+G+B)/3\n"
"• S = 1 - min(R,G,B)/I (si I>0)\n"
"• H = acos( 0.5((R-G)+(R-B)) / sqrt((R-G)^2 + (R-B)(G-B)) ); ajustar cuadrante\n";
            break;
        case TRANS_HSI_TO_RGB:
            txt =
"HSI → RGB\n"
"• Tres sectores de H (0–120, 120–240, 240–360)\n"
"• Aplicar fórmulas por sector con S e I\n";
            break;
        case TRANS_RGB_TO_HSV:
            txt =
"RGB → HSV\n"
"• Cmax=max(R,G,B), Cmin=min(R,G,B), Δ=Cmax-Cmin\n"
"• H según componente de Cmax y Δ\n"
"• S=0 si Cmax=0; en otro caso S=Δ/Cmax\n"
"• V=Cmax\n";
            break;
        case TRANS_HSV_TO_RGB:
            txt =
"HSV → RGB\n"
"• C=V*S, H'=H/60, X=C*(1 - |(H' mod 2) - 1|), m=V-C\n"
"• Asignar (R,G,B) según el sextante de H'\n"
"• (R,G,B) = (R+m, G+m, B+m)\n";
            break;
        default: break;
    }
    gtk_text_buffer_set_text(buf, txt, -1);
}

static void on_transform_changed(GtkComboBox *combo, gpointer user_data) {
    AppState *st = (AppState*)user_data;
    fill_steps_text(st, (TransformType)gtk_combo_box_get_active(GTK_COMBO_BOX(st->combo)));
    render_channels(st);
}

static void on_mode_changed(GtkComboBox *combo, gpointer user_data){
    AppState *st = (AppState*)user_data;
    st->mode = (DisplayMode)gtk_combo_box_get_active(GTK_COMBO_BOX(st->mode_combo));
    render_channels(st);
}

static void on_channels_size_allocate(GtkWidget* widget, GdkRectangle* alloc, gpointer user_data){
    AppState *st = (AppState*)user_data;
    int gap = 8;
    int usable_w = alloc->width - 16;
    int cols = 3;
    if (usable_w < 700) cols = 2;
    else if (usable_w < 1000) cols = 3;
    else if (usable_w < 1400) cols = 4;
    else cols = 5;
    int tile = (usable_w - (cols - 1) * gap) / cols;
    if (tile < 120) tile = 120;
    if (tile > 360) tile = 360;

    if (st->tile_size_px != tile || st->last_alloc_w != alloc->width){
        st->tile_size_px = tile;
        st->last_alloc_w = alloc->width;
        render_channels(st);
    }
}

static void render_channels(AppState *st) {
    if (!st->loaded_pixbuf) return;
    clear_children(st->channels_box);

    // Selected transform
    TransformType t = (TransformType)gtk_combo_box_get_active(GTK_COMBO_BOX(st->combo));

    // Source as RGB planes
    ImageRGB rgb = pixbuf_to_rgb(st->loaded_pixbuf);

    // Build sections
    GtkWidget *sec_origin = make_section_box("Origen");
    GtkWidget *grid_o = make_grid();
    GtkWidget *sec_mid = make_section_box("Intermedios");
    GtkWidget *grid_m = make_grid();
    GtkWidget *sec_dest = make_section_box("Destino");
    GtkWidget *grid_d = make_grid();

    auto pack = [&](GtkWidget* grid, const std::vector<GtkWidget*>& vec){
        GtkAllocation alloc; gtk_widget_get_allocation(st->channels_scroller, &alloc);
        int usable_w = (alloc.width > 0 ? alloc.width : 1000) - 16;
        int cols = 3;
        if (usable_w < 700) cols = 2;
        else if (usable_w < 1000) cols = 3;
        else if (usable_w < 1400) cols = 4;
        else cols = 5;
        for (size_t i=0;i<vec.size();++i){
            int row = (int)(i / cols);
            int col = (int)(i % cols);
            gtk_grid_attach(GTK_GRID(grid), vec[i], col, row, 1, 1);
        }
    };

    std::vector<GtkWidget*> tiles_o, tiles_m, tiles_d;

    switch (t) {
        case TRANS_RGB_TO_CMY:
        case TRANS_RGB_TO_YIQ:
        case TRANS_RGB_TO_HSI:
        case TRANS_RGB_TO_HSV: {
            tiles_o.push_back(make_channel_tile(st, "R (origen)", rgb.r, false));
            tiles_o.push_back(make_channel_tile(st, "G (origen)", rgb.g, false));
            tiles_o.push_back(make_channel_tile(st, "B (origen)", rgb.b, false));
        } break;
        default: break;
    }

    if (t == TRANS_RGB_TO_CMY) {
        ImageCMY cmy = rgb_to_cmy(rgb);
        tiles_d.push_back(make_channel_tile(st, "C (destino)", cmy.c, false));
        tiles_d.push_back(make_channel_tile(st, "M (destino)", cmy.m, false));
        tiles_d.push_back(make_channel_tile(st, "Y (destino)", cmy.y, false));
    } else if (t == TRANS_CMY_TO_RGB) {
        ImageCMY cmy = rgb_to_cmy(rgb);
        ImageRGB out = cmy_to_rgb(cmy);
        tiles_o.push_back(make_channel_tile(st, "C (origen)", cmy.c, false));
        tiles_o.push_back(make_channel_tile(st, "M (origen)", cmy.m, false));
        tiles_o.push_back(make_channel_tile(st, "Y (origen)", cmy.y, false));
        tiles_d.push_back(make_channel_tile(st, "R (destino)", out.r, false));
        tiles_d.push_back(make_channel_tile(st, "G (destino)", out.g, false));
        tiles_d.push_back(make_channel_tile(st, "B (destino)", out.b, false));
    } else if (t == TRANS_CMY_TO_CMYK) {
        ImageCMY cmy = rgb_to_cmy(rgb);
        ImageCMYK cmyk = cmy_to_cmyk(cmy);
        tiles_o.push_back(make_channel_tile(st, "C (origen)", cmy.c, false));
        tiles_o.push_back(make_channel_tile(st, "M (origen)", cmy.m, false));
        tiles_o.push_back(make_channel_tile(st, "Y (origen)", cmy.y, false));
        tiles_m.push_back(make_channel_tile(st, "K=min(C,M,Y)", cmyk.k, false));
        tiles_d.push_back(make_channel_tile(st, "C' (destino)", cmyk.c, false));
        tiles_d.push_back(make_channel_tile(st, "M' (destino)", cmyk.m, false));
        tiles_d.push_back(make_channel_tile(st, "Y' (destino)", cmyk.y, false));
        tiles_d.push_back(make_channel_tile(st, "K (destino)", cmyk.k, false));
    } else if (t == TRANS_CMYK_TO_CMY) {
        ImageCMY cmy = rgb_to_cmy(rgb);
        ImageCMYK cmyk = cmy_to_cmyk(cmy);
        ImageCMY back = cmyk_to_cmy(cmyk);
        tiles_o.push_back(make_channel_tile(st, "C' (origen)", cmyk.c, false));
        tiles_o.push_back(make_channel_tile(st, "M' (origen)", cmyk.m, false));
        tiles_o.push_back(make_channel_tile(st, "Y' (origen)", cmyk.y, false));
        tiles_o.push_back(make_channel_tile(st, "K (origen)", cmyk.k, false));
        tiles_d.push_back(make_channel_tile(st, "C (destino)", back.c, false));
        tiles_d.push_back(make_channel_tile(st, "M (destino)", back.m, false));
        tiles_d.push_back(make_channel_tile(st, "Y (destino)", back.y, false));
    } else if (t == TRANS_RGB_TO_YIQ) {
        ImageYIQ yiq = rgb_to_yiq(rgb);
        tiles_d.push_back(make_channel_tile(st, "Y (destino)", yiq.y, false));
        tiles_d.push_back(make_channel_tile(st, "I (destino)", yiq.i, false));
        tiles_d.push_back(make_channel_tile(st, "Q (destino)", yiq.q, false));
    } else if (t == TRANS_YIQ_TO_RGB) {
        ImageYIQ yiq = rgb_to_yiq(rgb);
        ImageRGB out = yiq_to_rgb(yiq);
        tiles_o.push_back(make_channel_tile(st, "Y (origen)", yiq.y, false));
        tiles_o.push_back(make_channel_tile(st, "I (origen)", yiq.i, false));
        tiles_o.push_back(make_channel_tile(st, "Q (origen)", yiq.q, false));
        tiles_d.push_back(make_channel_tile(st, "R (destino)", out.r, false));
        tiles_d.push_back(make_channel_tile(st, "G (destino)", out.g, false));
        tiles_d.push_back(make_channel_tile(st, "B (destino)", out.b, false));
    } else if (t == TRANS_RGB_TO_HSI) {
        ImageHSI hsi = rgb_to_hsi(rgb);
        ImageGray minrgb; minrgb.width=rgb.width; minrgb.height=rgb.height; minrgb.data.resize(rgb.width*rgb.height);
        for (int i=0,n=rgb.width*rgb.height;i<n;++i){
            float m = std::min(rgb.r.data[i], std::min(rgb.g.data[i], rgb.b.data[i]));
            minrgb.data[i]=m;
        }
        tiles_m.push_back(make_channel_tile(st, "min(R,G,B)", minrgb, false));
        tiles_d.push_back(make_channel_tile(st, "H (destino)", hsi.h, true));
        tiles_d.push_back(make_channel_tile(st, "S (destino)", hsi.s, false));
        tiles_d.push_back(make_channel_tile(st, "I (destino)", hsi.i, false));
    } else if (t == TRANS_HSI_TO_RGB) {
        ImageHSI hsi = rgb_to_hsi(rgb);
        ImageRGB out = hsi_to_rgb(hsi);
        tiles_o.push_back(make_channel_tile(st, "H (origen)", hsi.h, true));
        tiles_o.push_back(make_channel_tile(st, "S (origen)", hsi.s, false));
        tiles_o.push_back(make_channel_tile(st, "I (origen)", hsi.i, false));
        tiles_d.push_back(make_channel_tile(st, "R (destino)", out.r, false));
        tiles_d.push_back(make_channel_tile(st, "G (destino)", out.g, false));
        tiles_d.push_back(make_channel_tile(st, "B (destino)", out.b, false));
    } else if (t == TRANS_RGB_TO_HSV) {
        ImageHSV hsv = rgb_to_hsv(rgb);
        ImageGray cmax{rgb.width, rgb.height, std::vector<float>(rgb.width*rgb.height)};
        ImageGray cmin{rgb.width, rgb.height, std::vector<float>(rgb.width*rgb.height)};
        ImageGray delta{rgb.width, rgb.height, std::vector<float>(rgb.width*rgb.height)};
        for (int i=0,n=rgb.width*rgb.height;i<n;++i){
            float mx = std::max({rgb.r.data[i], rgb.g.data[i], rgb.b.data[i]});
            float mn = std::min({rgb.r.data[i], rgb.g.data[i], rgb.b.data[i]});
            cmax.data[i]=mx; cmin.data[i]=mn; delta.data[i]=mx-mn;
        }
        tiles_m.push_back(make_channel_tile(st, "Cmax", cmax, false));
        tiles_m.push_back(make_channel_tile(st, "Cmin", cmin, false));
        tiles_m.push_back(make_channel_tile(st, "Δ", delta, false));
        tiles_d.push_back(make_channel_tile(st, "H (destino)", hsv.h, true));
        tiles_d.push_back(make_channel_tile(st, "S (destino)", hsv.s, false));
        tiles_d.push_back(make_channel_tile(st, "V (destino)", hsv.v, false));
    } else if (t == TRANS_HSV_TO_RGB) {
        ImageHSV hsv = rgb_to_hsv(rgb);
        ImageRGB out = hsv_to_rgb(hsv);
        tiles_o.push_back(make_channel_tile(st, "H (origen)", hsv.h, true));
        tiles_o.push_back(make_channel_tile(st, "S (origen)", hsv.s, false));
        tiles_o.push_back(make_channel_tile(st, "V (origen)", hsv.v, false));
        tiles_d.push_back(make_channel_tile(st, "R (destino)", out.r, false));
        tiles_d.push_back(make_channel_tile(st, "G (destino)", out.g, false));
        tiles_d.push_back(make_channel_tile(st, "B (destino)", out.b, false));
    }

    if (!tiles_o.empty()){ pack(grid_o, tiles_o); gtk_box_pack_start(GTK_BOX(st->channels_box), sec_origin, FALSE, FALSE, 2); gtk_box_pack_start(GTK_BOX(sec_origin), grid_o, FALSE, FALSE, 0); }
    if (!tiles_m.empty()){ pack(grid_m, tiles_m); gtk_box_pack_start(GTK_BOX(st->channels_box), sec_mid, FALSE, FALSE, 2); gtk_box_pack_start(GTK_BOX(sec_mid), grid_m, FALSE, FALSE, 0); }
    if (!tiles_d.empty()){ pack(grid_d, tiles_d); gtk_box_pack_start(GTK_BOX(st->channels_box), sec_dest, FALSE, FALSE, 2); gtk_box_pack_start(GTK_BOX(sec_dest), grid_d, FALSE, FALSE, 0); }

    gtk_widget_show_all(st->channels_box);
}

static void on_open_image(GtkButton *btn, gpointer user_data) {
    AppState *st = (AppState*)user_data;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Abrir imagen",
                        GTK_WINDOW(st->window),
                        GTK_FILE_CHOOSER_ACTION_OPEN,
                        "_Cancelar", GTK_RESPONSE_CANCEL,
                        "_Abrir", GTK_RESPONSE_ACCEPT,
                        NULL);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name(filter, "Imágenes");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (st->loaded_pixbuf) g_object_unref(st->loaded_pixbuf);
        GError *err = NULL;
        st->loaded_pixbuf = gdk_pixbuf_new_from_file(filename, &err);
        if (!st->loaded_pixbuf) {
            g_printerr("Error cargando imagen: %s\n", err ? err->message : "desconocido");
            if (err) g_error_free(err);
        } else {
            st->filename = filename;
            gtk_label_set_text(GTK_LABEL(st->file_label), st->filename.c_str());
            if (st->preview_pixbuf) { g_object_unref(st->preview_pixbuf); st->preview_pixbuf = NULL; }
            GdkPixbuf* scaled = scale_pixbuf_fit(st->loaded_pixbuf, 260, 260);
            st->preview_pixbuf = scaled ? scaled : (GdkPixbuf*)g_object_ref(st->loaded_pixbuf);
            gtk_image_set_from_pixbuf(GTK_IMAGE(st->orig_image), st->preview_pixbuf);
            render_channels(st);
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void on_app_activate(GtkApplication *app, gpointer user_data) {
    AppState *st = g_new0(AppState, 1);

    st->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(st->window), "Transformaciones de Color");
    gtk_window_set_default_size(GTK_WINDOW(st->window), 1200, 800);

    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(st->window), paned);

    // Left panel (1/4 width)
    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_hexpand(left, FALSE);
    gtk_widget_set_vexpand(left, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(left), 8);

    GtkWidget *btn_open = gtk_button_new_with_label("Cargar imagen…");
    g_signal_connect(btn_open, "clicked", G_CALLBACK(on_open_image), st);

    st->file_label = gtk_label_new("(sin archivo)");
    gtk_label_set_xalign(GTK_LABEL(st->file_label), 0.0);

    st->orig_image = gtk_image_new();

    st->combo = gtk_combo_box_text_new();
    for (int i = 0; i < TRANS_COUNT; ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(st->combo), kTransformNames[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(st->combo), 0);
    g_signal_connect(st->combo, "changed", G_CALLBACK(on_transform_changed), st);

    GtkWidget *mode_lbl = gtk_label_new("Visualización de canal:");
    st->mode_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(st->mode_combo), "Gris");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(st->mode_combo), "Tintado");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(st->mode_combo), "Pseudocolor");
    gtk_combo_box_set_active(GTK_COMBO_BOX(st->mode_combo), 1);
    g_signal_connect(st->mode_combo, "changed", G_CALLBACK(on_mode_changed), st);

    GtkWidget *steps_lbl = gtk_label_new("Acciones / Cálculos:");
    st->steps_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(st->steps_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(st->steps_view), GTK_WRAP_WORD_CHAR);
    GtkWidget *steps_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(steps_scroll, -1, 200);
    gtk_container_add(GTK_CONTAINER(steps_scroll), st->steps_view);

    gtk_box_pack_start(GTK_BOX(left), btn_open, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), st->file_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), gtk_label_new("Vista previa (original):"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), st->orig_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), gtk_label_new("Transformación:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), st->combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), mode_lbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), st->mode_combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), steps_lbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left), steps_scroll, TRUE, TRUE, 0);

    // Right panel: scrolled box of sections
    st->channels_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    st->channels_scroller = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(st->channels_scroller), st->channels_box);
    g_signal_connect(st->channels_scroller, "size-allocate", G_CALLBACK(on_channels_size_allocate), st);

    gtk_paned_pack1(GTK_PANED(paned), left, FALSE, FALSE);
    gtk_paned_pack2(GTK_PANED(paned), st->channels_scroller, TRUE, FALSE);
    gtk_paned_set_position(GTK_PANED(paned), 300); // approx quarter on 1200px

    st->loaded_pixbuf = NULL;
    st->preview_pixbuf = NULL;
    st->tile_size_px = 220;
    st->last_alloc_w = 0;
    st->mode = DISPLAY_TINT;

    gtk_widget_show_all(st->window);
    // Fill steps text initially
    fill_steps_text(st, (TransformType)gtk_combo_box_get_active(GTK_COMBO_BOX(st->combo)));
}
