#ifndef PRODUTO_HPP
#define PRODUTO_HPP

#include <string>

// Um produto da loja.
class Produto {
public:
    // Limites de alerta (constantes nomeadas em vez de numeros magicos).
    static constexpr int ESTOQUE_MINIMO       = 5;   // abaixo disso: alerta de estoque baixo
    static constexpr int DIAS_ALERTA_VALIDADE = 7;   // vence em ate N dias: alerta

    int         id       = 0;
    std::string nome;
    double      preco    = 0.0;   // preco de venda
    double      custo    = 0.0;   // custo de compra
    int         estoque  = 0;
    std::string validade;         // formato dd/mm/aaaa

    bool estoqueBaixo() const { return estoque < ESTOQUE_MINIMO; }
    bool vencendo() const;     // true se vence em ate DIAS_ALERTA_VALIDADE dias (ou ja venceu)
};

#endif // PRODUTO_HPP
