#ifndef VENDA_HPP
#define VENDA_HPP

#include <string>

// Registro de uma venda no historico (uma linha por item vendido).
struct Venda {
    int         id          = 0;
    int         produtoId   = 0;
    std::string funcionario;       // usuario que efetuou a venda
    int         quantidade  = 0;
    double      total       = 0.0;
    std::string data;              // dd/mm/aaaa
};

#endif // VENDA_HPP
