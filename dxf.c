#include "dxf.h"

static const char *current_layer = "0";

void dxf_set_layer(FILE *f, const char *layer) {
    (void)f; // unused for now
    current_layer = layer;
}

void dxf_begin(FILE *f) {
    fprintf(f, "0\nSECTION\n2\nENTITIES\n");
}

void dxf_end(FILE *f) {
    fprintf(f, "0\nENDSEC\n0\nEOF\n");
}

void dxf_point(FILE *f, double x, double y) {
    if (!f) return;

    fprintf(f, "0\nPOINT\n");
    fprintf(f, "8\n%s\n", current_layer); // layer
    fprintf(f, "10\n%.6f\n", x); // X
    fprintf(f, "20\n%.6f\n", y); // Y
    fprintf(f, "30\n%.6f\n", 0.0); // Z
}

void dxf_line(FILE *f, double x1, double y1, double x2, double y2) {
    fprintf(f, "0\nLINE\n");
    fprintf(f, "8\n%s\n", current_layer);
    fprintf(f, "10\n%f\n20\n%f\n", x1, y1);
    fprintf(f, "11\n%f\n21\n%f\n", x2, y2);
}

void dxf_circle(FILE *f, double x, double y, double r) {
    fprintf(f, "0\nCIRCLE\n");
    fprintf(f, "8\n%s\n", current_layer);
    fprintf(f, "10\n%f\n20\n%f\n", x, y);
    fprintf(f, "40\n%f\n", r);
}
