#pragma once

#include "rengine.h"

using namespace rengine;
using namespace std;

RENGINE_BEGIN_NAMESPACE

inline void animation_rotateZ(AnimationManager *manager, TransformNode *node, float duration)
{
    AnimationClosure<TransformNode, LinearTimingFunction> *anim
        = new AnimationClosure<TransformNode, LinearTimingFunction>(node);
    anim->setDuration(duration);
    anim->setIterations(-1);
    anim->keyFrames.times() << 0 << 1;
    anim->keyFrames.addValues<double, TransformNode_rotateAroundZ>() << 0 << M_PI * 2.0;
    manager->startAnimation(anim);
}

inline Texture *rengine_fractalTexture(Renderer *renderer, const vec2 &size)
{
    unsigned r = rand() % 256;
    unsigned g = rand() % 256;
    unsigned b = rand() % 256;

    float cy = (rand() % 100) / 100.0;
    float cx = (rand() % 100) / 100.0;
    int iter = 50;

    int w = size.x;
    int h = size.y;
    unsigned *bits = new unsigned[w * h];
    unsigned stride = w;

    for (int iy=0; iy<h; ++iy) {
        float ty = iy / (float) h;
        for (int ix=0; ix<w; ++ix) {
            float tx = ix / (float) w;

            float zx = 2.0 * (tx - 0.5);
            float zy = 3.0 * (ty - 0.5);
            int i = 0;
            for (i=0; i<iter; ++i) {
                float x = (zx * zx - zy * zy) + cx;
                float y = (zy * zx + zx * zy) + cy;

                if (x*x + y*y > 4.0) break;
                zx = x;
                zy = y;
            }

            float fv = i / float(iter);
            if (fv > 1.0)
                fv = 1.0;

            unsigned pa = (int) (fv * 255);
            unsigned pr = (int) (fv * r);
            unsigned pg = (int) (fv * g);
            unsigned pb = (int) (fv * b);
            bits[ix + iy * stride] = (pa << 24) | (pb << 16) | (pg << 8) | (pr);
        }
    }

    Texture *texture = renderer->createTextureFromImageData(vec2(w, h), Texture::RGBA_32, bits);
    delete [] bits;
    return texture;
}

inline Texture *rengine_loadImage(Renderer *renderer, const char *file)
{
    // read the image...
    int w, h, n;
    char location[1024];
    sprintf(location, "../examples/images/%s", file);
    unsigned char *data = stbi_load(location, &w, &h, &n, 4);
    if (!data) {
        sprintf(location, "examples/images/%s", file);
        data = stbi_load(location, &w, &h, &n, 4);
    }
    if (!data) {
        cout << "Failed to find the image 'walker.png' under 'examples/images' or '../examples/images'. "
             << "We're a bit dumb, you see, and can't find images unless you've built directly in the "
             << "source directory or in a direct subdirectory, like 'build' or 'debug'..." << endl;
        exit(1);
    }

    // Premultiply it...
    for (int y=0; y<h; ++y)
        for (int x=0; x<w; ++x) {
            unsigned char *p = data + (y * w + x) * 4;
            unsigned a = p[3];
            p[0] = (unsigned(p[0]) * a) / 255;
            p[1] = (unsigned(p[1]) * a) / 255;
            p[2] = (unsigned(p[2]) * a) / 255;
        }

    Texture *layer = renderer->createTextureFromImageData(vec2(w,h), Texture::RGBA_32, data);
    assert(layer);
    STBI_FREE(data);
    return layer;
}

RENGINE_END_NAMESPACE