#ifndef LANCAMENTO_CAIXA_HPP
#define LANCAMENTO_CAIXA_HPP

#include <string>

// Um lancamento no caixa (por enquanto, sempre uma venda).
struct LancamentoCaixa {
    std::string tipo;          // ex.: "VENDA"
    double      valor = 0.0;
};

#endif // LANCAMENTO_CAIXA_HPP
