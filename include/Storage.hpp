#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include <vector>

#include "Produto.hpp"
#include "Usuario.hpp"
#include "Venda.hpp"
#include "LancamentoCaixa.hpp"

struct sqlite3;  // forward decl -- a definicao vem do sqlite3.h dentro do .cpp

// Snapshot dos dados de UMA organizacao.
struct DadosLoja {
    std::vector<Produto>         produtos;
    std::vector<Usuario>         usuarios;
    std::vector<Venda>           vendas;
    std::vector<LancamentoCaixa> caixa;
};

// Persistencia em SQLite, MULTI-ORGANIZACAO. O inquilino e identificado por um
// "org_id" (inteiro estavel); a organizacao tem um NOME editavel. O login e so
// usuario + senha: o nome de usuario e UNICO no banco inteiro (PRIMARY KEY) e
// aponta para o org_id ao qual pertence. Cada tabela de dados carrega org_id e
// e filtrada/gravada por organizacao -> orgs nunca enxergam dados umas das outras.
class Storage {
public:
    explicit Storage(const std::string& caminho = "loja.db");
    ~Storage();

    Storage(const Storage&)            = delete;
    Storage& operator=(const Storage&) = delete;

    bool abrir();                                  // abre o arquivo e garante o schema

    // ---- Contas / organizacoes ----
    bool        usuarioExiste(const std::string& usuario);                     // global
    int         orgDoUsuario(const std::string& usuario, const std::string& senha); // org_id se autentica, senao -1
    int         criarOrg(const std::string& nome, const std::string& dono);    // -> novo org_id (-1 se falhar)
    std::string orgNome(int orgId);
    std::string orgDono(int orgId);
    void        renomearOrg(int orgId, const std::string& nome);

    // ---- Dados de uma organizacao ----
    void carregar(int orgId, DadosLoja& dados);       // SELECT ... WHERE org_id=?
    void salvar(int orgId, const DadosLoja& dados);   // reescreve SO esta org
    void registrarFechamento(int orgId, const std::string& data, double total);

private:
    std::string     caminho_;
    struct sqlite3* db_ = nullptr;

    void exec(const char* sql);                   // roda SQL ou lanca runtime_error
};

#endif // STORAGE_HPP
