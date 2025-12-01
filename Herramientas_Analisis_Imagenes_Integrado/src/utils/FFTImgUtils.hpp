#pragma once
#include <gtkmm.h>
#include <vector>

namespace FFTImgUtils {

// Ensure square power-of-two using bilinear scale.
Glib::RefPtr<Gdk::Pixbuf> ToSquarePow2(const Glib::RefPtr<Gdk::Pixbuf>& src, int& N_out);

// Convert Pixbuf (RGB8) to grayscale double [0,1], width=height=N
std::vector<double> PixbufToGray(const Glib::RefPtr<Gdk::Pixbuf>& p);

// Convert grayscale [0,1] N×N to Pixbuf RGB8
Glib::RefPtr<Gdk::Pixbuf> GrayToPixbuf(const std::vector<double>& g, int N);

// Visualize magnitude (log) to Pixbuf, auto-normalized
Glib::RefPtr<Gdk::Pixbuf> MagToPixbuf(const std::vector<double>& mag, int N);

// Visualize phase [-pi,pi] to Pixbuf (wrapped to [0,1])
Glib::RefPtr<Gdk::Pixbuf> PhaseToPixbuf(const std::vector<double>& phase, int N);

int NextPow2(int v);
bool IsPow2(int v);

} //  namespace
