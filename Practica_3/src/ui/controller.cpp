
#include "ui/controller.hpp"
#include "io/loader.hpp"
#include "model/adapters.hpp"
#include "core/image_view.hpp"
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cairo.h>
#include <pango/pangocairo.h>

namespace {

void rgb_to_gray(const core::ImageView& src, core::ImageBuffer& dst){
    dst.reset(src.width(), src.height(), 3);
    for (int y=0;y<src.height();++y){
        const uint8_t* s = src.pixel(0,y);
        uint8_t* d = dst.data() + y*dst.stride();
        for (int x=0;x<src.width();++x){
            uint8_t r=s[0], g=s[1], b=s[2];
            uint8_t gray = uint8_t((0.299*r + 0.587*g + 0.114*b) + 0.5);
            d[0]=d[1]=d[2]=gray;
            s+=3; d+=3;
        }
    }
}

struct Stats { double mean, var, stddev, skew, kurt, entropy; int minv, maxv, mode; double median; };

Stats compute_stats(const std::vector<uint32_t>& h, uint32_t total){
    Stats st{}; st.minv=0; st.maxv=0; st.mode=0;
    double mean=0.0;
    for (int i=0;i<256;++i) mean += i * (double)h[i];
    mean /= (double)total;
    st.mean = mean;
    double var=0, skew=0, kurt=0, entropy=0;
    uint32_t cum=0;
    int minvFound=-1, maxvFound=255, modeIdx=0;
    for (int i=0;i<256;++i){
        double p = h[i] / (double)total;
        double diff = i - mean;
        var += diff*diff * p;
        skew += diff*diff*diff * p;
        kurt += diff*diff*diff*diff * p;
        if (p>0) entropy += -p*std::log2(p);
        if (h[i]>h[modeIdx]) modeIdx=i;
        if (minvFound==-1 && h[i]>0) minvFound=i;
        if (h[i]>0) maxvFound=i;
        cum += h[i];
    }
    st.var=var;
    st.stddev = std::sqrt(var);
    st.skew = (st.stddev>1e-12)? (skew/(st.stddev*st.stddev*st.stddev)) : 0.0;
    st.kurt = (var>1e-12)? (kurt/(var*var)) : 0.0;
    st.entropy = entropy;
    st.minv=minvFound==-1?0:minvFound;
    st.maxv=maxvFound;
    st.mode=modeIdx;
    uint32_t half = total/2;
    uint32_t acc=0;
    int med=0;
    for (int i=0;i<256;++i){ acc+=h[i]; if (acc>=half){ med=i; break; } }
    st.median = med;
    return st;
}

GdkPixbuf* draw_hist(const std::vector<uint32_t>& h, const char* /*title*/){
    const int W=ui::View::TILE_W, H=ui::View::TILE_H;
    const int L=50, B=28, R=10, T=10;
    cairo_surface_t* surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, W, H);
    cairo_t* cr = cairo_create(surf);
    cairo_set_source_rgb(cr, 1,1,1); cairo_paint(cr);
    int PW=W-L-R, PH=H-T-B;
    uint32_t hmax=1; for (auto v: h) if(v>hmax) hmax=v;

    // axes
    cairo_set_source_rgb(cr,0,0,0);
    cairo_move_to(cr, L, H-B); cairo_line_to(cr, W-R, H-B); cairo_stroke(cr);
    cairo_move_to(cr, L, H-B); cairo_line_to(cr, L, T); cairo_stroke(cr);

    PangoLayout* layout = pango_cairo_create_layout(cr);
    PangoFontDescription* fd = pango_font_description_from_string("Sans 9");
    pango_layout_set_font_description(layout, fd);

    for(int j=0;j<=5;++j){
        double val = (hmax * j)/5.0;
        int y = H-B - int(std::round((val / hmax) * PH));
        cairo_move_to(cr, L-5, y); cairo_line_to(cr, L, y); cairo_stroke(cr);
        char buf[32]; snprintf(buf, sizeof(buf), "%.0f", val);
        pango_layout_set_text(layout, buf, -1);
        int tw,th; pango_layout_get_pixel_size(layout, &tw, &th);
        cairo_move_to(cr, L-10-tw, y - th/2); pango_cairo_show_layout(cr, layout);
    }
    int xticks[] = {0,50,100,150,200,255};
    for (int k=0;k<6;++k){
        int i=xticks[k];
        int x = L + int(std::round((i/255.0)*PW));
        cairo_move_to(cr, x, H-B); cairo_line_to(cr, x, H-B+5); cairo_stroke(cr);
        char buf[16]; snprintf(buf, sizeof(buf), "%d", i);
        pango_layout_set_text(layout, buf, -1);
        int tw,th; pango_layout_get_pixel_size(layout, &tw, &th);
        cairo_move_to(cr, x - tw/2, H-B+6); pango_cairo_show_layout(cr, layout);
    }

    // bars
    for (int i=0;i<256;++i){
        int x0 = L + int(std::round((i/255.0)*PW));
        int x1 = L + int(std::round(((i+1)/255.0)*PW));
        int w = std::max(1, x1-x0);
        int bar_h = (hmax>0)? int(std::round((h[i]/(double)hmax)*PH)) : 0;
        cairo_rectangle(cr, x0, H-B-bar_h, w, bar_h);
    }
    cairo_set_source_rgb(cr, 0,0,0);
    cairo_fill(cr);

    g_object_unref(layout);
    pango_font_description_free(fd);

    GdkPixbuf* px = gdk_pixbuf_get_from_surface(surf, 0,0,W,H);
    cairo_destroy(cr);
    cairo_surface_destroy(surf);
    return px;
}

GdkPixbuf* draw_curve(const std::vector<double>& y, const char* /*title*/){
    const int W=ui::View::TILE_W, H=ui::View::TILE_H;
    const int L=50, B=28, R=10, T=10;
    cairo_surface_t* surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, W, H);
    cairo_t* cr = cairo_create(surf);
    cairo_set_source_rgb(cr, 1,1,1); cairo_paint(cr);
    int PW=W-L-R, PH=H-T-B;
    double ymax=0.0; for(double v:y) if(v>ymax) ymax=v; if (ymax<=0) ymax=1.0;

    // axes
    cairo_set_source_rgb(cr,0,0,0);
    cairo_move_to(cr, L, H-B); cairo_line_to(cr, W-R, H-B); cairo_stroke(cr);
    cairo_move_to(cr, L, H-B); cairo_line_to(cr, L, T); cairo_stroke(cr);

    PangoLayout* layout = pango_cairo_create_layout(cr);
    PangoFontDescription* fd = pango_font_description_from_string("Sans 9");
    pango_layout_set_font_description(layout, fd);

    for(int j=0;j<=5;++j){
        double val = (ymax * j)/5.0;
        int ypix = H-B - int(std::round((val / ymax) * PH));
        cairo_move_to(cr, L-5, ypix); cairo_line_to(cr, L, ypix); cairo_stroke(cr);
        char buf[32]; snprintf(buf, sizeof(buf), "%.3f", val);
        pango_layout_set_text(layout, buf, -1);
        int tw,th; pango_layout_get_pixel_size(layout, &tw, &th);
        cairo_move_to(cr, L-10-tw, ypix - th/2); pango_cairo_show_layout(cr, layout);
    }
    int xticks[] = {0,50,100,150,200,255};
    for (int k=0;k<6;++k){
        int i=xticks[k];
        int x = L + int(std::round((i/255.0)*PW));
        cairo_move_to(cr, x, H-B); cairo_line_to(cr, x, H-B+5); cairo_stroke(cr);
        char buf[16]; snprintf(buf, sizeof(buf), "%d", i);
        pango_layout_set_text(layout, buf, -1);
        int tw,th; pango_layout_get_pixel_size(layout, &tw, &th);
        cairo_move_to(cr, x - tw/2, H-B+6); pango_cairo_show_layout(cr, layout);
    }

    // polyline
    cairo_set_source_rgb(cr,0,0,0); cairo_set_line_width(cr,1.5);
    for (int i=0;i<256;++i){
        int x = L + int(std::round((i/255.0)*PW));
        int ypix = H-B - int(std::round((y[i]/ymax)*PH));
        if (i==0) cairo_move_to(cr, x, ypix);
        else cairo_line_to(cr, x, ypix);
    }
    cairo_stroke(cr);

    g_object_unref(layout);
    pango_font_description_free(fd);

    GdkPixbuf* px = gdk_pixbuf_get_from_surface(surf, 0,0,W,H);
    cairo_destroy(cr);
    cairo_surface_destroy(surf);
    return px;
}

} // anon

namespace ui {

static GdkPixbuf* scale_to_box(GdkPixbuf* src, int bw, int bh){
    if (!src) return nullptr;
    int w=gdk_pixbuf_get_width(src), h=gdk_pixbuf_get_height(src);
    double sx=(double)bw/w, sy=(double)bh/h, s= sx<sy? sx: sy;
    int nw=std::max(1,(int)std::round(w*s)), nh=std::max(1,(int)std::round(h*s));
    return gdk_pixbuf_scale_simple(src, nw, nh, GDK_INTERP_BILINEAR);
}

void Controller::connect_signals(){
    g_signal_connect(view_.btn_open(), "clicked", G_CALLBACK(+[](GtkButton*, gpointer self){
        reinterpret_cast<Controller*>(self)->on_open_image();
    }), this);
    g_signal_connect(view_.btn_apply(), "clicked", G_CALLBACK(+[](GtkButton*, gpointer self){
        reinterpret_cast<Controller*>(self)->on_apply_custom();
    }), this);
    g_signal_connect(view_.btn_equalize(), "clicked", G_CALLBACK(+[](GtkButton*, gpointer self){
        reinterpret_cast<Controller*>(self)->on_equalize();
    }), this);
}

bool Controller::ensure_gray(){
    if (!img_.valid()) return false;
    rgb_to_gray(img_.view(), img_gray_);
    return true;
}

void Controller::on_open_image(){
    GtkWidget* dlg = gtk_file_chooser_dialog_new(
        "Seleccionar imagen",
        GTK_WINDOW(view_.window()),
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
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg), filter);

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT){
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        core::ImageBuffer tmp;
        if (io::load_image_to_buffer(filename, tmp)){
            img_ = std::move(tmp);
            GdkPixbuf* px = model::to_pixbuf(img_.view());
            GdkPixbuf* scaled = scale_to_box(px, View::LEFT_IMG_W, View::LEFT_IMG_H);
            view_.set_input_image(scaled ? scaled : px);
            if (scaled) g_object_unref(scaled);
            g_object_unref(px);
            compute_and_draw_all();
        }
        g_free(filename);
    }
    gtk_widget_destroy(dlg);
}

void Controller::compute_and_draw_all(){
    if (!ensure_gray()) return;
    auto v = img_gray_.view();
    std::vector<uint32_t> h(256,0);
    int W=v.width(), H=v.height();
    for (int y=0;y<H;++y){
        const uint8_t* row=v.pixel(0,y);
        for (int x=0;x<W;++x){ h[row[0]]++; row+=3; }
    }
    uint32_t total = (uint32_t)W * (uint32_t)H;
    std::vector<double> pdf(256,0.0), cdf(256,0.0);
    for (int i=0;i<256;++i) pdf[i]=h[i]/(double)total;
    double acc=0.0; for (int i=0;i<256;++i){ acc+=pdf[i]; cdf[i]=acc; }

    GdkPixbuf* pxh = draw_hist(h, "h[i]");
    GdkPixbuf* pxpdf = draw_curve(pdf, "phi[i]");
    GdkPixbuf* pxcdf = draw_curve(cdf, "cdf[i]");
    view_.set_hist_pixbuf(pxh); view_.set_pdf_pixbuf(pxpdf); view_.set_cdf_pixbuf(pxcdf);
    g_object_unref(pxh); g_object_unref(pxpdf); g_object_unref(pxcdf);

    auto st = compute_stats(h, total);
    std::ostringstream oss; oss.setf(std::ios::fixed); oss.precision(4);
    oss << "N=" << total << "\n"
        << "Mín: " << st.minv << "  Máx: " << st.maxv << "  Moda: " << st.mode << "\n"
        << "Media: " << st.mean << "  Mediana: " << st.median << "\n"
        << "Varianza: " << st.var << "  Desv.Estd: " << st.stddev << "\n"
        << "Asimetría (skew): " << st.skew << "  Curtosis: " << st.kurt << "\n"
        << "Entropía: " << st.entropy << " bits\n";
    view_.set_stats_text(oss.str());
}

void Controller::on_equalize(){
    if (!ensure_gray()) return;
    auto v = img_gray_.view();
    std::vector<uint32_t> h(256,0);
    int W=v.width(), H=v.height();
    for (int y=0;y<H;++y){
        const uint8_t* row=v.pixel(0,y);
        for (int x=0;x<W;++x){ h[row[0]]++; row+=3; }
    }
    uint32_t total = (uint32_t)W * (uint32_t)H;
    std::vector<double> pdf(256,0.0), cdf(256,0.0);
    for (int i=0;i<256;++i) pdf[i]=h[i]/(double)total;
    double acc=0.0; for (int i=0;i<256;++i){ acc+=pdf[i]; cdf[i]=acc; }

    std::vector<uint8_t> lut(256);
    for(int i=0;i<256;++i) lut[i]=(uint8_t)std::min(255.0, std::max(0.0, std::round(255.0*cdf[i])));

    core::ImageBuffer out(W,H,3);
    for (int y=0;y<H;++y){
        const uint8_t* s=v.pixel(0,y);
        uint8_t* d=out.data()+y*out.stride();
        for (int x=0;x<W;++x){
            uint8_t g=s[0]; uint8_t e=lut[g];
            d[0]=d[1]=d[2]=e; s+=3; d+=3;
        }
    }
    GdkPixbuf* px = model::to_pixbuf(out.view());
    GdkPixbuf* scaled = scale_to_box(px, View::TILE_W, View::TILE_H);
    view_.set_equalized_image(scaled ? scaled : px);
    if (scaled) g_object_unref(scaled);
    g_object_unref(px);

    std::vector<uint32_t> h2(256,0);
    for (int y=0;y<H;++y){
        const uint8_t* row=out.view().pixel(0,y);
        for (int x=0;x<W;++x){ h2[row[0]]++; row+=3; }
    }
    std::vector<double> pdf2(256,0.0);
    for (int i=0;i<256;++i) pdf2[i]=h2[i]/(double)total;
    GdkPixbuf* pxpdf2 = draw_curve(pdf2, "phi eq");
    view_.set_pdf_eq_pixbuf(pxpdf2);
    g_object_unref(pxpdf2);
}

void Controller::on_apply_custom(){
    if (!ensure_gray()) return;
    int idx = gtk_combo_box_get_active(GTK_COMBO_BOX(view_.cmb_transform()));
    double gamma = gtk_range_get_value(GTK_RANGE(view_.scale_gamma()));
    std::vector<uint8_t> lut(256);
    for(int i=0;i<256;++i){
        double r=i/255.0, s=0.0;
        switch(idx){
            case 0: s = 1.0 - r; break;
            case 1: s = std::log(1.0 + 9.0*r)/std::log(10.0); break;
            case 2: s = std::pow(r, (gamma>0?gamma:1.0)); break;
            case 3: {
                auto v = img_gray_.view();
                std::vector<uint32_t> h(256,0);
                for(int y=0;y<v.height();++y){
                    const uint8_t* row=v.pixel(0,y);
                    for(int x=0;x<v.width();++x){ h[row[0]]++; row+=3; }
                }
                uint32_t total=(uint32_t)v.width()*(uint32_t)v.height();
                uint32_t p2=(uint32_t)(0.02*total), p98=(uint32_t)(0.98*total);
                uint32_t acc=0; int rmin=0,rmax=255;
                for (int k=0;k<256;++k){ acc+=h[k]; if(acc>=p2){ rmin=k; break; } }
                acc=0; for (int k=0;k<256;++k){ acc+=h[k]; if(acc>=p98){ rmax=k; break; } }
                double rr=(double)i;
                if (rmax>rmin) s = std::clamp((rr - rmin)/(double)(rmax-rmin), 0.0, 1.0);
                else s = r;
            } break;
        }
        lut[i]=(uint8_t)std::round(s*255.0);
    }

    auto v = img_gray_.view();
    core::ImageBuffer out(v.width(), v.height(), 3);
    for(int y=0;y<v.height();++y){
        const uint8_t* s=v.pixel(0,y);
        uint8_t* d=out.data()+y*out.stride();
        for(int x=0;x<v.width();++x){
            uint8_t e = lut[s[0]];
            d[0]=d[1]=d[2]=e;
            s+=3; d+=3;
        }
    }
    GdkPixbuf* px = model::to_pixbuf(out.view());
    GdkPixbuf* scaled = scale_to_box(px, View::TILE_W, View::TILE_H);
    view_.set_custom_image(scaled ? scaled : px);
    if (scaled) g_object_unref(scaled);
    g_object_unref(px);

    std::vector<uint32_t> h2(256,0);
    for(int y=0;y<v.height();++y){
        const uint8_t* row=out.view().pixel(0,y);
        for (int x=0;x<v.width();++x){ h2[row[0]]++; row+=3; }
    }
    GdkPixbuf* pxh = draw_hist(h2, "h2");
    view_.set_custom_hist(pxh);
    g_object_unref(pxh);
}

void Controller::show(){ view_.show(); }

} // namespace ui
