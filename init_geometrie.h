/*
#pragma once //: È una direttiva che indica al compilatore di includere il file una sola volta per unità di compilazione.
//  Il meccanismo esatto varia a seconda del compilatore, ma in generale si basa su un meccanismo interno per tenere traccia dei file già inclusi.
//Non è parte dello standard C++, quindi la sua disponibilità e il comportamento preciso possono variare tra diversi compilatori. Tuttavia, è supportata dalla maggior parte dei compilatori moderni.
*/

#pragma once

#include "strutture.h"
void INIT_TRIANGLE(Figura* fig, vec4 color_top, vec4 color_bottom);
void INIT_CIRCLE(float cx, float cy, float raggiox, float raggioy, Figura* fig, vec4 color_top, vec4 color_bottom);
void INIT_BUTTERFLY(float cx, float cy, float raggiox, float raggioy, Figura* fig, vec4 color_top, vec4 color_bottom);
void INIT_HEART(float cx, float cy, float raggiox, float raggioy, Figura* fig, vec4 color_top, vec4 color_bottom);
void INIT_PIANO(Figura* fig, vec4 color_top, vec4 color_bottom);
void INIT_ASSI(Figura* fig);