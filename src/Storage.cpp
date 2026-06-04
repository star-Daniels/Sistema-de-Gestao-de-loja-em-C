#include "Storage.hpp"

#include "sqlite3.h"

#include <stdexcept>
#include <string>

namespace {

// Schema MULTI-ORGANIZACAO. "orgs" tem id estavel + nome editavel + dono.
// usuarios.usuario e PRIMARY KEY GLOBAL (login so com usuario+senha) e aponta
// para o org_id. As demais tabelas usam (org_id, id) para isolar por org.
const char* SCHEMA = R"sql(
CREATE TABLE IF NOT EXISTS orgs (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL,
    dono TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS usuarios (
    usuario       TEXT PRIMARY KEY,
    senha         TEXT NOT NULL,
    cargo         TEXT NOT NULL,
    org_id        INTEGER NOT NULL,
    total_vendido REAL NOT NULL DEFAULT 0,
    meta          REAL NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS produtos (
    org_id   INTEGER NOT NULL,
    id       INTEGER NOT NULL,
    nome     TEXT    NOT NULL,
    preco    REAL    NOT NULL,
    custo    REAL    NOT NULL,
    estoque  INTEGER NOT NULL,
    validade TEXT,
    PRIMARY KEY (org_id, id)
);
CREATE TABLE IF NOT EXISTS vendas (
    org_id      INTEGER NOT NULL,
    id          INTEGER NOT NULL,
    produto_id  INTEGER NOT NULL,
    funcionario TEXT    NOT NULL,
    quantidade  INTEGER NOT NULL,
    total       REAL    NOT NULL,
    data        TEXT    NOT NULL,
    PRIMARY KEY (org_id, id)
);
CREATE TABLE IF NOT EXISTS caixa (
    id     INTEGER PRIMARY KEY AUTOINCREMENT,
    org_id INTEGER NOT NULL,
    tipo   TEXT NOT NULL,
    valor  REAL NOT NULL
);
CREATE TABLE IF NOT EXISTS fechamentos (
    id     INTEGER PRIMARY KEY AUTOINCREMENT,
    org_id INTEGER NOT NULL,
    data   TEXT NOT NULL,
    total  REAL NOT NULL
);
)sql";

std::string textoColuna(sqlite3_stmt* st, int col) {
    const unsigned char* t = sqlite3_column_text(st, col);
    return t ? reinterpret_cast<const char*>(t) : std::string();
}

} // namespace

Storage::Storage(const std::string& caminho) : caminho_(caminho) {}

Storage::~Storage() {
    if (db_) sqlite3_close(db_);
}

void Storage::exec(const char* sql) {
    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "erro SQLite";
        sqlite3_free(err);
        throw std::runtime_error(msg);
    }
}

bool Storage::abrir() {
    if (sqlite3_open(caminho_.c_str(), &db_) != SQLITE_OK)
        return false;
    exec(SCHEMA);
    return true;
}

// ----------------------- Contas / organizacoes -----------------------

bool Storage::usuarioExiste(const std::string& usuario) {
    sqlite3_stmt* st = nullptr;
    bool existe = false;
    if (sqlite3_prepare_v2(db_, "SELECT 1 FROM usuarios WHERE usuario=? LIMIT 1;",
                           -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, usuario.c_str(), -1, SQLITE_TRANSIENT);
        existe = (sqlite3_step(st) == SQLITE_ROW);
    }
    sqlite3_finalize(st);
    return existe;
}

int Storage::orgDoUsuario(const std::string& usuario, const std::string& senha) {
    sqlite3_stmt* st = nullptr;
    int orgId = -1;
    if (sqlite3_prepare_v2(db_, "SELECT org_id, senha FROM usuarios WHERE usuario=?;",
                           -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, usuario.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(st) == SQLITE_ROW) {
            const int         o = sqlite3_column_int(st, 0);
            const std::string s = textoColuna(st, 1);
            if (s == senha) orgId = o;   // confere a senha
        }
    }
    sqlite3_finalize(st);
    return orgId;
}

int Storage::criarOrg(const std::string& nome, const std::string& dono) {
    sqlite3_stmt* st = nullptr;
    int id = -1;
    if (sqlite3_prepare_v2(db_, "INSERT INTO orgs (nome, dono) VALUES (?,?);",
                           -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, nome.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(st, 2, dono.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(st) == SQLITE_DONE)
            id = static_cast<int>(sqlite3_last_insert_rowid(db_));
    }
    sqlite3_finalize(st);
    return id;
}

std::string Storage::orgNome(int orgId) {
    sqlite3_stmt* st = nullptr;
    std::string nome;
    if (sqlite3_prepare_v2(db_, "SELECT nome FROM orgs WHERE id=?;", -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, orgId);
        if (sqlite3_step(st) == SQLITE_ROW) nome = textoColuna(st, 0);
    }
    sqlite3_finalize(st);
    return nome;
}

std::string Storage::orgDono(int orgId) {
    sqlite3_stmt* st = nullptr;
    std::string dono;
    if (sqlite3_prepare_v2(db_, "SELECT dono FROM orgs WHERE id=?;", -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, orgId);
        if (sqlite3_step(st) == SQLITE_ROW) dono = textoColuna(st, 0);
    }
    sqlite3_finalize(st);
    return dono;
}

void Storage::renomearOrg(int orgId, const std::string& nome) {
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, "UPDATE orgs SET nome=? WHERE id=?;", -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, nome.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int (st, 2, orgId);
        sqlite3_step(st);
    }
    sqlite3_finalize(st);
}

// ----------------------- Dados de uma organizacao -----------------------

void Storage::carregar(int orgId, DadosLoja& d) {
    d.produtos.clear();
    d.usuarios.clear();
    d.vendas.clear();
    d.caixa.clear();

    sqlite3_stmt* st = nullptr;

    if (sqlite3_prepare_v2(db_,
            "SELECT id, nome, preco, custo, estoque, validade FROM produtos WHERE org_id=? ORDER BY id;",
            -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, orgId);
        while (sqlite3_step(st) == SQLITE_ROW) {
            Produto p;
            p.id       = sqlite3_column_int(st, 0);
            p.nome     = textoColuna(st, 1);
            p.preco    = sqlite3_column_double(st, 2);
            p.custo    = sqlite3_column_double(st, 3);
            p.estoque  = sqlite3_column_int(st, 4);
            p.validade = textoColuna(st, 5);
            d.produtos.push_back(p);
        }
    }
    sqlite3_finalize(st);
    st = nullptr;

    if (sqlite3_prepare_v2(db_,
            "SELECT usuario, senha, cargo, total_vendido, meta FROM usuarios WHERE org_id=?;",
            -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, orgId);
        while (sqlite3_step(st) == SQLITE_ROW) {
            Usuario u;
            u.usuario      = textoColuna(st, 0);
            u.senha        = textoColuna(st, 1);
            u.cargo        = cargoDeTexto(textoColuna(st, 2));
            u.totalVendido = sqlite3_column_double(st, 3);
            u.meta         = sqlite3_column_double(st, 4);
            d.usuarios.push_back(u);
        }
    }
    sqlite3_finalize(st);
    st = nullptr;

    if (sqlite3_prepare_v2(db_,
            "SELECT id, produto_id, funcionario, quantidade, total, data FROM vendas WHERE org_id=? ORDER BY id;",
            -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, orgId);
        while (sqlite3_step(st) == SQLITE_ROW) {
            Venda v;
            v.id          = sqlite3_column_int(st, 0);
            v.produtoId   = sqlite3_column_int(st, 1);
            v.funcionario = textoColuna(st, 2);
            v.quantidade  = sqlite3_column_int(st, 3);
            v.total       = sqlite3_column_double(st, 4);
            v.data        = textoColuna(st, 5);
            d.vendas.push_back(v);
        }
    }
    sqlite3_finalize(st);
    st = nullptr;

    if (sqlite3_prepare_v2(db_,
            "SELECT tipo, valor FROM caixa WHERE org_id=? ORDER BY id;",
            -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, orgId);
        while (sqlite3_step(st) == SQLITE_ROW) {
            LancamentoCaixa l;
            l.tipo  = textoColuna(st, 0);
            l.valor = sqlite3_column_double(st, 1);
            d.caixa.push_back(l);
        }
    }
    sqlite3_finalize(st);
    st = nullptr;
}

void Storage::salvar(int orgId, const DadosLoja& d) {
    exec("BEGIN IMMEDIATE;");
    try {
        // apaga apenas os dados DESTA org (nao toca nas outras)
        auto del = [&](const char* sql) {
            sqlite3_stmt* st = nullptr;
            sqlite3_prepare_v2(db_, sql, -1, &st, nullptr);
            sqlite3_bind_int(st, 1, orgId);
            sqlite3_step(st);
            sqlite3_finalize(st);
        };
        del("DELETE FROM produtos WHERE org_id=?;");
        del("DELETE FROM usuarios WHERE org_id=?;");
        del("DELETE FROM vendas   WHERE org_id=?;");
        del("DELETE FROM caixa    WHERE org_id=?;");

        sqlite3_stmt* st = nullptr;

        sqlite3_prepare_v2(db_,
            "INSERT INTO produtos (org_id,id,nome,preco,custo,estoque,validade) VALUES (?,?,?,?,?,?,?);",
            -1, &st, nullptr);
        for (const auto& p : d.produtos) {
            sqlite3_bind_int   (st, 1, orgId);
            sqlite3_bind_int   (st, 2, p.id);
            sqlite3_bind_text  (st, 3, p.nome.c_str(),     -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(st, 4, p.preco);
            sqlite3_bind_double(st, 5, p.custo);
            sqlite3_bind_int   (st, 6, p.estoque);
            sqlite3_bind_text  (st, 7, p.validade.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(st);
            sqlite3_reset(st);
        }
        sqlite3_finalize(st);
        st = nullptr;

        sqlite3_prepare_v2(db_,
            "INSERT INTO usuarios (usuario,senha,cargo,org_id,total_vendido,meta) VALUES (?,?,?,?,?,?);",
            -1, &st, nullptr);
        for (const auto& u : d.usuarios) {
            std::string cargo = textoDeCargo(u.cargo);
            sqlite3_bind_text  (st, 1, u.usuario.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text  (st, 2, u.senha.c_str(),   -1, SQLITE_TRANSIENT);
            sqlite3_bind_text  (st, 3, cargo.c_str(),     -1, SQLITE_TRANSIENT);
            sqlite3_bind_int   (st, 4, orgId);
            sqlite3_bind_double(st, 5, u.totalVendido);
            sqlite3_bind_double(st, 6, u.meta);
            sqlite3_step(st);
            sqlite3_reset(st);
        }
        sqlite3_finalize(st);
        st = nullptr;

        sqlite3_prepare_v2(db_,
            "INSERT INTO vendas (org_id,id,produto_id,funcionario,quantidade,total,data) VALUES (?,?,?,?,?,?,?);",
            -1, &st, nullptr);
        for (const auto& v : d.vendas) {
            sqlite3_bind_int   (st, 1, orgId);
            sqlite3_bind_int   (st, 2, v.id);
            sqlite3_bind_int   (st, 3, v.produtoId);
            sqlite3_bind_text  (st, 4, v.funcionario.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int   (st, 5, v.quantidade);
            sqlite3_bind_double(st, 6, v.total);
            sqlite3_bind_text  (st, 7, v.data.c_str(),        -1, SQLITE_TRANSIENT);
            sqlite3_step(st);
            sqlite3_reset(st);
        }
        sqlite3_finalize(st);
        st = nullptr;

        sqlite3_prepare_v2(db_,
            "INSERT INTO caixa (org_id,tipo,valor) VALUES (?,?,?);",
            -1, &st, nullptr);
        for (const auto& l : d.caixa) {
            sqlite3_bind_int   (st, 1, orgId);
            sqlite3_bind_text  (st, 2, l.tipo.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(st, 3, l.valor);
            sqlite3_step(st);
            sqlite3_reset(st);
        }
        sqlite3_finalize(st);
        st = nullptr;

        exec("COMMIT;");
    } catch (...) {
        exec("ROLLBACK;");
        throw;
    }
}

void Storage::registrarFechamento(int orgId, const std::string& data, double total) {
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_,
            "INSERT INTO fechamentos (org_id,data,total) VALUES (?,?,?);",
            -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int   (st, 1, orgId);
        sqlite3_bind_text  (st, 2, data.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(st, 3, total);
        sqlite3_step(st);
    }
    sqlite3_finalize(st);
}
