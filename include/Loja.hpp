#ifndef LOJA_HPP
#define LOJA_HPP

#include <vector>
#include <string>
#include <optional>

#include "Produto.hpp"
#include "Usuario.hpp"
#include "Venda.hpp"
#include "LancamentoCaixa.hpp"
#include "Storage.hpp"

// Um item de um carrinho de venda (produto + quantidade).
struct ItemVenda {
    int produtoId  = 0;
    int quantidade = 0;
};

// Nucleo do sistema, MULTI-ORGANIZACAO. O login e so usuario + senha (o nome de
// usuario e unico no banco e leva direto a organizacao a que pertence). Apos
// entrar/registrar, guarda em memoria apenas os dados DESSA org. Cada org e
// isolada. O NOME da organizacao e editavel (so muda em Configuracoes).
class Loja {
public:
    explicit Loja(double percentualComissao = 0.05, const std::string& dbPath = "loja.db");

    // ---- Banco ----
    bool abrirBanco();   // abre o SQLite e garante o schema (nao carrega nenhuma org)

    // ---- Conta / organizacao / acesso ----
    // Cria uma conta nova: abre uma organizacao (nome inicial = o proprio usuario,
    // editavel depois) e o usuario vira o ADMIN dono, ja entrando logado.
    // false se faltar dado ou o usuario ja existir.
    bool registrarConta(const std::string& usuario, const std::string& senha);
    // Entra so com usuario + senha; resolve a organizacao automaticamente.
    bool entrar(const std::string& usuario, const std::string& senha);
    bool usuarioExiste(const std::string& usuario);
    bool renomearOrg(const std::string& nome);   // so o admin; muda o nome de exibicao
    void logout();
    bool temUsuarioLogado() const { return usuarioLogado_ >= 0; }
    const Usuario&     usuarioAtual() const;
    const std::string& org()  const { return orgNome_; }   // nome de exibicao
    const std::string& dono() const { return dono_; }
    bool               ehDono(const std::string& usuario) const { return !dono_.empty() && usuario == dono_; }

    // ---- Produtos ----
    int                          proximoIdProduto() const;
    Produto*                     produtoPorId(int id);
    const std::vector<Produto>&  produtos() const { return produtos_; }
    void                         adicionarProduto(const Produto& p);
    bool                         removerProduto(int id);
    bool                         atualizarEstoque(int id, int novoEstoque);
    std::vector<const Produto*>  buscarProdutos(const std::string& termo) const;

    // ---- Usuarios (da org atual) ----
    const std::vector<Usuario>&  usuarios() const { return usuarios_; }
    bool                         adicionarUsuario(const Usuario& u);  // sempre FUNC; false se o nome ja existir (global)
    bool                         definirCargo(const std::string& usuario, Cargo novo);
    void                         definirMeta(double valor);           // do usuario logado
    bool                         mudarSenha(const std::string& atual, const std::string& nova); // do usuario logado

    // ---- Vendas / caixa ----
    std::optional<double> registrarVenda(const std::vector<ItemVenda>& itens);
    double                fecharCaixa();   // grava o fechamento e zera; devolve o total
    double                saldoCaixa() const;
    const std::vector<LancamentoCaixa>& lancamentosCaixa() const { return caixa_; }

    // ---- Relatorios ----
    const std::vector<Venda>& historico() const { return historico_; }
    double                    faturamentoTotal() const;
    double                    lucroEstimado() const;
    int                       totalVendidoDoProduto(int produtoId) const;

    // ---- Configuracao ----
    double percentualComissao() const { return percentualComissao_; }

private:
    void salvar();   // persiste o estado da org atual (Storage, escopado a orgId_)

    std::vector<Produto>         produtos_;
    std::vector<Usuario>         usuarios_;
    std::vector<Venda>           historico_;
    std::vector<LancamentoCaixa> caixa_;
    double                       percentualComissao_;
    int                          usuarioLogado_ = -1;
    int                          orgId_ = -1;
    std::string                  orgNome_;
    std::string                  dono_;
    Storage                      storage_;
};

#endif // LOJA_HPP
