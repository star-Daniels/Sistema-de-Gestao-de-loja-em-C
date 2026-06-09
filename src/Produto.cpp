#include "Produto.hpp"

#include <cstdio>
#include <ctime>

bool Produto::vencendo() const {
    int dia, mes, ano;
    if (std::sscanf(validade.c_str(), "%d/%d/%d", &dia, &mes, &ano) != 3)
        return false;   // sem data valida: nao alerta

    std::time_t agora = std::time(nullptr);
    std::tm* lt = std::localtime(&agora);
    int anoAtual = lt->tm_year + 1900;
    int mesAtual = lt->tm_mon + 1;
    int diaAtual = lt->tm_mday;

    if (ano < anoAtual) return true;
    if (ano == anoAtual && mes < mesAtual) return true;
    if (ano == anoAtual && mes == mesAtual &&
        dia <= diaAtual + DIAS_ALERTA_VALIDADE) return true;
    return false;
}
