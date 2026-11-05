#pragma once
#include "strutture.h"
#include "inizializzazioni.h"
extern float Tens, Bias, Cont;
extern Curva* curva;
extern float* t;
extern bool  visualizzaTg, modTg;




void ImpostaChiusuraC1(Curva* c)
{
    if (c->CP.size() < 3 || c->chiusaC1) return;
    c->chiusaC1 = true;
    c->CP.push_back(c->CP.front());
    c->colCP.push_back(c->colCP.front());
    c->Tens.push_back(c->Tens.front());
    c->Bias.push_back(c->Bias.front());
    c->Cont.push_back(c->Cont.front());
    c->Derivata.push_back(c->Derivata.front());
    c->ncp = static_cast<int>(c->CP.size());
}

void AggiornaChiusuraC1(Curva* c, float* parametri)
{
    if (!c->chiusaC1 || c->CP.size() < 3 || !parametri) return;
    const size_t last = c->CP.size() - 1;
    c->CP[last] = c->CP[0];
    c->colCP[last] = c->colCP[0];
    // Tangente periodica: media delle due secanti rispetto al parametro
    // globale. E' la stessa derivata ai due lati della giunzione.
    const float h0 = parametri[1] - parametri[0];
    const float h1 = parametri[last] - parametri[last - 1];
    if (h0 <= 1.e-7f || h1 <= 1.e-7f) {
        return;
        return;
    }
    const vec3 avanti = (c->CP[1] - c->CP[0]) / h0;
    const vec3 indietro = (c->CP[last] - c->CP[last - 1]) / h1;
    const vec3 tangenteChiusuraC1 = 0.5f * (1.0f - c->Tens[0]) *
        ((1.0f - c->Bias[0]) * avanti + (1.0f + c->Bias[0]) * indietro);
    c->Derivata[0] = tangenteChiusuraC1;
    c->Derivata[last] = tangenteChiusuraC1;
}

int pval = 200;  //Numero dei valori del parametro in cui visualizzare la curva


void t_equidistanti(Curva* curva) {
	int i;
	float step_t;
	delete[] t;
	t = new float[curva->CP.size()];
	step_t = 1.0 / (curva->CP.size() - 1);
	for (i = 0; i < curva->CP.size(); i++)
		t[i] = (float)i * step_t;
	t[curva->CP.size()-1] = 1.0;

}
void t_corde(Curva* curva)
{
	int i;
	float step_t;
	delete[] t;
	t = new float[curva->CP.size()];
	t[0] = 0;
	for (i = 1; i < curva->CP.size(); i++)
		t[i] = t[i - 1] + distance(curva->CP[i - 1], curva->CP[i]);

	for (i = 0; i < curva->CP.size(); i++)
		t[i] = t[i] / std::max(t[curva->CP.size() - 1], 1.e-7f);

}

/// /////////////////////////////////// Disegna geometria //////////////////////////////////////
//Per Curve di hermite
#define PHI0(t)  (2.0*t*t*t-3.0*t*t+1)
#define PHI1(t)  (t*t*t-2.0*t*t+t)
#define PSI0(t)  (-2.0*t*t*t+3.0*t*t)
#define PSI1(t)  (t*t*t-t*t)


float dx(int i, float* t, Curva* curva)
{
	if (i == 0)
		return  0.5 * (1.0 - curva->Tens[i]) * (1.0 - curva->Bias[i]) * (1.0 - curva->Cont[i]) * (curva->CP[i + 1].x - curva->CP[i].x) / (t[i + 1] - t[i]);
	if (i == curva->CP.size() - 1)
		return  0.5 * (1.0 - curva->Tens[i]) * (1.0 - curva->Bias[i]) * (1.0 - curva->Cont[i]) * (curva->CP[i].x - curva->CP[i - 1].x) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1.0 - curva->Tens[i]) * (1.0 + curva->Bias[i]) * (1.0 + curva->Cont[i]) * (curva->CP.at(i).x - curva->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - curva->Tens[i]) * (1 - curva->Bias[i]) * (1 - curva->Cont[i]) * (curva->CP.at(i + 1).x - curva->CP.at(i).x) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1.0 - curva->Tens[i]) * (1.0 + curva->Bias[i]) * (1.0 - curva->Cont[i]) * (curva->CP.at(i).x - curva->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - curva->Tens[i]) * (1 - curva->Bias[i]) * (1 + curva->Cont[i]) * (curva->CP.at(i + 1).x - curva->CP.at(i).x) / (t[i + 1] - t[i]);
}
float dy(int i, float* t,  Curva* curva)
{
	if (i == 0)
		return 0.5 * (1.0 - curva->Tens[i]) * (1.0 - curva->Bias[i]) * (1.0 - curva->Cont[i]) * (curva->CP.at(i + 1).y - curva->CP.at(i).y) / (t[i + 1] - t[i]);
	if (i == curva->CP.size() - 1)
		return  0.5 * (1.0 - curva->Tens[i]) * (1.0 - curva->Bias[i]) * (1.0 - curva->Cont[i]) * (curva->CP.at(i).y - curva->CP.at(i - 1).y) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1.0 - curva->Tens[i]) * (1.0 + curva->Bias[i]) * (1.0 + curva->Cont[i]) * (curva->CP.at(i).y - curva->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - curva->Tens[i]) * (1 - curva->Bias[i]) * (1 - curva->Cont[i]) * (curva->CP.at(i + 1).y - curva->CP.at(i).y) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1.0 - curva->Tens[i]) * (1.0 + curva->Bias[i]) * (1.0 - curva->Cont[i]) * (curva->CP.at(i).y - curva->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - curva->Tens[i]) * (1 - curva->Bias[i]) * (1 + curva->Cont[i]) * (curva->CP.at(i + 1).y - curva->CP.at(i).y) / (t[i + 1] - t[i]);
}

float DX(int i, float* t, Curva* curva)
{
	//Nei vertici di controllo per i quali non sono stati modificati i parametri Tens, Bias, Cont il valore della derivata della componente x della curva è quello originale, altrimenti è quello che è stato modificato nella funzione 
	//keyboardfunc  in seguito alla modifica dei valori Tens, Bias e Cont.
	 
	if (curva->chiusaC1 && (i == 0 || i == static_cast<int>(curva->CP.size()) - 1)) return curva->Derivata[0].x;
	if (curva->Derivata.at(i).x == 0)
	   return dx(i, t, curva);
	
	if (curva->Derivata.at(i).x != 0)
		return curva->Derivata.at(i).x;
	
	 
	
}

float DY(int i, float* t, Curva* curva)
{
	// Nei vertici di controllo per i quali non sono stati modificati i parametri Tens, Bias, Cont il valore della derivata della componente y della curva è quello originale, altrimenti è quello che è stato modificato nella funzione
		//keyboardfunc  in seguito alla modifica dei valori Tens, Bias e Cont.
	 
	if (curva->chiusaC1 && (i == 0 || i == static_cast<int>(curva->CP.size()) - 1)) return curva->Derivata[0].y;
	if (curva->Derivata.at(i).y == 0)
		return dy(i, t, curva);
	 
	if (curva->Derivata.at(i).y != 0)
		return curva->Derivata.at(i).y;

}

void InterpolazioneHermite(float* t, Curva* curva, vec4 color_top, vec4 color_bot)
{
	int i, is;
	float tg, x, y, ampiezza, tgmapp;
	float passotg = 1.0 / (float)(pval - 1);

	is = 0; // indice dell'estremo sinistro dell'intervallo [t_is, t_(is + 1)] a cui tg appartiene

	for (i = 0; i < pval; i++) {

		tg = (float)i * passotg;

		// localizzare nell'intervallo [t_is, t_(is + 1)]
		while (is < curva->CP.size() - 2 && tg > t[is + 1]) ++is;
		ampiezza = (t[is + 1] - t[is]);

		// mappare tg in [0, 1] --> tgmapp = (tg - t[is]) / t_(is + 1) - t_(is)
		tgmapp = (tg - t[is]) / ampiezza;

		// calcolare l'ascissa del punto sulla curva corrispondente al valore del parametro tg
		x = curva->CP[is].x * PHI0(tgmapp) + DX(is, t, curva) * PHI1(tgmapp) * ampiezza + curva->CP[is + 1].x * PSI0(tgmapp) + DX(is + 1, t, curva) * PSI1(tgmapp) * ampiezza;

		// calcolare l'ordinata del punto sulla curva corrispondente al valore del parametro tg
		y = curva->CP[is].y * PHI0(tgmapp) + DX(is, t, curva) * PHI1(tgmapp) * ampiezza + curva->CP[is + 1].y * PSI0(tgmapp) + DX(is + 1, t, curva) * PSI1(tgmapp) * ampiezza;

		curva->vertices.push_back(vec3(x, y, 0.0));
		curva->colors.push_back(color_bot);
		curva->nv = curva->vertices.size();
	}
	 
	 
	if (visualizzaTg)
	{
		for (size_t i = 0; i < curva->CP.size(); i++)
		{
			 //Individuo le coordinate  dell'estremo destro del segmento tangente nel vertice di controllo i.esimo: si trova a partire da CP[i] nella direzione della tangente 

			curva->tg.push_back(vec3(curva->CP[i].x + DX(i, t, curva) / 50, curva->CP[i].y + DY(i, t, curva) / 50, 0.0));
			curva->col_tg.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			//Individuo le coordinate  dell'estremo sinistro del segmento tangente nel vertice di controllo i.esimo: si trova a partire da CP[i] nella direzione della tangente (verso opposto)
			curva->tg.push_back(vec3(curva->CP[i].x - DX(i, t, curva) / 50, curva->CP[i].y - DY(i, t, curva) / 50, 0.0));
			curva->col_tg.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			curva->ntg = curva->tg.size();

		}
		UPDATE_VAO_DYNAMIC(curva, true);
	}
	  
	 
}

void CostruisciHermite( Curva* curva)
{
	curva->vertices.clear();
	curva->colors.clear();
	curva->tg.clear();
	curva->col_tg.clear();
	vec4 color_top = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 color_bot = vec4(1.0, 0.8, 1.0, 1.0);
	AggiornaChiusuraC1(curva, t);
	InterpolazioneHermite(t, curva, curva->coloreCurva, curva->coloreCurva);
	
	//Calcolo di una misura normalizzata della distanza di ciascun vertice
	//  dal  centroide (il punto medio geometrico) della curva. 
	vec3 centroid(0.0f);
	for (auto& v : curva->vertices)
		centroid += v;
	centroid /= curva->vertices.size();
	
	curva->distNormalized.clear();

	// Calcolo della distanza massima dal centro
	float maxDist = 0.0f;
	for (auto& v : curva->vertices) {
		float dist = distance(v, centroid);
		maxDist = std::max(maxDist, dist);
	}

	// Calcolo valori normalizzati per ogni vertice

	for (auto& v : curva->vertices) {
		float dist = glm::distance(v, centroid);
		curva->distNormalized.push_back(maxDist > 1.e-7f ? dist / maxDist : 0.0f);
	}
	UPDATE_VAO_DYNAMIC(curva,true);
}