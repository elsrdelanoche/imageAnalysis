#include <gtk/gtk.h>
#include "app.hpp"

// Asegura que la función main esté en el namespace global
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    App app;
    app.show();
    gtk_main();
    return 0;
}
