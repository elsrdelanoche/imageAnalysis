#include "operations.hpp"
#include "utils/ImageProcessor.h"
#include "utils/MorphOps.hpp"
#include "utils/SEFactory.hpp"
#include "utils/Types.hpp"
#include "utils/FFT.hpp"
#include "utils/FFTImgUtils.hpp"

using ImgUtils::PixbufToGray;
using ImgUtils::GrayToPixbuf;

namespace {

// Utilidad para asegurar imagen activa y convertir a grises normalizado [0,1].
std::pair<std::vector<double>, bool>
ensure_gray_pixbuf(Gtk::Window& win,
                   const Glib::RefPtr<Gdk::Pixbuf>& pix,
                   int& w,
                   int& h) {
    if (!pix) {
        Gtk::MessageDialog dlg(win, "No hay una imagen activa.", false,
                               Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
        dlg.run();
        return {{}, false};
    }
    w = pix->get_width();
    h = pix->get_height();
    auto gray = PixbufToGray(pix);
    if (static_cast<int>(gray.size()) != w*h) {
        Gtk::MessageDialog dlg(win, "Error interno al convertir a escala de grises.", false,
                               Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dlg.run();
        return {{}, false};
    }
    return {std::move(gray), true};
}

// Convierte una pixbuf RGB a escala de grises in-place.
static Glib::RefPtr<Gdk::Pixbuf> to_grayscale_pixbuf(const Glib::RefPtr<Gdk::Pixbuf>& src) {
    if (!src) return {};
    auto dest = src->copy();
    int w = dest->get_width();
    int h = dest->get_height();
    int rs = dest->get_rowstride();
    int nchan = dest->get_n_channels();
    guchar* data = dest->get_pixels();

    for (int y=0; y<h; ++y) {
        guchar* row = data + y*rs;
        for (int x=0; x<w; ++x) {
            guchar* p = row + x*nchan;
            double R = p[0];
            double G = p[1];
            double B = p[2];
            guchar g = static_cast<guchar>(0.299*R + 0.587*G + 0.114*B);
            p[0]=p[1]=p[2]=g;
        }
    }
    return dest;
}

// Aplica brillo/contraste simple sobre RGB 0-255.
static Glib::RefPtr<Gdk::Pixbuf> apply_brightness_contrast(
        const Glib::RefPtr<Gdk::Pixbuf>& src,
        double brightness,
        double contrast)
{
    if (!src) return {};
    auto dest = src->copy();
    int w = dest->get_width();
    int h = dest->get_height();
    int rs = dest->get_rowstride();
    int nchan = dest->get_n_channels();
    guchar* data = dest->get_pixels();

    auto clamp_u8 = [](double v)->guchar {
        if (v < 0.0) return 0;
        if (v > 255.0) return 255;
        return static_cast<guchar>(v + 0.5);
    };

    for (int y=0; y<h; ++y) {
        guchar* row = data + y*rs;
        for (int x=0; x<w; ++x) {
            guchar* p = row + x*nchan;
            for (int c=0; c<3 && c<nchan; ++c) {
                double v = p[c];
                v = (v + brightness) * contrast;
                p[c] = clamp_u8(v);
            }
        }
    }
    return dest;
}

// Extrae un canal R/G/B dejando el resto a 0.
static Glib::RefPtr<Gdk::Pixbuf> extract_channel(
        const Glib::RefPtr<Gdk::Pixbuf>& src, int channel)
{
    if (!src) return {};
    auto dest = src->copy();
    int w = dest->get_width();
    int h = dest->get_height();
    int rs = dest->get_rowstride();
    int nchan = dest->get_n_channels();
    guchar* data = dest->get_pixels();

    if (channel < 0 || channel > 2) return dest;

    for (int y=0; y<h; ++y) {
        guchar* row = data + y*rs;
        for (int x=0; x<w; ++x) {
            guchar* p = row + x*nchan;
            for (int c=0; c<3 && c<nchan; ++c) {
                if (c != channel) p[c] = 0;
            }
        }
    }
    return dest;
}

} // anonymous

namespace Operations {

// =================== Filtros lineales / Canny (Práctica 8) ===================

void apply_lowpass(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Filtro pasa bajas (Gaussiano)");

    auto box   = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl   = Gtk::manage(new Gtk::Label("Pasa bajas gaussiano"));
    lbl->set_xalign(0.0);

    auto adj_k = Gtk::Adjustment::create(5.0, 3.0, 21.0, 2.0, 2.0);
    auto scl_k = Gtk::manage(new Gtk::Scale(adj_k));
    scl_k->set_digits(0);
    scl_k->set_value_pos(Gtk::POS_RIGHT);
    scl_k->set_hexpand(true);
    auto lbl_k = Gtk::manage(new Gtk::Label("Tamaño del kernel (impar):"));
    lbl_k->set_xalign(0.0);

    auto adj_s = Gtk::Adjustment::create(1.0, 0.3, 5.0, 0.1, 0.5);
    auto scl_s = Gtk::manage(new Gtk::Scale(adj_s));
    scl_s->set_digits(2);
    scl_s->set_value_pos(Gtk::POS_RIGHT);
    scl_s->set_hexpand(true);
    auto lbl_s = Gtk::manage(new Gtk::Label("Sigma:"));
    lbl_s->set_xalign(0.0);

    auto btn   = Gtk::manage(new Gtk::Button("Aplicar filtro"));

    btn->signal_clicked().connect([&win, &canvas, &panel, adj_k, adj_s]() {
        int w=0,h=0;
        auto pix = canvas.get_active_image();
        auto tup = ensure_gray_pixbuf(win, pix, w, h);
        auto img = std::move(tup.first);
        bool ok  = tup.second;
        if (!ok) return;

        int k = static_cast<int>(adj_k->get_value());
        if (k % 2 == 0) k += 1; // asegurar impar
        double sigma = adj_s->get_value();

        auto ks = Kern::LowPassKernels(k, sigma);
        if (ks.empty()) {
            Gtk::MessageDialog dlg(win, "No se pudo construir el kernel pasa bajas.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        auto kern = ks.front();
        auto out = Conv::Convolve(img, w, h, kern.data, kern.k);
        auto outPix = GrayToPixbuf(out, w, h);
        canvas.push_result_image(outPix);
        panel.set_current_tool("Pasa bajas (k=" + std::to_string(k) + ", σ=" + std::to_string(sigma) + ")");
    });

    box->pack_start(*lbl,  Gtk::PACK_SHRINK);
    box->pack_start(*lbl_k, Gtk::PACK_SHRINK);
    box->pack_start(*scl_k, Gtk::PACK_SHRINK);
    box->pack_start(*lbl_s, Gtk::PACK_SHRINK);
    box->pack_start(*scl_s, Gtk::PACK_SHRINK);
    box->pack_start(*btn,   Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

void apply_highpass(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Filtro de realce (high-pass)");

    auto box   = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl   = Gtk::manage(new Gtk::Label("Filtro de realce / high-pass"));
    lbl->set_xalign(0.0);

    auto adj_a = Gtk::Adjustment::create(1.0, 0.0, 3.0, 0.1, 0.5);
    auto scl_a = Gtk::manage(new Gtk::Scale(adj_a));
    scl_a->set_digits(2);
    scl_a->set_value_pos(Gtk::POS_RIGHT);
    scl_a->set_hexpand(true);
    auto lbl_a = Gtk::manage(new Gtk::Label("α (intensidad de realce):"));
    lbl_a->set_xalign(0.0);

    auto btn   = Gtk::manage(new Gtk::Button("Aplicar filtro"));

    btn->signal_clicked().connect([&win, &canvas, &panel, adj_a]() {
        int w=0,h=0;
        auto pix = canvas.get_active_image();
        auto tup = ensure_gray_pixbuf(win, pix, w, h);
        auto img = std::move(tup.first);
        bool ok  = tup.second;
        if (!ok) return;

        double alpha = adj_a->get_value();
        auto ks = Kern::HighPassKernels(alpha);
        if (ks.empty()) {
            Gtk::MessageDialog dlg(win, "No se pudo construir el kernel de realce.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        auto k = ks.front();
        auto out = Conv::Convolve(img, w, h, k.data, k.k);
        auto outPix = GrayToPixbuf(out, w, h);
        canvas.push_result_image(outPix);
        panel.set_current_tool("Realce (α=" + std::to_string(alpha) + ")");
    });

    box->pack_start(*lbl,   Gtk::PACK_SHRINK);
    box->pack_start(*lbl_a, Gtk::PACK_SHRINK);
    box->pack_start(*scl_a, Gtk::PACK_SHRINK);
    box->pack_start(*btn,   Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

void apply_edge(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Detección de bordes");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("Detección de bordes (kernels Sobel/Prewitt)"));
    lbl->set_xalign(0.0);

    auto btn = Gtk::manage(new Gtk::Button("Aplicar"));

    btn->signal_clicked().connect([&win, &canvas, &panel]() {
        int w=0,h=0;
        auto pix = canvas.get_active_image();
        auto tup = ensure_gray_pixbuf(win, pix, w, h);
        auto img = std::move(tup.first);
        bool ok  = tup.second;
        if (!ok) return;

        auto ks = Kern::EdgeKernels();

        if (ks.size() < 2) {
            Gtk::MessageDialog dlg(win, "No se pudieron construir los kernels de borde.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }

        // Buscar específicamente los kernels Sobel Gx y Sobel Gy para evitar kernels 2x2 (Roberts)
        Kern::KernelDesc kx = ks[0];
        Kern::KernelDesc ky = ks[1];
        for (const auto& kd : ks) {
            if (kd.name.find("Sobel Gx") != std::string::npos) kx = kd;
            if (kd.name.find("Sobel Gy") != std::string::npos) ky = kd;
        }
        auto gx = Conv::Convolve(img, w, h, kx.data, kx.k);
        auto gy = Conv::Convolve(img, w, h, ky.data, ky.k);
        auto mag = Conv::GradMag(gx, gy);
        auto outPix = GrayToPixbuf(mag, w, h);
        canvas.push_result_image(outPix);
        panel.set_current_tool("Bordes (gradiente de magnitud)");
    });

    box->pack_start(*lbl, Gtk::PACK_SHRINK);
    box->pack_start(*btn, Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

void apply_canny(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Canny (bordes)");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("Detector de bordes de Canny"));
    lbl->set_xalign(0.0);

    auto adj_k = Gtk::Adjustment::create(5.0, 3.0, 15.0, 2.0, 2.0);
    auto scl_k = Gtk::manage(new Gtk::Scale(adj_k));
    scl_k->set_digits(0);
    scl_k->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_k = Gtk::manage(new Gtk::Label("Kernel gaussiano (impar):"));
    lbl_k->set_xalign(0.0);

    auto adj_s = Gtk::Adjustment::create(1.0, 0.3, 5.0, 0.1, 0.5);
    auto scl_s = Gtk::manage(new Gtk::Scale(adj_s));
    scl_s->set_digits(2);
    scl_s->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_s = Gtk::manage(new Gtk::Label("Sigma:"));
    lbl_s->set_xalign(0.0);

    auto adj_tl = Gtk::Adjustment::create(0.1, 0.0, 1.0, 0.01, 0.1);
    auto scl_tl = Gtk::manage(new Gtk::Scale(adj_tl));
    scl_tl->set_digits(2);
    scl_tl->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_tl = Gtk::manage(new Gtk::Label("Umbral bajo (0-1):"));
    lbl_tl->set_xalign(0.0);

    auto adj_th = Gtk::Adjustment::create(0.3, 0.0, 1.0, 0.01, 0.1);
    auto scl_th = Gtk::manage(new Gtk::Scale(adj_th));
    scl_th->set_digits(2);
    scl_th->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_th = Gtk::manage(new Gtk::Label("Umbral alto (0-1):"));
    lbl_th->set_xalign(0.0);

    auto btn = Gtk::manage(new Gtk::Button("Aplicar Canny"));

    btn->signal_clicked().connect([&win, &canvas, &panel,
                                   adj_k, adj_s, adj_tl, adj_th]() {
        int w=0,h=0;
        auto pix = canvas.get_active_image();
        auto tup = ensure_gray_pixbuf(win, pix, w, h);
        auto img = std::move(tup.first);
        bool ok  = tup.second;
        if (!ok) return;

        Canny::Params p;
        int k = static_cast<int>(adj_k->get_value());
        if (k % 2 == 0) k += 1;
        p.k_gauss = k;
        p.sigma   = adj_s->get_value();
        p.t_low   = adj_tl->get_value();
        p.t_high  = adj_th->get_value();

        auto edges = Canny::Run(img, w, h, p);
        auto outPix = GrayToPixbuf(edges, w, h);
        canvas.push_result_image(outPix);
        panel.set_current_tool("Canny (k=" + std::to_string(k) + ")");
    });

    box->pack_start(*lbl,    Gtk::PACK_SHRINK);
    box->pack_start(*lbl_k,  Gtk::PACK_SHRINK);
    box->pack_start(*scl_k,  Gtk::PACK_SHRINK);
    box->pack_start(*lbl_s,  Gtk::PACK_SHRINK);
    box->pack_start(*scl_s,  Gtk::PACK_SHRINK);
    box->pack_start(*lbl_tl, Gtk::PACK_SHRINK);
    box->pack_start(*scl_tl, Gtk::PACK_SHRINK);
    box->pack_start(*lbl_th, Gtk::PACK_SHRINK);
    box->pack_start(*scl_th, Gtk::PACK_SHRINK);
    box->pack_start(*btn,    Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

// =================== Color / histograma / lógicas ===================

void do_color_transforms(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Transformaciones de color");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("Operaciones de color"));
    lbl->set_xalign(0.0);

    auto combo = Gtk::manage(new Gtk::ComboBoxText());
    combo->append("gris", "Escala de grises");
    combo->append("r",    "Canal R");
    combo->append("g",    "Canal G");
    combo->append("b",    "Canal B");
    combo->append("neg",  "Negativo");
    combo->append("bc",   "Brillo / contraste");
    combo->set_active_id("gris");

    auto adj_b = Gtk::Adjustment::create(0.0, -100.0, 100.0, 1.0, 10.0);
    auto scl_b = Gtk::manage(new Gtk::Scale(adj_b));
    scl_b->set_digits(0);
    scl_b->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_b = Gtk::manage(new Gtk::Label("Brillo (solo modo B/C):"));
    lbl_b->set_xalign(0.0);

    auto adj_c = Gtk::Adjustment::create(1.0, 0.1, 3.0, 0.1, 0.5);
    auto scl_c = Gtk::manage(new Gtk::Scale(adj_c));
    scl_c->set_digits(2);
    scl_c->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_c = Gtk::manage(new Gtk::Label("Contraste (solo modo B/C):"));
    lbl_c->set_xalign(0.0);

    auto btn = Gtk::manage(new Gtk::Button("Aplicar"));

    btn->signal_clicked().connect([&win, &canvas, &panel,
                                   combo, adj_b, adj_c]() {
        auto pix = canvas.get_active_image();
        if (!pix) {
            Gtk::MessageDialog dlg(win, "No hay una imagen activa.", false,
                                   Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }

        Glib::ustring id = combo->get_active_id();
        Glib::RefPtr<Gdk::Pixbuf> out;

        if (id == "gris") {
            out = to_grayscale_pixbuf(pix);
            panel.set_current_tool("Escala de grises");
        } else if (id == "r") {
            out = extract_channel(pix, 0);
            panel.set_current_tool("Canal R");
        } else if (id == "g") {
            out = extract_channel(pix, 1);
            panel.set_current_tool("Canal G");
        } else if (id == "b") {
            out = extract_channel(pix, 2);
            panel.set_current_tool("Canal B");
        } else if (id == "neg") {
            auto tmp = pix->copy();
            int w = tmp->get_width();
            int h = tmp->get_height();
            int rs = tmp->get_rowstride();
            int nchan = tmp->get_n_channels();
            guchar* data = tmp->get_pixels();
            for (int y=0; y<h; ++y) {
                guchar* row = data + y*rs;
                for (int x=0; x<w; ++x) {
                    guchar* p = row + x*nchan;
                    for (int c=0; c<3 && c<nchan; ++c) {
                        p[c] = 255 - p[c];
                    }
                }
            }
            out = tmp;
            panel.set_current_tool("Negativo");
        } else if (id == "bc") {
            double b = adj_b->get_value();
            double c = adj_c->get_value();
            out = apply_brightness_contrast(pix, b, c);
            panel.set_current_tool("Brillo/contraste");
        }

        if (out) {
            canvas.push_result_image(out);
        }
    });

    box->pack_start(*lbl,   Gtk::PACK_SHRINK);
    box->pack_start(*combo, Gtk::PACK_SHRINK);
    box->pack_start(*lbl_b, Gtk::PACK_SHRINK);
    box->pack_start(*scl_b, Gtk::PACK_SHRINK);
    box->pack_start(*lbl_c, Gtk::PACK_SHRINK);
    box->pack_start(*scl_c, Gtk::PACK_SHRINK);
    box->pack_start(*btn,   Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

// Histograma: ecualización simple con botón
void do_histogram(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Histograma / ecualización");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("Ecualización de histograma (escala de grises)"));
    lbl->set_xalign(0.0);

    auto btn = Gtk::manage(new Gtk::Button("Aplicar ecualización"));

    btn->signal_clicked().connect([&win, &canvas, &panel]() {
        int w=0,h=0;
        auto pix = canvas.get_active_image();
        auto tup = ensure_gray_pixbuf(win, pix, w, h);
        auto img = std::move(tup.first);
        bool ok  = tup.second;
        if (!ok) return;

        const int L = 256;
        std::vector<int> hist(L,0);
        for (double v : img) {
            int idx = std::min(L-1, std::max(0, static_cast<int>(v*255.0 + 0.5)));
            hist[idx]++;
        }
        std::vector<double> cdf(L,0.0);
        int total = w*h;
        int acc = 0;
        for (int i=0;i<L;++i) {
            acc += hist[i];
            cdf[i] = static_cast<double>(acc) / static_cast<double>(total);
        }
        std::vector<double> out(img.size());
        for (size_t i=0;i<img.size();++i) {
            int idx = std::min(L-1, std::max(0, static_cast<int>(img[i]*255.0 + 0.5)));
            out[i] = cdf[idx];
        }
        auto outPix = GrayToPixbuf(out, w, h);
        canvas.push_result_image(outPix);
        panel.set_current_tool("Ecualización de histograma");
    });

    box->pack_start(*lbl, Gtk::PACK_SHRINK);
    box->pack_start(*btn, Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

// Lógicas: umbral configurable
void do_logical_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Operaciones lógicas");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("Umbral binario sobre grises"));
    lbl->set_xalign(0.0);

    auto adj_t = Gtk::Adjustment::create(0.5, 0.0, 1.0, 0.01, 0.1);
    auto scl_t = Gtk::manage(new Gtk::Scale(adj_t));
    scl_t->set_digits(2);
    scl_t->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_t = Gtk::manage(new Gtk::Label("Umbral (0-1):"));
    lbl_t->set_xalign(0.0);

    auto btn = Gtk::manage(new Gtk::Button("Aplicar"));

    btn->signal_clicked().connect([&win, &canvas, &panel, adj_t]() {
        int w=0,h=0;
        auto pix = canvas.get_active_image();
        auto tup = ensure_gray_pixbuf(win, pix, w, h);
        auto img = std::move(tup.first);
        bool ok  = tup.second;
        if (!ok) return;

        double thr = adj_t->get_value();
        std::vector<double> out(img.size());
        for (size_t i=0;i<img.size();++i) {
            out[i] = img[i] >= thr ? 1.0 : 0.0;
        }
        auto outPix = GrayToPixbuf(out, w, h);
        canvas.push_result_image(outPix);
        panel.set_current_tool("Umbral binario (t=" + std::to_string(thr) + ")");
    });

    box->pack_start(*lbl,  Gtk::PACK_SHRINK);
    box->pack_start(*lbl_t, Gtk::PACK_SHRINK);
    box->pack_start(*scl_t, Gtk::PACK_SHRINK);
    box->pack_start(*btn,   Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

// =================== Geométricas (Práctica 5) ===================

void do_geom_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Transformaciones geométricas");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("Transformaciones geométricas básicas"));
    lbl->set_xalign(0.0);

    auto adj_ang = Gtk::Adjustment::create(15.0, -180.0, 180.0, 1.0, 10.0);
    auto scl_ang = Gtk::manage(new Gtk::Scale(adj_ang));
    scl_ang->set_digits(1);
    scl_ang->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_ang = Gtk::manage(new Gtk::Label("Ángulo de rotación (°):"));
    lbl_ang->set_xalign(0.0);

    auto adj_sx = Gtk::Adjustment::create(1.0, 0.1, 3.0, 0.1, 0.5);
    auto scl_sx = Gtk::manage(new Gtk::Scale(adj_sx));
    scl_sx->set_digits(2);
    scl_sx->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_sx = Gtk::manage(new Gtk::Label("Escala X:"));
    lbl_sx->set_xalign(0.0);

    auto adj_sy = Gtk::Adjustment::create(1.0, 0.1, 3.0, 0.1, 0.5);
    auto scl_sy = Gtk::manage(new Gtk::Scale(adj_sy));
    scl_sy->set_digits(2);
    scl_sy->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_sy = Gtk::manage(new Gtk::Label("Escala Y:"));
    lbl_sy->set_xalign(0.0);

    auto btn_rot = Gtk::manage(new Gtk::Button("Aplicar rotación"));
    auto btn_scl = Gtk::manage(new Gtk::Button("Aplicar reescalado"));

    btn_rot->signal_clicked().connect([&win, &canvas, &panel, adj_ang]() {
        auto pix = canvas.get_active_image();
        if (!pix) {
            Gtk::MessageDialog dlg(win, "No hay una imagen activa.", false,
                                   Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        ImageProcessor proc;
        double ang = adj_ang->get_value();
        auto rotated = proc.rotate(pix, ang);
        if (!rotated) {
            Gtk::MessageDialog dlg(win, "No se pudo aplicar la rotación.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        canvas.push_result_image(rotated);
        panel.set_current_tool("Rotación " + std::to_string(ang) + "°");
    });

    btn_scl->signal_clicked().connect([&win, &canvas, &panel, adj_sx, adj_sy]() {
        auto pix = canvas.get_active_image();
        if (!pix) {
            Gtk::MessageDialog dlg(win, "No hay una imagen activa.", false,
                                   Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        ImageProcessor proc;
        double sx = adj_sx->get_value();
        double sy = adj_sy->get_value();
        auto scaled = proc.scale(pix, sx, sy, Gdk::INTERP_BILINEAR);
        if (!scaled) {
            Gtk::MessageDialog dlg(win, "No se pudo aplicar el reescalado.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        canvas.push_result_image(scaled);
        panel.set_current_tool("Escala (" + std::to_string(sx) + ", " + std::to_string(sy) + ")");
    });

    box->pack_start(*lbl,    Gtk::PACK_SHRINK);
    box->pack_start(*lbl_ang, Gtk::PACK_SHRINK);
    box->pack_start(*scl_ang, Gtk::PACK_SHRINK);
    box->pack_start(*lbl_sx,  Gtk::PACK_SHRINK);
    box->pack_start(*scl_sx,  Gtk::PACK_SHRINK);
    box->pack_start(*lbl_sy,  Gtk::PACK_SHRINK);
    box->pack_start(*scl_sy,  Gtk::PACK_SHRINK);
    box->pack_start(*btn_rot, Gtk::PACK_SHRINK);
    box->pack_start(*btn_scl, Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

// =================== Morfología (Práctica 6) ===================

void do_morph_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Operaciones morfológicas");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("Morfología binaria"));
    lbl->set_xalign(0.0);

    auto combo = Gtk::manage(new Gtk::ComboBoxText());
    combo->append("erode",  "Erosión");
    combo->append("dilate", "Dilatación");
    combo->append("open",   "Apertura");
    combo->append("close",  "Cierre");
    combo->set_active_id("open");

    auto adj_k = Gtk::Adjustment::create(3.0, 3.0, 15.0, 2.0, 2.0);
    auto scl_k = Gtk::manage(new Gtk::Scale(adj_k));
    scl_k->set_digits(0);
    scl_k->set_value_pos(Gtk::POS_RIGHT);
    auto lbl_k = Gtk::manage(new Gtk::Label("Tamaño del elemento estructurante (impar):"));
    lbl_k->set_xalign(0.0);

    auto btn = Gtk::manage(new Gtk::Button("Aplicar"));

    btn->signal_clicked().connect([&win, &canvas, &panel, combo, adj_k]() {
        auto pix = canvas.get_active_image();
        if (!pix) {
            Gtk::MessageDialog dlg(win, "No hay una imagen activa.", false,
                                   Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        int k = static_cast<int>(adj_k->get_value());
        if (k % 2 == 0) k += 1;

        auto se = SEFactory::make(SEShape::Square, k);
        Glib::RefPtr<Gdk::Pixbuf> out;

        Glib::ustring id = combo->get_active_id();
        if (id == "erode") {
            out = MorphOps::Erode(pix, se);
        } else if (id == "dilate") {
            out = MorphOps::Dilate(pix, se);
        } else if (id == "open") {
            out = MorphOps::Open(pix, se);
        } else if (id == "close") {
            out = MorphOps::Close(pix, se);
        }

        if (!out) {
            Gtk::MessageDialog dlg(win, "No se pudo aplicar la operación morfológica.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        canvas.push_result_image(out);
        panel.set_current_tool("Morfología (" + id + ", k=" + std::to_string(k) + ")");
    });

    box->pack_start(*lbl,   Gtk::PACK_SHRINK);
    box->pack_start(*combo, Gtk::PACK_SHRINK);
    box->pack_start(*lbl_k, Gtk::PACK_SHRINK);
    box->pack_start(*scl_k, Gtk::PACK_SHRINK);
    box->pack_start(*btn,   Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

// =================== Fourier (Práctica 7) ===================

void do_fourier_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel) {
    panel.set_current_tool("Transformada de Fourier");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->set_spacing(6);

    auto lbl = Gtk::manage(new Gtk::Label("FFT 2D sobre imagen cuadrada (N×N, potencia de 2)"));
    lbl->set_xalign(0.0);

    auto combo = Gtk::manage(new Gtk::ComboBoxText());
    combo->append("mag", "Espectro de magnitud");
    combo->append("phase", "Fase");
    combo->set_active_id("mag");

    auto btn = Gtk::manage(new Gtk::Button("Aplicar FFT"));

    btn->signal_clicked().connect([&win, &canvas, &panel, combo]() {
        auto pix = canvas.get_active_image();
        if (!pix) {
            Gtk::MessageDialog dlg(win, "No hay una imagen activa.", false,
                                   Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }

        int N = 0;
        auto sq = FFTImgUtils::ToSquarePow2(pix, N);
        if (!sq || N <= 0) {
            Gtk::MessageDialog dlg(win, "La imagen debe poder escalarse a tamaño N×N potencia de 2.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }
        auto gray = FFTImgUtils::PixbufToGray(sq);
        if (static_cast<int>(gray.size()) != N*N) {
            Gtk::MessageDialog dlg(win, "Error interno en preparación para FFT.", false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
            return;
        }

        std::vector<FFT::cd> img(N*N);
        for (int i=0;i<N*N;++i) {
            img[i] = FFT::cd(gray[i], 0.0);
        }

        FFT::fft2d(img, N, +1);
        FFT::fftshift2d(img, N);

        std::vector<double> mag, phase;
        FFT::mag_phase(img, N, mag, phase);

        Glib::RefPtr<Gdk::Pixbuf> out;
        Glib::ustring id = combo->get_active_id();
        if (id == "mag") {
            out = FFTImgUtils::MagToPixbuf(mag, N);
            panel.set_current_tool("Espectro de magnitud");
        } else {
            out = FFTImgUtils::MagToPixbuf(phase, N); // reusar mapeo
            panel.set_current_tool("Fase (mapeada)");
        }

        if (out) {
            canvas.push_result_image(out);
        }
    });

    box->pack_start(*lbl,   Gtk::PACK_SHRINK);
    box->pack_start(*combo, Gtk::PACK_SHRINK);
    box->pack_start(*btn,   Gtk::PACK_SHRINK);

    panel.set_param_widget(box);
}

// =================== Placeholder genérico ===================

void not_implemented(Gtk::Window& win, const Glib::ustring& name) {
    Glib::ustring msg = "La herramienta \"" + name + "\" todavía no está integrada.";
    Gtk::MessageDialog dlg(win, msg, false,
                           Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
    dlg.run();
}

} // namespace Operations
