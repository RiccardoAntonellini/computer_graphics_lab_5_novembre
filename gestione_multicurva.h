#pragma once
#include "strutture.h"
#include <memory>
#include <vector>
#include <string>
extern std::vector<std::unique_ptr<Curva>> curve;
extern int curvaAttivaIndice;
extern Curva* curva;
void InizializzaCurve(int numero);
void SelezionaCurva(int indice);
void CambiaNumeroCurve(int numero);

// Formato testuale versionato: tutte le curve, incluse quelle vuote.
bool SalvaForma(const std::string& percorso, std::string& errore);
bool ApriForma(const std::string& percorso, std::string& errore);
