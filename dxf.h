#ifndef DXF_H
#define DXF_H

#include <stdio.h>

// Start / end file
extern void dxf_begin(FILE *f);
extern void dxf_end(FILE *f);

// Basic entities
extern void dxf_point(FILE *f, double x, double y);
extern void dxf_line(FILE *f, double x1, double y1, double x2, double y2);
extern void dxf_circle(FILE *f, double x, double y, double r);

// Optional: layer support (simple version)
extern void dxf_set_layer(FILE *f, const char *layer);

#endif
