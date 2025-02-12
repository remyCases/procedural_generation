// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include "../include/save.h"

static int save_png_libpng(const char* filename, uint8_t *pixels, int w, int h)
{
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) 
    {
        return PG_ALLOCATION_ERROR;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) 
    {
        png_destroy_write_struct(&png, &info);
        return PG_ALLOCATION_ERROR;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) 
    {
        png_destroy_write_struct(&png, &info);
        return PG_ACCESS_DENIED;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, w, h, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_colorp palette = (png_colorp)png_malloc(png, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));

    if (!palette) 
    {
        fclose(fp);
        png_destroy_write_struct(&png, &info);
        return PG_ALLOCATION_ERROR;
    }
    png_set_PLTE(png, info, palette, PNG_MAX_PALETTE_LENGTH);
    png_write_info(png, info);
    png_set_packing(png);

    png_bytepp rows = (png_bytepp)png_malloc(png, h * sizeof(png_bytep));

    if (!rows)
    {
        fclose(fp);
        png_destroy_write_struct(&png, &info);
        png_free(png, palette);
        return PG_ALLOCATION_ERROR;
    }

    for (int i = 0; i < h; ++i) 
    {
        rows[i] = (png_bytep)(pixels + (h - i - 1) * w * 3);
    }

    png_write_image(png, rows);
    png_write_end(png, info);

    // cleanup
    png_free(png, palette);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    free(rows);

    printf("[>] Saved %s.\n", filename);
    return PG_SUCCESS;
}

int save_png(const char* filename, data_t* data)
{
    int last_status = PG_SUCCESS;

    int w = data->state.width;
    int h = data->state.height;
    uint8_t* pixels = (uint8_t*)malloc(sizeof(uint8_t)*(w * h * 3));
    // copy pixels from screen
    //glBindTexture(GL_TEXTURE_2D, screenTex);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

    // save the image
    CHECK_CALL(save_png_libpng, filename, pixels, w, h);

    return last_status;
}