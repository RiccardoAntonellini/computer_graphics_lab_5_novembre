#pragma once
#include "strutture.h"
void t_equidistanti(Curva* curva);
void t_corde(Curva* curva);
float dx(int i, float* t, Curva* curva);
float dy(int i, float* t, Curva* curva);
float DX(int i, float* t, Curva* curva);
float DY(int i, float* t, Curva* curva);
void InterpolazioneHermite(float* t, Curva* curva, vec4 color_top, vec4 color_bot);
void CostruisciHermite( Curva* curva);
// Chiusura periodica C1 (posizione e derivata rispetto al parametro globale).

void ImpostaChiusuraC1(Curva* curva);
void AggiornaChiusuraC1(Curva* curva, float* t);
