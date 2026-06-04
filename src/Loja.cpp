#include "Loja.hpp"
#include "Util.hpp"

#include <algorithm>
#include <utility>

Loja::Loja(double percentualComissao, const std::string& dbPath)
    : percentualComissao_(percentualComissao), storage_(dbPath) {}

// ----------------------- Banco -----------------------

bool Loja::abrirBanco() {
    return storage_.abrir();
}

// ----------------------- Conta / organizacao / acesso -----------------------

bool Loja::usuarioExiste(const std::string& usuario) {
    return storage_.usuarioExiste(usuario);
}

bool Loja::registrarConta(const std::string& usuario, const std::string& senha) {
    if (usuario.empty() || senha.empty()) return false;
    if (storage_.usuarioExiste(usuario))  return false;   // usuario e unico no banco

    // abre uma organizacao para essa conta (nome inicial = o proprio usuario)
    const int oid = storage_.criarOrg(usuario, usuario);
    if (oid < 0) return false;

    orgId_   = oid;
    orgNome_ = usuario;
    dono_    = usuario;
    produtos_.clear();
    usuarios_.clear();
    historico_.clear();
    caixa_.clear();

    Usuario admin;
    admin.usuario = usuario;
    admin.senha   = senha;
    admin.cargo   = Cargo::ADMIN;   // quem cria a conta e o admin dono
    usuarios_.push_back(admin);
    usuarioLogado_ = 0;

    salvar();
    return true;
}

bool Loja::entrar(const std::string& usuario, const std::string& senha) {
    const int oid = storage_.orgDoUsuario(usuario, senha);   // resolve a org pelo usuario
    if (oid < 0) return false;

    DadosLoja d;
    storage_.carregar(oid, d);

    int idx = -1;
    for (std::size_t i = 0; i < d.usuarios.size(); ++i)
        if (d.usuarios[i].usuario == usuario) { idx = static_cast<int>(i); break; }
    if (idx < 0) return false;   // nao deveria ocorrer (acabou de autenticar)

    orgId_     = oid;
    orgNome_   = storage_.orgNome(oid);
    dono_      = storage_.orgDono(oid);
    produtos_  = std::move(d.produtos);
    usuarios_  = std::move(d.usuarios);
    historico_ = std::move(d.vendas);
    caixa_     = std::move(d.caixa);
    usuarioLogado_ = idx;
    return true;
}

bool Loja::renomearOrg(const std::string& nome) {
    if (orgId_ < 0 || nome.empty()) return false;
    if (usuarioLogado_ < 0 || !usuarios_[static_cast<std::size_t>(usuarioLogado_)].isAdmin())
        return false;   // so o admin renomeia
    storage_.renomearOrg(orgId_, nome);
    orgNome_ = nome;
    return true;
}

void Loja::logout() {
    usuarioLogado_ = -1;
    orgId_ = -1;
    orgNome_.clear();
    dono_.clear();
    produtos_.clear();
    usuarios_.clear();
    historico_.clear();
    caixa_.clear();
}

const Usuario& Loja::usuarioAtual() const {
    return usuarios_[static_cast<std::size_t>(usuarioLogado_)];
}

// ----------------------- Produtos -----------------------

int Loja::proximoIdProduto() const {
    int maior = 0;
    for (const auto& p : produtos_)
        if (p.id > maior) maior = p.id;
    return maior + 1;
}

Produto* Loja::produtoPorId(int id) {
    for (auto& p : produtos_)
        if (p.id == id) return &p;
    return nullptr;
}

void Loja::adicionarProduto(const Produto& p) {
    produtos_.push_back(p);
    salvar();
}

bool Loja::removerProduto(int id) {
    auto it = std::find_if(produtos_.begin(), produtos_.end(),
                           [id](const Produto& p) { return p.id == id; });
    if (it == produtos_.end()) return false;
    produtos_.erase(it);
    salvar();
    return true;
}

bool Loja::atualizarEstoque(int id, int novoEstoque) {
    Produto* p = produtoPorId(id);
    if (!p) return false;
    p->estoque = (novoEstoque < 0) ? 0 : novoEstoque;
    salvar();
    return true;
}

std::vector<const Produto*> Loja::buscarProdutos(const std::string& termo) const {
    std::vector<const Produto*> achados;
    for (const auto& p : produtos_)
        if (p.nome.find(termo) != std::string::npos)
            achados.push_back(&p);
    return achados;
}

// ----------------------- Usuarios -----------------------

bool Loja::adicionarUsuario(const Usuario& u) {
    if (u.usuario.empty())               return false;
    if (storage_.usuarioExiste(u.usuario)) return false;   // usuario e unico no banco inteiro
    Usuario novo = u;
    novo.cargo = Cargo::FUNC;   // contas novas sao SEMPRE funcionario; admin so por elevacao
    usuarios_.push_back(novo);
    salvar();
    return true;
}

bool Loja::definirCargo(const std::string& usuario, Cargo novo) {
    // ninguem altera o proprio cargo (evita auto-elevacao e travar a sessao)
    if (usuarioLogado_ >= 0 &&
        usuarios_[static_cast<std::size_t>(usuarioLogado_)].usuario == usuario)
        return false;
    for (auto& u : usuarios_) {
        if (u.usuario != usuario) continue;
        // o dono da organizacao nunca pode ser rebaixado: e a garantia de acesso
        if (u.usuario == dono_ && novo != Cargo::ADMIN) return false;
        if (u.cargo == novo) return true;   // nada a fazer
        u.cargo = novo;
        salvar();
        return true;
    }
    return false;   // usuario inexistente nesta org
}

void Loja::definirMeta(double valor) {
    if (usuarioLogado_ < 0) return;
    usuarios_[static_cast<std::size_t>(usuarioLogado_)].meta = valor;
    salvar();
}

bool Loja::mudarSenha(const std::string& atual, const std::string& nova) {
    if (usuarioLogado_ < 0 || nova.empty()) return false;
    Usuario& u = usuarios_[static_cast<std::size_t>(usuarioLogado_)];
    if (!u.senhaConfere(atual)) return false;
    u.senha = nova;
    salvar();
    return true;
}

// ----------------------- Vendas / caixa -----------------------

std::optional<double> Loja::registrarVenda(const std::vector<ItemVenda>& itens) {
    if (usuarioLogado_ < 0 || itens.empty()) return std::nullopt;

    // agrega quantidades por produto (o carrinho pode repetir o mesmo id)
    std::vector<std::pair<int, int>> agregado;   // (produtoId, qtdTotal)
    for (const auto& it : itens) {
        if (it.quantidade <= 0) return std::nullopt;
        bool achou = false;
        for (auto& a : agregado)
            if (a.first == it.produtoId) { a.second += it.quantidade; achou = true; break; }
        if (!achou) agregado.push_back({ it.produtoId, it.quantidade });
    }

    // valida existencia e estoque suficiente ANTES de alterar qualquer coisa
    for (const auto& a : agregado) {
        Produto* p = produtoPorId(a.first);
        if (!p || a.second > p->estoque) return std::nullopt;
    }

    // tudo valido: aplica a baixa de estoque e gera as vendas
    double total = 0.0;
    const std::string func = usuarioAtual().usuario;
    const std::string data = util::dataAtual();
    for (const auto& it : itens) {
        Produto* p   = produtoPorId(it.produtoId);   // garantido existir
        p->estoque  -= it.quantidade;
        double subtotal = it.quantidade * p->preco;
        total += subtotal;

        Venda v;
        v.id          = static_cast<int>(historico_.size()) + 1;
        v.produtoId   = it.produtoId;
        v.funcionario = func;
        v.quantidade  = it.quantidade;
        v.total       = subtotal;
        v.data        = data;
        historico_.push_back(v);
    }

    caixa_.push_back({ "VENDA", total });
    usuarios_[static_cast<std::size_t>(usuarioLogado_)].totalVendido += total;

    salvar();   // persiste estoque + vendas + caixa + total numa unica transacao
    return total;
}

double Loja::saldoCaixa() const {
    double total = 0.0;
    for (const auto& l : caixa_) total += l.valor;
    return total;
}

double Loja::fecharCaixa() {
    double total = saldoCaixa();
    storage_.registrarFechamento(orgId_, util::dataAtual(), total);
    caixa_.clear();   // inicia um novo periodo
    salvar();
    return total;
}

// ----------------------- Relatorios -----------------------

double Loja::faturamentoTotal() const {
    double total = 0.0;
    for (const auto& v : historico_) total += v.total;
    return total;
}

int Loja::totalVendidoDoProduto(int produtoId) const {
    int qtd = 0;
    for (const auto& v : historico_)
        if (v.produtoId == produtoId) qtd += v.quantidade;
    return qtd;
}

double Loja::lucroEstimado() const {
    double lucro = 0.0;
    for (const auto& v : historico_) {
        const Produto* prod = nullptr;
        for (const auto& p : produtos_)
            if (p.id == v.produtoId) { prod = &p; break; }
        if (!prod) continue;   // produto removido: nao da para estimar o custo
        lucro += v.total - prod->custo * v.quantidade;
    }
    return lucro;
}

// ----------------------- Persistencia (SQLite via Storage) -----------------------

void Loja::salvar() {
    if (orgId_ < 0) return;   // sem org ativa nao ha o que salvar
    DadosLoja d;
    d.produtos = produtos_;
    d.usuarios = usuarios_;
    d.vendas   = historico_;
    d.caixa    = caixa_;
    storage_.salvar(orgId_, d);
}
