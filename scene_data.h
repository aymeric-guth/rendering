#include "matrix.h"
#include "types.h"

#ifndef SCENE_DATA_H
#define SCENE_DATA_H
#define SCENE_SIZE 12

static Tri _scene[] = {
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
        },
    },
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
        },
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
        },
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
        },
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 1.0f },
        },
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 1.0f },
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
        },
    },
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
        },
    },
    {
        .v = {
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        },
    },
    {
        .v = {
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            { .x = 0.0f, .y = 1.0f, .z = 1.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
        },
    },
    {
        .v = {
            { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            { .x = 1.0f, .y = 1.0f, .z = 1.0f },
            { .x = 1.0f, .y = 1.0f, .z = 0.0f },
        },
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        },
    },
    {
        .v = {
            { .x = 1.0f, .y = 0.0f, .z = 1.0f },
            { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            { .x = 1.0f, .y = 0.0f, .z = 0.0f },
        },
    },
};

static Pixel _colors[SCENE_SIZE][3] = {
    {
        {
            .color = COLOR_RED,
            .shader = 1
        },
        {
            .color = COLOR_RED,
            .shader = 1
        },
        {
            .color = COLOR_RED,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_RED,
            .shader = 1
        },
        {
            .color = COLOR_RED,
            .shader = 1
        },
        {
            .color = COLOR_RED,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_CYAN,
            .shader = 1
        },
        {
            .color = COLOR_CYAN,
            .shader = 1
        },
        {
            .color = COLOR_CYAN,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_CYAN,
            .shader = 1
        },
        {
            .color = COLOR_CYAN,
            .shader = 1
        },
        {
            .color = COLOR_CYAN,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_GREEN,
            .shader = 1
        },
        {
            .color = COLOR_GREEN,
            .shader = 1
        },
        {
            .color = COLOR_GREEN,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_GREEN,
            .shader = 1
        },
        {
            .color = COLOR_GREEN,
            .shader = 1
        },
        {
            .color = COLOR_GREEN,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_BLUE,
            .shader = 1
        },
        {
            .color = COLOR_BLUE,
            .shader = 1
        },
        {
            .color = COLOR_BLUE,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_BLUE,
            .shader = 1
        },
        {
            .color = COLOR_BLUE,
            .shader = 1
        },
        {
            .color = COLOR_BLUE,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_YELLOW,
            .shader = 1
        },
        {
            .color = COLOR_YELLOW,
            .shader = 1
        },
        {
            .color = COLOR_YELLOW,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_YELLOW,
            .shader = 1
        },
        {
            .color = COLOR_YELLOW,
            .shader = 1
        },
        {
            .color = COLOR_YELLOW,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_WHITE,
            .shader = 1
        },
        {
            .color = COLOR_WHITE,
            .shader = 1
        },
        {
            .color = COLOR_WHITE,
            .shader = 1
        },
    },
    {
        {
            .color = COLOR_WHITE,
            .shader = 1
        },
        {
            .color = COLOR_WHITE,
            .shader = 1
        },
        {
            .color = COLOR_WHITE,
            .shader = 1
        },
    },
};

#endif
