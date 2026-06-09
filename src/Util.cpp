#include "Util.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>

namespace util {

void limparTela() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void titulo(const std::string& texto) {
    limparTela();
    std::cout << "========================================\n";
    std::cout << "            SISTEMA DE LOJA\n";
    std::cout << "========================================\n";
    std::cout << "  " << texto << "\n";
    std::cout << "========================================\n\n";
}

void pausar() {
    std::cout << "\nPressione ENTER para continuar...";
    std::string lixo;
    std::getline(std::cin, lixo);
}

int lerInteiro(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string linha;
        if (!std::getline(std::cin, linha)) return 0;   // fim da entrada (EOF)
        std::istringstream ss(linha);
        int valor;
        if (ss >> valor) return valor;
        std::cout << "Valor invalido. Digite um numero inteiro.\n";
    }
}

double lerDecimal(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string linha;
        if (!std::getline(std::cin, linha)) return 0.0;  // fim da entrada (EOF)
        std::istringstream ss(linha);
        double valor;
        if (ss >> valor) return valor;
        std::cout << "Valor invalido. Digite um numero.\n";
    }
}

std::string lerTexto(const std::string& prompt) {
    std::cout << prompt;
    std::string linha;
    std::getline(std::cin, linha);
    return linha;
}

std::string dataAtual() {
    std::time_t agora = std::time(nullptr);
    std::tm* lt = std::localtime(&agora);
    char buf[11];
    std::snprintf(buf, sizeof(buf), "%02d/%02d/%04d",
                  lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
    return std::string(buf);
}

} // namespace util
