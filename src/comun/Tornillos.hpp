#pragma once
#include "plugin.hpp"

// agrega los 4 tornillos estandar a un
// ModuleWidget, en las esquinas
inline void agregarTornillos(
    ModuleWidget *widget)
{
    widget->addChild(
        createWidget<ScrewBlack>(
            Vec(RACK_GRID_WIDTH, 0)));

    widget->addChild(
        createWidget<ScrewBlack>(
            Vec(widget->box.size.x -
                    2 * RACK_GRID_WIDTH,
                0)));

    widget->addChild(
        createWidget<ScrewBlack>(
            Vec(RACK_GRID_WIDTH,
                RACK_GRID_HEIGHT -
                    RACK_GRID_WIDTH)));

    widget->addChild(
        createWidget<ScrewBlack>(
            Vec(widget->box.size.x -
                    2 * RACK_GRID_WIDTH,
                RACK_GRID_HEIGHT -
                    RACK_GRID_WIDTH)));
}