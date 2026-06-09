#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

// Funcoes utilitarias de interface (tela), leitura de entrada validada
// e datas. Agrupadas no namespace "util".
namespace util {

void limparTela();
void titulo(const std::string& texto);   // limpa a tela e imprime o cabecalho
void pausar();                            // aguarda o usuario pressionar ENTER

int         lerInteiro(const std::string& prompt);   // repete ate ler um inteiro
double      lerDecimal(const std::string& prompt);   // repete ate ler um numero
std::string lerTexto(const std::string& prompt);     // le uma linha inteira

std::string dataAtual();                  // data de hoje no formato "dd/mm/aaaa"

} // namespace util

#endif // UTIL_HPP
