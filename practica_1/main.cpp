#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdint>
#include <fstream>

class ImageChannelExtractor {
public:
    // === API ===
    enum class IntImageMode {
        COLOR_RGB_PACKED,  // 0x00RRGGBB
        RED_8U,            // 0..255
        GREEN_8U,          // 0..255
        BLUE_8U,           // 0..255
        GRAY_8U            // 0..255
    };

private:
    GtkWidget *window;
    GtkWidget *main_grid;
    GtkWidget *load_button;
    GtkWidget *brightness_scale;
    GtkWidget *contrast_scale;
    GtkWidget *brightness_label;
    GtkWidget *contrast_label;
    GtkWidget *filename_label;
    GtkWidget *export_button;
    GtkWidget *mode_combo;
    GtkWidget *images[6];            // 0: menú, 1: original adj., 2:R, 3:B, 4:G, 5:gris adj.
    GtkWidget *labels[6];
    GdkPixbuf *original_pixbuf;      // sin escalar
    GdkPixbuf *base_color_pixbuf;    // escalada (color) sin ajustes
    GdkPixbuf *base_grayscale_pixbuf;// escalada (grises) sin ajustes
    
    // Controles
    double brightness_value;
    double contrast_value;
    
public:
    ImageChannelExtractor()
        : original_pixbuf(nullptr),
          base_color_pixbuf(nullptr),
          base_grayscale_pixbuf(nullptr),
          brightness_value(0.0),
          contrast_value(1.0) {
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Extractor de Canales RGB - 24 bits");
        gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        setupUI();
    }
    
    void setupUI() {
        main_grid = gtk_grid_new();
        gtk_grid_set_row_homogeneous(GTK_GRID(main_grid), TRUE);
        gtk_grid_set_column_homogeneous(GTK_GRID(main_grid), TRUE);
        gtk_grid_set_row_spacing(GTK_GRID(main_grid), 5);
        gtk_grid_set_column_spacing(GTK_GRID(main_grid), 5);
        gtk_container_set_border_width(GTK_CONTAINER(main_grid), 10);
        
        const char* label_texts[] = {
            "Menú", "Imagen Original", "Canal Rojo (8 bits R)",
            "Canal Azul (8 bits B)", "Canal Verde (8 bits G)", "Escala de Grises con Controles"
        };
        
        for (int i = 0; i < 6; i++) {
            GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
            labels[i] = gtk_label_new(label_texts[i]);
            gtk_widget_set_halign(labels[i], GTK_ALIGN_CENTER);
            
            if (i == 0) {
                setupControlSection(vbox);
            } else {
                images[i] = gtk_image_new();
                gtk_image_set_from_icon_name(GTK_IMAGE(images[i]), "image-missing", GTK_ICON_SIZE_DIALOG);
                gtk_box_pack_start(GTK_BOX(vbox), labels[i], FALSE, FALSE, 0);
                gtk_box_pack_start(GTK_BOX(vbox), images[i], TRUE, TRUE, 0);
            }
            
            GtkWidget *frame = gtk_frame_new(NULL);
            gtk_container_add(GTK_CONTAINER(frame), vbox);
            int row = i / 3;
            int col = i % 3;
            gtk_grid_attach(GTK_GRID(main_grid), frame, col, row, 1, 1);
        }
        
        gtk_container_add(GTK_CONTAINER(window), main_grid);
    }
    
    void setupControlSection(GtkWidget *vbox) {
        gtk_box_set_spacing(GTK_BOX(vbox), 3);
        gtk_box_pack_start(GTK_BOX(vbox), labels[0], FALSE, FALSE, 0);
        gtk_widget_set_halign(labels[0], GTK_ALIGN_CENTER);

        // Botón de carga
        load_button = gtk_button_new_with_label("Seleccionar Imagen");
        gtk_widget_set_halign(load_button, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(load_button, GTK_ALIGN_CENTER);
        g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_clicked), this);
        gtk_box_pack_start(GTK_BOX(vbox), load_button, FALSE, FALSE, 0);
        
        // Ícono
        images[0] = gtk_image_new();
        gtk_image_set_from_icon_name(GTK_IMAGE(images[0]), "image-x-generic", GTK_ICON_SIZE_DIALOG);
        gtk_image_set_pixel_size(GTK_IMAGE(images[0]), 96);
        gtk_widget_set_halign(images[0], GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(vbox), images[0], FALSE, FALSE, 0);

        // Nombre de archivo
        filename_label = gtk_label_new("Sin archivo");
        gtk_widget_set_halign(filename_label, GTK_ALIGN_CENTER);
        gtk_label_set_xalign(GTK_LABEL(filename_label), 0.5);
        gtk_box_pack_start(GTK_BOX(vbox), filename_label, FALSE, FALSE, 0);
        
        // Separador
        GtkWidget *separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start(GTK_BOX(vbox), separator1, FALSE, FALSE, 3);
        
        // Brillo
        brightness_label = gtk_label_new("Brillo: 0");
        gtk_widget_set_halign(brightness_label, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(vbox), brightness_label, FALSE, FALSE, 0);
        
        brightness_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -100.0, 100.0, 1.0);
        gtk_range_set_value(GTK_RANGE(brightness_scale), 0.0);
        gtk_scale_set_draw_value(GTK_SCALE(brightness_scale), FALSE);
        gtk_widget_set_halign(brightness_scale, GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(brightness_scale, 220, -1);
        g_signal_connect(brightness_scale, "value-changed", G_CALLBACK(on_brightness_changed), this);
        gtk_box_pack_start(GTK_BOX(vbox), brightness_scale, FALSE, FALSE, 0);
        
        // Contraste
        contrast_label = gtk_label_new("Contraste: 1.00");
        gtk_widget_set_halign(contrast_label, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(vbox), contrast_label, FALSE, FALSE, 0);
        
        contrast_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.1, 2.0, 0.01);
        gtk_range_set_value(GTK_RANGE(contrast_scale), 1.0);
        gtk_scale_set_draw_value(GTK_SCALE(contrast_scale), FALSE);
        gtk_widget_set_halign(contrast_scale, GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(contrast_scale, 220, -1);
        g_signal_connect(contrast_scale, "value-changed", G_CALLBACK(on_contrast_changed), this);
        gtk_box_pack_start(GTK_BOX(vbox), contrast_scale, FALSE, FALSE, 0);

        // ----- Título del selector de exportación -----
        GtkWidget *mode_title = gtk_label_new("Modo para exportar a CSV");
        gtk_widget_set_halign(mode_title, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(vbox), mode_title, FALSE, FALSE, 0);

        // Selector de modo (para exportar)
        mode_combo = gtk_combo_box_text_new();
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), "Color (0x00RRGGBB)");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), "Rojo (0..255)");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), "Verde (0..255)");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), "Azul (0..255)");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), "Gris (0..255)");
        gtk_combo_box_set_active(GTK_COMBO_BOX(mode_combo), 0);
        gtk_widget_set_halign(mode_combo, GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(mode_combo, 220, -1);
        gtk_box_pack_start(GTK_BOX(vbox), mode_combo, FALSE, FALSE, 2);
        
        // Botón Exportar CSV
        export_button = gtk_button_new_with_label("Exportar CSV");
        gtk_widget_set_halign(export_button, GTK_ALIGN_CENTER);
        g_signal_connect(export_button, "clicked", G_CALLBACK(on_export_clicked), this);
        gtk_box_pack_start(GTK_BOX(vbox), export_button, FALSE, FALSE, 3);

        // Reset
        GtkWidget *reset_button = gtk_button_new_with_label("Reset Controles");
        gtk_widget_set_halign(reset_button, GTK_ALIGN_CENTER);
        g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_clicked), this);
        gtk_box_pack_start(GTK_BOX(vbox), reset_button, FALSE, FALSE, 3);
    }
    
    // ====== Callbacks ======
    static void on_load_clicked(GtkWidget *widget, gpointer data) {
        (void)widget;
        ImageChannelExtractor* app = static_cast<ImageChannelExtractor*>(data);
        app->loadImage();
    }
    
    static void on_brightness_changed(GtkRange *range, gpointer data) {
        ImageChannelExtractor* app = static_cast<ImageChannelExtractor*>(data);
        app->brightness_value = gtk_range_get_value(range);
        app->updateBrightnessLabel();
        app->updateColorImage();
        app->updateGrayscaleImage();
    }
    
    static void on_contrast_changed(GtkRange *range, gpointer data) {
        ImageChannelExtractor* app = static_cast<ImageChannelExtractor*>(data);
        app->contrast_value = gtk_range_get_value(range);
        app->updateContrastLabel();
        app->updateColorImage();
        app->updateGrayscaleImage();
    }
    
    static void on_reset_clicked(GtkWidget *widget, gpointer data) {
        (void)widget;
        ImageChannelExtractor* app = static_cast<ImageChannelExtractor*>(data);
        app->resetControls();
    }

    static void on_export_clicked(GtkWidget *widget, gpointer data) {
        (void)widget;
        ImageChannelExtractor* app = static_cast<ImageChannelExtractor*>(data);

        // Elegir modo según combo
        IntImageMode mode = app->comboToMode();

        // Diálogo de guardado
        GtkWidget *dialog = gtk_file_chooser_dialog_new(
            "Guardar CSV",
            GTK_WINDOW(app->window),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Cancelar", GTK_RESPONSE_CANCEL,
            "_Guardar", GTK_RESPONSE_ACCEPT,
            NULL
        );
        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "matriz.csv");

        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            char *filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            bool ok = app->exportCSV(mode, filepath);
            if (!ok) {
                GtkWidget *md = gtk_message_dialog_new(
                    GTK_WINDOW(app->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                    "No se pudo exportar el CSV."
                );
                gtk_dialog_run(GTK_DIALOG(md));
                gtk_widget_destroy(md);
            }
            g_free(filepath);
        }
        gtk_widget_destroy(dialog);
    }
    
    // ====== UI helpers ======
    void updateBrightnessLabel() {
        char text[50];
        snprintf(text, sizeof(text), "Brillo: %.0f", brightness_value);
        gtk_label_set_text(GTK_LABEL(brightness_label), text);
    }
    
    void updateContrastLabel() {
        char text[50];
        snprintf(text, sizeof(text), "Contraste: %.2f", contrast_value);
        gtk_label_set_text(GTK_LABEL(contrast_label), text);
    }
    
    void resetControls() {
        gtk_range_set_value(GTK_RANGE(brightness_scale), 0.0);
        gtk_range_set_value(GTK_RANGE(contrast_scale), 1.0);
        brightness_value = 0.0;
        contrast_value = 1.0;
        updateBrightnessLabel();
        updateContrastLabel();
        updateColorImage();
        updateGrayscaleImage();
    }
    
    // ====== Carga / pipeline ======
    void loadImage() {
        GtkWidget *dialog = gtk_file_chooser_dialog_new(
            "Seleccionar Imagen",
            GTK_WINDOW(window),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancelar", GTK_RESPONSE_CANCEL,
            "_Abrir", GTK_RESPONSE_ACCEPT,
            NULL
        );
        
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, "Imágenes");
        gtk_file_filter_add_pattern(filter, "*.jpg");
        gtk_file_filter_add_pattern(filter, "*.jpeg");
        gtk_file_filter_add_pattern(filter, "*.png");
        gtk_file_filter_add_pattern(filter, "*.bmp");
        gtk_file_filter_add_pattern(filter, "*.gif");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            processImage(filename);
            g_free(filename);
        }
        
        gtk_widget_destroy(dialog);
    }
    
    void processImage(const char* filename) {
        GError *error = NULL;
        
        if (original_pixbuf)       g_object_unref(original_pixbuf);
        if (base_color_pixbuf)     g_object_unref(base_color_pixbuf);
        if (base_grayscale_pixbuf) g_object_unref(base_grayscale_pixbuf);
        
        original_pixbuf = gdk_pixbuf_new_from_file(filename, &error);
        if (error) {
            g_print("Error cargando imagen: %s\n", error->message);
            g_error_free(error);
            return;
        }
        
        // Actualiza etiqueta con el nombre (basename)
        char *base = g_path_get_basename(filename);
        gtk_label_set_text(GTK_LABEL(filename_label), base ? base : "Sin archivo");
        if (base) g_free(base);

        // Redimensionar para visualización
        int target_width = 250;
        int target_height = 200;
        GdkPixbuf *scaled_original = gdk_pixbuf_scale_simple(
            original_pixbuf, target_width, target_height, GDK_INTERP_BILINEAR
        );

        // Guardar base color escalada
        base_color_pixbuf = gdk_pixbuf_copy(scaled_original);

        // Mostrar (sin ajuste) y continuar pipeline
        gtk_image_set_from_pixbuf(GTK_IMAGE(images[1]), scaled_original);

        // Extraer canales y base de grises (usa scaled_original)
        extractChannelsWith24BitManipulation(scaled_original, target_width, target_height);

        // Aplicar ajustes actuales a color y grises
        updateColorImage();
        updateGrayscaleImage();

        g_object_unref(scaled_original);
    }
    
    void extractChannelsWith24BitManipulation(GdkPixbuf *pixbuf, int width, int height) {
        if (!pixbuf) return;
        int channels = gdk_pixbuf_get_n_channels(pixbuf);
        int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
        guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
        if (channels < 3) {
            g_print("La imagen debe tener al menos 3 canales RGB\n");
            return;
        }
        
        GdkPixbuf *red_pixbuf   = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
        GdkPixbuf *blue_pixbuf  = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
        GdkPixbuf *green_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
        base_grayscale_pixbuf   = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
        
        guchar *red_pixels   = gdk_pixbuf_get_pixels(red_pixbuf);
        guchar *blue_pixels  = gdk_pixbuf_get_pixels(blue_pixbuf);
        guchar *green_pixels = gdk_pixbuf_get_pixels(green_pixbuf);
        guchar *gray_pixels  = gdk_pixbuf_get_pixels(base_grayscale_pixbuf);
        
        int red_rowstride   = gdk_pixbuf_get_rowstride(red_pixbuf);
        int blue_rowstride  = gdk_pixbuf_get_rowstride(blue_pixbuf);
        int green_rowstride = gdk_pixbuf_get_rowstride(green_pixbuf);
        int gray_rowstride  = gdk_pixbuf_get_rowstride(base_grayscale_pixbuf);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                guchar *p = pixels + y * rowstride + x * channels;
                guchar r = p[0], g = p[1], b = p[2];
                uint32_t pixel_data_b = ((uint32_t)b) & 0xFF;
                uint32_t pixel_data_g = (((uint32_t)g) << 8) & 0xFF00;
                uint32_t pixel_data_r = (((uint32_t)r) << 16) & 0xFF0000;
                guchar fB = (guchar)(pixel_data_b);
                guchar fG = (guchar)(pixel_data_g >> 8);
                guchar fR = (guchar)(pixel_data_r >> 16);
                
                guchar *red_p   = red_pixels   + y * red_rowstride   + x * 3;
                red_p[0] = fR; red_p[1] = 0;   red_p[2] = 0;
                
                guchar *blue_p  = blue_pixels  + y * blue_rowstride  + x * 3;
                blue_p[0] = 0;  blue_p[1] = 0; blue_p[2] = fB;
                
                guchar *green_p = green_pixels + y * green_rowstride + x * 3;
                green_p[0] = 0; green_p[1] = fG; green_p[2] = 0;
                
                guchar gray_value = (guchar)((fR + fG + fB) / 3);
                guchar *gray_p = gray_pixels + y * gray_rowstride + x * 3;
                gray_p[0] = gray_value; gray_p[1] = gray_value; gray_p[2] = gray_value;
            }
        }
        
        gtk_image_set_from_pixbuf(GTK_IMAGE(images[2]), red_pixbuf);
        gtk_image_set_from_pixbuf(GTK_IMAGE(images[3]), blue_pixbuf);
        gtk_image_set_from_pixbuf(GTK_IMAGE(images[4]), green_pixbuf);
        
        g_object_unref(red_pixbuf);
        g_object_unref(blue_pixbuf);
        g_object_unref(green_pixbuf);
    }

    // ====== Ajuste color (Brillo/Contraste) ======
    void updateColorImage() {
        if (!base_color_pixbuf) return;
        int width      = gdk_pixbuf_get_width(base_color_pixbuf);
        int height     = gdk_pixbuf_get_height(base_color_pixbuf);
        gboolean alpha = gdk_pixbuf_get_has_alpha(base_color_pixbuf);
        int channels   = gdk_pixbuf_get_n_channels(base_color_pixbuf);
        int src_rs     = gdk_pixbuf_get_rowstride(base_color_pixbuf);
        guchar *src_px = gdk_pixbuf_get_pixels(base_color_pixbuf);

        GdkPixbuf *adj = gdk_pixbuf_new(GDK_COLORSPACE_RGB, alpha, 8, width, height);
        int dst_rs     = gdk_pixbuf_get_rowstride(adj);
        guchar *dst_px = gdk_pixbuf_get_pixels(adj);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                guchar *s = src_px + y * src_rs + x * channels;
                guchar *d = dst_px + y * dst_rs + x * channels;
                for (int c = 0; c < 3; ++c) {
                    double v  = s[c];
                    double br = v + brightness_value;
                    double sc = br * contrast_value;
                    if (sc < 0.0)   sc = 0.0;
                    if (sc > 255.0) sc = 255.0;
                    d[c] = static_cast<guchar>(sc);
                }
                if (alpha && channels == 4) d[3] = s[3];
            }
        }
        gtk_image_set_from_pixbuf(GTK_IMAGE(images[1]), adj);
        g_object_unref(adj);
    }
    
    // ====== Ajuste grises (Brillo/Contraste) ======
    void updateGrayscaleImage() {
        if (!base_grayscale_pixbuf) return;
        int width  = gdk_pixbuf_get_width(base_grayscale_pixbuf);
        int height = gdk_pixbuf_get_height(base_grayscale_pixbuf);
        
        GdkPixbuf *adjusted_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
        guchar *base_pixels = gdk_pixbuf_get_pixels(base_grayscale_pixbuf);
        guchar *adj_pixels  = gdk_pixbuf_get_pixels(adjusted_pixbuf);
        int base_rowstride  = gdk_pixbuf_get_rowstride(base_grayscale_pixbuf);
        int adj_rowstride   = gdk_pixbuf_get_rowstride(adjusted_pixbuf);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                guchar *base_p = base_pixels + y * base_rowstride + x * 3;
                guchar *adj_p  = adj_pixels  + y * adj_rowstride  + x * 3;
                double original = base_p[0];
                double bright   = original + brightness_value;
                double scaled   = bright * contrast_value;
                if (scaled < 0.0)   scaled = 0.0;
                if (scaled > 255.0) scaled = 255.0;
                guchar v = static_cast<guchar>(scaled);
                adj_p[0] = v; adj_p[1] = v; adj_p[2] = v;
            }
        }
        gtk_image_set_from_pixbuf(GTK_IMAGE(images[5]), adjusted_pixbuf);
        g_object_unref(adjusted_pixbuf);
    }

    // ====== API "tipo Java" ======
    std::vector<std::vector<uint32_t>> getImagenInt(IntImageMode mode) const {
        if (!base_color_pixbuf) return {};
        const int width      = gdk_pixbuf_get_width(base_color_pixbuf);
        const int height     = gdk_pixbuf_get_height(base_color_pixbuf);
        const int channels   = gdk_pixbuf_get_n_channels(base_color_pixbuf);
        const int rowstride  = gdk_pixbuf_get_rowstride(base_color_pixbuf);
        const gboolean alpha = gdk_pixbuf_get_has_alpha(base_color_pixbuf);
        const guchar* pixels = gdk_pixbuf_get_pixels(base_color_pixbuf);
        (void)alpha;

        std::vector<std::vector<uint32_t>> out(height, std::vector<uint32_t>(width, 0));
        for (int y = 0; y < height; ++y) {
            const guchar* row = pixels + y * rowstride;
            for (int x = 0; x < width; ++x) {
                const guchar* p = row + x * channels;
                const uint32_t r = p[0];
                const uint32_t g = p[1];
                const uint32_t b = p[2];
                switch (mode) {
                    case IntImageMode::COLOR_RGB_PACKED:
                        out[y][x] = (r << 16) | (g << 8) | (b);
                        break;
                    case IntImageMode::RED_8U:   out[y][x] = r; break;
                    case IntImageMode::GREEN_8U: out[y][x] = g; break;
                    case IntImageMode::BLUE_8U:  out[y][x] = b; break;
                    case IntImageMode::GRAY_8U:  out[y][x] = (r + g + b) / 3; break;
                }
            }
        }
        return out;
    }

    static std::vector<uint32_t> convertirInt2DA1D(const std::vector<std::vector<uint32_t>>& m) {
        if (m.empty()) return {};
        std::vector<uint32_t> v;
        v.reserve(m.size() * m[0].size());
        for (const auto& row : m) v.insert(v.end(), row.begin(), row.end());
        return v;
    }

    // ====== Exportación CSV ======
    bool exportCSV(IntImageMode mode, const char* path) {
        auto M = getImagenInt(mode);
        if (M.empty()) return false;

        std::ofstream ofs(path);
        if (!ofs.is_open()) return false;

        const size_t H = M.size();
        const size_t W = M[0].size();

        for (size_t y = 0; y < H; ++y) {
            for (size_t x = 0; x < W; ++x) {
                ofs << M[y][x];
                if (x + 1 < W) ofs << ',';
            }
            ofs << '\n';
        }
        ofs.close();
        return true;
    }

    // ====== Utilidad: leer selección del combo ======
    IntImageMode comboToMode() const {
        int idx = gtk_combo_box_get_active(GTK_COMBO_BOX(mode_combo));
        switch (idx) {
            case 0: return IntImageMode::COLOR_RGB_PACKED;
            case 1: return IntImageMode::RED_8U;
            case 2: return IntImageMode::GREEN_8U;
            case 3: return IntImageMode::BLUE_8U;
            case 4: return IntImageMode::GRAY_8U;
            default: return IntImageMode::COLOR_RGB_PACKED;
        }
    }

    // ====== Mostrar ventana ======
    void show() { gtk_widget_show_all(window); }
    
    ~ImageChannelExtractor() {
        if (original_pixbuf)       g_object_unref(original_pixbuf);
        if (base_color_pixbuf)     g_object_unref(base_color_pixbuf);
        if (base_grayscale_pixbuf) g_object_unref(base_grayscale_pixbuf);
    }
};

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    ImageChannelExtractor app;
    app.show();
    gtk_main();
    return 0;
}





guchar gray_value = (guchar)((r + g + b) / 3);
guchar *gray_p = gray_pixels + y * gray_rowstride + x * 3;
gray_p[0] = gray_value;
gray_p[1] = gray_value;
gray_p[2] = gray_value;


guchar r = p[0];  // componente rojo
guchar g = p[1];  // componente verde
guchar b = p[2];  // componente azul

// Construcción de imágenes separadas
guchar *red_p   = red_pixels   + y * red_rowstride   + x * 3;
red_p[0] = r; red_p[1] = 0; red_p[2] = 0;

guchar *blue_p  = blue_pixels  + y * blue_rowstride  + x * 3;
blue_p[0] = 0; blue_p[1] = 0; blue_p[2] = b;

guchar *green_p = green_pixels + y * green_rowstride + x * 3;
green_p[0] = 0; green_p[1] = g; green_p[2] = 0;




original_pixbuf = gdk_pixbuf_new_from_file(filename, &error);
int width  = gdk_pixbuf_get_width(original_pixbuf);
int height = gdk_pixbuf_get_height(original_pixbuf);
guchar *pixels = gdk_pixbuf_get_pixels(original_pixbuf);


