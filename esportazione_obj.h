#pragma once
#include <string>
// Esporta le curve chiuse come gruppi OBJ, con materiali RGB in un MTL.
// Le curve aperte o degeneri vengono saltate e segnalate.
bool EsportaFormaOBJ(const std::string& percorso, std::string& messaggio);
