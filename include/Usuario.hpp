#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>
#include <algorithm>
#include <cctype>

// Nivel de acesso do usuario dentro da sua organizacao.
enum class Cargo { ADMIN, FUNC };

inline std::string textoDeCargo(Cargo c) {
    return c == Cargo::ADMIN ? "ADMIN" : "FUNC";
}

inline Cargo cargoDeTexto(std::string texto) {
    std::transform(texto.begin(), texto.end(), texto.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
    return (texto == "ADMIN") ? Cargo::ADMIN : Cargo::FUNC;
}

// Usuario do sistema. Pertence a UMA organizacao (a coluna "org" fica no banco;
// em memoria a Loja so carrega os usuarios da org logada). Quando funcionario,
// tambem acumula o desempenho de vendas (total vendido e meta).
struct Usuario {
    std::string usuario;
    std::string senha;
    Cargo       cargo        = Cargo::FUNC;
    double      totalVendido = 0.0;   // soma das vendas do funcionario
    double      meta         = 0.0;   // meta de vendas definida (0 = sem meta)

    bool   isAdmin() const                          { return cargo == Cargo::ADMIN; }
    bool   senhaConfere(const std::string& s) const { return senha == s; }
    double comissao(double percentual) const        { return totalVendido * percentual; }
    bool   metaAtingida() const                      { return meta > 0.0 && totalVendido >= meta; }
};

#endif // USUARIO_HPP
