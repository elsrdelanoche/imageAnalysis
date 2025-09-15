#include <gtk/gtk.h>
#include "app.hpp"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    App app;
    app.show();
    gtk_main();
    return 0;
}
