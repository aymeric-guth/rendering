#include "matrix.h"
#include "types.h"

#ifndef SCENE_DATA_H
#define SCENE_DATA_H
#define SCENE_SIZE 12

static Tri _scene[] = {
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 1.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        },
        .c = COLOR_WHITE
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
        },
        .c = COLOR_WHITE
    },
};

#endif
