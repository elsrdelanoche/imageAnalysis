#include "BinaryOps.hpp"

using Glib::RefPtr;

static inline unsigned char clamp01(unsigned char v){ return v ? 255 : 0; }

namespace BinaryOps {

static RefPtr<Gdk::Pixbuf> make_out_like(const RefPtr<Gdk::Pixbuf>& A){
    return Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, A->get_width(), A->get_height());
}

Glib::RefPtr<Gdk::Pixbuf> logical(const RefPtr<Gdk::Pixbuf>& A, const RefPtr<Gdk::Pixbuf>& B,
                                  LogicalOp op, bool not_a, bool not_b) {
    auto out = make_out_like(A);
    int w = A->get_width(), h = A->get_height();
    int rsa = A->get_rowstride(), rsb = B->get_rowstride(), rso = out->get_rowstride();
    int nca = A->get_n_channels(), ncb=B->get_n_channels(), nco=out->get_n_channels();

    for(int y=0;y<h;++y){
        const unsigned char* pa = A->get_pixels() + y*rsa;
        const unsigned char* pb = B->get_pixels() + y*rsb;
        unsigned char* po = out->get_pixels() + y*rso;
        for(int x=0;x<w;++x){
            unsigned char va = pa[x*nca+0];
            unsigned char vb = pb[x*ncb+0];
            bool ba = va >= 128;
            bool bb = vb >= 128;
            if(not_a) ba = !ba;
            if(not_b) bb = !bb;
            bool br = false;
            switch(op){
                case LogicalOp::AND: br = (ba && bb); break;
                case LogicalOp::OR:  br = (ba || bb); break;
                case LogicalOp::XOR: br = (ba ^ bb);  break;
            }
            unsigned char vr = br ? 255 : 0;
            po[x*nco+0]=po[x*nco+1]=po[x*nco+2]=vr;
        }
    }
    return out;
}

Glib::RefPtr<Gdk::Pixbuf> relational(const RefPtr<Gdk::Pixbuf>& A, const RefPtr<Gdk::Pixbuf>& B, RelOp op) {
    auto out = make_out_like(A);
    int w = A->get_width(), h = A->get_height();
    int rsa = A->get_rowstride(), rsb = B->get_rowstride(), rso = out->get_rowstride();
    int nca = A->get_n_channels(), ncb=B->get_n_channels(), nco=out->get_n_channels();

    for(int y=0;y<h;++y){
        const unsigned char* pa = A->get_pixels() + y*rsa;
        const unsigned char* pb = B->get_pixels() + y*rsb;
        unsigned char* po = out->get_pixels() + y*rso;
        for(int x=0;x<w;++x){
            unsigned char va = pa[x*nca+0];
            unsigned char vb = pb[x*ncb+0];
            // compare raw intensities (post-binarización si se activó antes)
            bool cond=false;
            switch(op){
                case RelOp::EQ: cond = (va == vb); break;
                case RelOp::NE: cond = (va != vb); break;
                case RelOp::GT: cond = (va >  vb); break;
                case RelOp::GE: cond = (va >= vb); break;
                case RelOp::LT: cond = (va <  vb); break;
                case RelOp::LE: cond = (va <= vb); break;
            }
            unsigned char vr = cond ? 255 : 0;
            po[x*nco+0]=po[x*nco+1]=po[x*nco+2]=vr;
        }
    }
    return out;
}

}
