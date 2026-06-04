#include "MainWindow.hpp"
#include "Loja.hpp"

#include <QtWidgets>
#include <QFont>
#include <QColor>
#include <QBrush>

#include <functional>
#include <memory>
#include <vector>

// ----------------------------------------------------------------------------
//  Pequenos auxiliares de UI (mantem as abas curtas e o visual consistente).
//  A aparencia fina (cores, bordas, cantos) vem do stylesheet global aplicado
//  em main.cpp; aqui so marcamos os widgets com objectName/estrutura.
// ----------------------------------------------------------------------------

namespace {

QLabel* tituloPagina(const QString& texto) {
    auto* l = new QLabel(texto);
    l->setObjectName("pageTitle");
    return l;
}

QLabel* legenda(const QString& texto) {
    auto* l = new QLabel(texto);
    l->setObjectName("muted");
    l->setWordWrap(true);
    return l;
}

QLabel* tituloSecao(const QString& texto) {
    auto* l = new QLabel(texto);
    l->setObjectName("sectionTitle");
    return l;
}

QFrame* cartao() {
    auto* c = new QFrame;
    c->setObjectName("card");
    return c;
}

// Iniciais (ate 2 letras) de um nome de usuario, para o avatar.
QString iniciais(const std::string& nome) {
    const QString s = QString::fromStdString(nome).trimmed();
    if (s.isEmpty()) return "?";
    const QStringList partes = s.split(' ', Qt::SkipEmptyParts);
    QString r = (partes.size() >= 2) ? (partes[0].left(1) + partes[1].left(1))
                                     : s.left(2);
    return r.toUpper();
}

// Aparencia padrao das tabelas: linhas alternadas, sem grade dura, selecao por
// linha unica e sem o cabecalho vertical (numeros de linha).
void prepararTabela(QTableWidget* t) {
    t->horizontalHeader()->setStretchLastSection(true);
    t->verticalHeader()->setVisible(false);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setAlternatingRowColors(true);
    t->setShowGrid(false);
}

// Esqueleto de uma aba: um widget com titulo, legenda e um layout pronto para
// receber o conteudo. Devolve o widget; o layout sai por *outLayout.
QWidget* paginaBase(const QString& titulo, const QString& subtitulo, QVBoxLayout** outLayout) {
    auto* page = new QWidget;
    auto* lay  = new QVBoxLayout(page);
    lay->setContentsMargins(22, 20, 22, 20);
    lay->setSpacing(12);
    lay->addWidget(tituloPagina(titulo));
    if (!subtitulo.isEmpty()) lay->addWidget(legenda(subtitulo));
    lay->addSpacing(4);
    *outLayout = lay;
    return page;
}

} // namespace

// ----------------------------------------------------------------------------
//  Construcao da janela e montagem das abas conforme o cargo
// ----------------------------------------------------------------------------

MainWindow::MainWindow(Loja& loja, QWidget* parent)
    : QMainWindow(parent), loja_(loja) {
    const Usuario& u   = loja_.usuarioAtual();
    const bool     adm = u.isAdmin();

    setWindowTitle(QString("Sistema de Loja  -  %1  /  %2 (%3)")
                       .arg(QString::fromStdString(loja_.org()),
                            QString::fromStdString(u.usuario), adm ? "ADMIN" : "FUNC"));
    resize(1060, 700);

    // Shell moderno: rail de navegacao a esquerda + area de conteudo (stack).
    auto* central = new QWidget;
    auto* root    = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QWidget* rail = construirSidebar();   // tambem define navLayout_ e navGroup_
    stack_ = new QStackedWidget;
    stack_->setObjectName("content");

    root->addWidget(rail);
    root->addWidget(stack_, 1);
    setCentralWidget(central);

    // Secoes na ordem do rail. Cada criar* empilha exatamente 1 callback em
    // refrescar_, mantendo o indice alinhado ao da secao (so as visiveis entram).
    if (adm) adicionarSecao("Produtos",      criarAbaProdutos());
             adicionarSecao("Vendas",        criarAbaVendas());
             adicionarSecao("Caixa",         criarAbaCaixa());
    if (adm) adicionarSecao("Usuarios",      criarAbaUsuarios());
    if (adm) adicionarSecao("Relatorios",    criarAbaRelatorios());
             adicionarSecao("Desempenho",    criarAbaDesempenho());
             adicionarSecao("Perfil",        criarAbaPerfil());
             adicionarSecao("Configuracoes", criarAbaConfiguracoes());

    statusBar()->showMessage(QString("Organizacao \"%1\"  -  conectado como %2  (%3)")
        .arg(QString::fromStdString(loja_.org()),
             QString::fromStdString(u.usuario), adm ? "Administrador" : "Funcionario"));

    stack_->setCurrentIndex(0);
    if (!refrescar_.empty() && refrescar_[0]) refrescar_[0]();   // popula a primeira secao
}

// ----------------------------------------------------------------------------
//  Rail de navegacao (sidebar) + troca de secoes
// ----------------------------------------------------------------------------

QWidget* MainWindow::construirSidebar() {
    const Usuario& u = loja_.usuarioAtual();

    auto* bar = new QFrame;
    bar->setObjectName("sidebar");
    bar->setFixedWidth(238);

    auto* v = new QVBoxLayout(bar);
    v->setContentsMargins(16, 18, 16, 16);
    v->setSpacing(6);

    // marca + organizacao
    auto* mark = new QLabel("SL");
    mark->setObjectName("brandSmall");
    mark->setFixedSize(36, 36);
    mark->setAlignment(Qt::AlignCenter);
    auto* bname = new QLabel("Gestao de Loja"); bname->setObjectName("brandName");
    auto* borg  = new QLabel(QString::fromStdString(loja_.org())); borg->setObjectName("brandOrg");
    brandOrg_ = borg;   // guardado para atualizar ao renomear a org
    auto* bcol  = new QVBoxLayout; bcol->setSpacing(0);
    bcol->addWidget(bname); bcol->addWidget(borg);
    auto* brand = new QHBoxLayout;
    brand->addWidget(mark); brand->addSpacing(10); brand->addLayout(bcol); brand->addStretch();
    v->addLayout(brand);
    v->addSpacing(16);

    // botoes de navegacao (preenchidos por adicionarSecao)
    navGroup_  = new QButtonGroup(this);
    navGroup_->setExclusive(true);
    navLayout_ = new QVBoxLayout;
    navLayout_->setSpacing(4);
    v->addLayout(navLayout_);

    v->addStretch();

    // chip do usuario logado
    auto* chip = new QFrame; chip->setObjectName("userChip");
    auto* ch   = new QHBoxLayout(chip); ch->setContentsMargins(10, 8, 10, 8);
    auto* av   = new QLabel(iniciais(u.usuario)); av->setObjectName("avatarSm");
    av->setFixedSize(34, 34); av->setAlignment(Qt::AlignCenter);
    auto* un   = new QLabel(QString::fromStdString(u.usuario)); un->setObjectName("chipName");
    auto* ur   = new QLabel(loja_.ehDono(u.usuario) ? "Dono"
                            : (u.isAdmin() ? "Administrador" : "Funcionario"));
    ur->setObjectName("chipRole");
    auto* ucol = new QVBoxLayout; ucol->setSpacing(0);
    ucol->addWidget(un); ucol->addWidget(ur);
    ch->addWidget(av); ch->addSpacing(10); ch->addLayout(ucol); ch->addStretch();
    v->addWidget(chip);

    v->addSpacing(6);
    auto* sair = new QPushButton("Sair");
    sair->setObjectName("logoutBtn");
    sair->setCursor(Qt::PointingHandCursor);
    connect(sair, &QPushButton::clicked, this, &QMainWindow::close);
    v->addWidget(sair);

    return bar;
}

void MainWindow::adicionarSecao(const QString& titulo, QWidget* pagina) {
    const int idx = stack_->addWidget(pagina);

    auto* btn = new QPushButton(titulo);
    btn->setObjectName("navBtn");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    navGroup_->addButton(btn, idx);
    navLayout_->addWidget(btn);

    connect(btn, &QPushButton::clicked, this, [this, idx]() {
        stack_->setCurrentIndex(idx);
        if (idx < static_cast<int>(refrescar_.size()) && refrescar_[idx]) refrescar_[idx]();
    });

    if (idx == 0) btn->setChecked(true);
}

// ----------------------------------------------------------------------------
//  Produtos (somente ADMIN): listar, buscar, cadastrar, remover, ajustar estoque
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaProdutos() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Produtos",
                         "Cadastre, busque e controle estoque e validade dos produtos.", &layout);

    auto* busca = new QLineEdit(w);
    busca->setPlaceholderText("Buscar por nome...");
    busca->setClearButtonEnabled(true);

    auto* tabela = new QTableWidget(0, 6, w);
    tabela->setHorizontalHeaderLabels({"ID", "Nome", "Preco", "Custo", "Estoque", "Validade"});
    prepararTabela(tabela);

    auto preencher = [this, tabela, busca]() {
        const QString termo = busca->text();
        std::vector<const Produto*> lista;
        if (termo.isEmpty())
            for (const auto& p : loja_.produtos()) lista.push_back(&p);
        else
            lista = loja_.buscarProdutos(termo.toStdString());

        tabela->setRowCount(static_cast<int>(lista.size()));
        for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
            const Produto& p = *lista[static_cast<std::size_t>(i)];
            auto set = [&](int c, const QString& s) {
                auto* item = new QTableWidgetItem(s);
                if (p.estoqueBaixo()) item->setForeground(QBrush(QColor(Qt::red)));
                tabela->setItem(i, c, item);
            };
            set(0, QString::number(p.id));
            set(1, QString::fromStdString(p.nome));
            set(2, QString::number(p.preco, 'f', 2));
            set(3, QString::number(p.custo, 'f', 2));
            set(4, QString::number(p.estoque) + (p.estoqueBaixo() ? "  (baixo)" : ""));
            set(5, QString::fromStdString(p.validade) + (p.vencendo() ? "  (vence!)" : ""));
        }
    };
    connect(busca, &QLineEdit::textChanged, w, [preencher](const QString&) { preencher(); });

    auto idSelecionado = [tabela]() -> int {
        int r = tabela->currentRow();
        if (r < 0 || !tabela->item(r, 0)) return -1;
        return tabela->item(r, 0)->text().toInt();
    };

    auto* novo    = new QPushButton("Novo produto", w);      novo->setObjectName("primaryBtn");
    auto* estoque = new QPushButton("Atualizar estoque", w); estoque->setObjectName("secondaryBtn");
    auto* remover = new QPushButton("Remover", w);           remover->setObjectName("dangerBtn");

    connect(novo, &QPushButton::clicked, w, [this, preencher, w]() {
        QDialog dlg(w);
        dlg.setWindowTitle("Novo produto");
        auto* nome  = new QLineEdit(&dlg);
        auto* preco = new QDoubleSpinBox(&dlg); preco->setMaximum(1e9); preco->setDecimals(2);
        auto* custo = new QDoubleSpinBox(&dlg); custo->setMaximum(1e9); custo->setDecimals(2);
        auto* est   = new QSpinBox(&dlg);       est->setMaximum(1000000);
        auto* val   = new QLineEdit(&dlg);      val->setPlaceholderText("dd/mm/aaaa");
        auto* form  = new QFormLayout;
        form->addRow("Nome:", nome);
        form->addRow("Preco de venda:", preco);
        form->addRow("Custo:", custo);
        form->addRow("Estoque:", est);
        form->addRow("Validade:", val);
        auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        auto* lay = new QVBoxLayout(&dlg);
        lay->addLayout(form);
        lay->addWidget(bb);
        if (dlg.exec() != QDialog::Accepted) return;
        if (nome->text().trimmed().isEmpty()) {
            QMessageBox::warning(w, "Novo produto", "O nome nao pode ser vazio.");
            return;
        }
        Produto p;
        p.id       = loja_.proximoIdProduto();
        p.nome     = nome->text().toStdString();
        p.preco    = preco->value();
        p.custo    = custo->value();
        p.estoque  = est->value();
        p.validade = val->text().toStdString();
        loja_.adicionarProduto(p);
        preencher();
    });

    connect(estoque, &QPushButton::clicked, w, [this, preencher, idSelecionado, w]() {
        int id = idSelecionado();
        if (id < 0) { QMessageBox::information(w, "Estoque", "Selecione um produto na tabela."); return; }
        Produto* p = loja_.produtoPorId(id);
        if (!p) return;
        bool ok = false;
        int novoEst = QInputDialog::getInt(w, "Atualizar estoque",
            QString("Novo estoque para \"%1\":").arg(QString::fromStdString(p->nome)),
            p->estoque, 0, 1000000, 1, &ok);
        if (ok) { loja_.atualizarEstoque(id, novoEst); preencher(); }
    });

    connect(remover, &QPushButton::clicked, w, [this, preencher, idSelecionado, w]() {
        int id = idSelecionado();
        if (id < 0) { QMessageBox::information(w, "Remover", "Selecione um produto na tabela."); return; }
        if (QMessageBox::question(w, "Remover", QString("Remover o produto %1?").arg(id))
                == QMessageBox::Yes) {
            loja_.removerProduto(id);
            preencher();
        }
    });

    auto* botoes = new QHBoxLayout;
    botoes->addWidget(novo);
    botoes->addWidget(estoque);
    botoes->addWidget(remover);
    botoes->addStretch();

    layout->addWidget(busca);
    layout->addWidget(tabela, 1);
    layout->addLayout(botoes);

    refrescar_.push_back(preencher);
    return w;
}

// ----------------------------------------------------------------------------
//  Vendas (todos): monta um carrinho e finaliza a venda de forma atomica
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaVendas() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Vendas",
                         "Monte o carrinho e finalize a venda; o estoque baixa automaticamente.", &layout);

    auto* combo  = new QComboBox(w);
    auto* qtd    = new QSpinBox(w); qtd->setRange(1, 1000000); qtd->setValue(1);
    auto* addBtn = new QPushButton("Adicionar ao carrinho", w); addBtn->setObjectName("secondaryBtn");

    auto* linha = new QHBoxLayout;
    linha->addWidget(new QLabel("Produto:", w));
    linha->addWidget(combo, 1);
    linha->addWidget(new QLabel("Qtd:", w));
    linha->addWidget(qtd);
    linha->addWidget(addBtn);

    auto* carrinho = new QTableWidget(0, 4, w);
    carrinho->setHorizontalHeaderLabels({"Produto", "Qtd", "Preco", "Subtotal"});
    prepararTabela(carrinho);

    auto* totalLbl = new QLabel("Total: R$ 0.00", w);
    totalLbl->setObjectName("kpi");

    auto* removerItem = new QPushButton("Remover item", w); removerItem->setObjectName("secondaryBtn");
    auto* limpar      = new QPushButton("Limpar", w);        limpar->setObjectName("secondaryBtn");
    auto* finalizar   = new QPushButton("Finalizar venda", w); finalizar->setObjectName("primaryBtn");

    auto itens = std::make_shared<std::vector<ItemVenda>>();

    auto recarregarCombo = [this, combo]() {
        combo->clear();
        for (const auto& p : loja_.produtos())
            if (p.estoque > 0)
                combo->addItem(QString("%1 - %2  (estoque %3, R$ %4)")
                                   .arg(p.id).arg(QString::fromStdString(p.nome))
                                   .arg(p.estoque).arg(QString::number(p.preco, 'f', 2)),
                               p.id);
    };

    auto atualizarCarrinho = [this, carrinho, totalLbl, itens]() {
        carrinho->setRowCount(static_cast<int>(itens->size()));
        double total = 0.0;
        for (int i = 0; i < static_cast<int>(itens->size()); ++i) {
            const ItemVenda& it = (*itens)[static_cast<std::size_t>(i)];
            const Produto*   p  = loja_.produtoPorId(it.produtoId);
            QString nome  = p ? QString::fromStdString(p->nome) : QString::number(it.produtoId);
            double  preco = p ? p->preco : 0.0;
            double  sub   = preco * it.quantidade;
            total += sub;
            carrinho->setItem(i, 0, new QTableWidgetItem(nome));
            carrinho->setItem(i, 1, new QTableWidgetItem(QString::number(it.quantidade)));
            carrinho->setItem(i, 2, new QTableWidgetItem(QString::number(preco, 'f', 2)));
            carrinho->setItem(i, 3, new QTableWidgetItem(QString::number(sub, 'f', 2)));
        }
        totalLbl->setText(QString("Total: R$ %1").arg(QString::number(total, 'f', 2)));
    };

    connect(addBtn, &QPushButton::clicked, w, [combo, qtd, itens, atualizarCarrinho]() {
        if (combo->currentIndex() < 0) return;
        itens->push_back({ combo->currentData().toInt(), qtd->value() });
        atualizarCarrinho();
    });

    connect(removerItem, &QPushButton::clicked, w, [carrinho, itens, atualizarCarrinho]() {
        int r = carrinho->currentRow();
        if (r >= 0 && r < static_cast<int>(itens->size())) {
            itens->erase(itens->begin() + r);
            atualizarCarrinho();
        }
    });

    connect(limpar, &QPushButton::clicked, w, [itens, atualizarCarrinho]() {
        itens->clear();
        atualizarCarrinho();
    });

    connect(finalizar, &QPushButton::clicked, w,
            [this, itens, atualizarCarrinho, recarregarCombo, w]() {
        if (itens->empty()) { QMessageBox::information(w, "Venda", "O carrinho esta vazio."); return; }
        auto total = loja_.registrarVenda(*itens);
        if (!total) {
            QMessageBox::warning(w, "Venda",
                "Nao foi possivel concluir: produto inexistente ou estoque insuficiente.");
            return;
        }
        QMessageBox::information(w, "Venda",
            QString("Venda concluida!\nTotal: R$ %1").arg(QString::number(*total, 'f', 2)));
        itens->clear();
        atualizarCarrinho();
        recarregarCombo();
    });

    auto* botoes = new QHBoxLayout;
    botoes->addWidget(removerItem);
    botoes->addWidget(limpar);
    botoes->addStretch();
    botoes->addWidget(totalLbl);
    botoes->addWidget(finalizar);

    layout->addLayout(linha);
    layout->addWidget(carrinho, 1);
    layout->addLayout(botoes);

    refrescar_.push_back([recarregarCombo, atualizarCarrinho]() {
        recarregarCombo();
        atualizarCarrinho();
    });
    return w;
}

// ----------------------------------------------------------------------------
//  Caixa (todos veem; somente ADMIN fecha)
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaCaixa() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Caixa",
                         "Lancamentos do periodo, saldo atual e fechamento.", &layout);

    auto* tabela = new QTableWidget(0, 2, w);
    tabela->setHorizontalHeaderLabels({"Tipo", "Valor"});
    prepararTabela(tabela);

    auto* saldoLbl = new QLabel(w);
    saldoLbl->setObjectName("kpi");

    auto* fechar = new QPushButton("Fechar caixa", w);
    fechar->setObjectName("primaryBtn");
    fechar->setVisible(loja_.usuarioAtual().isAdmin());

    auto preencher = [this, tabela, saldoLbl]() {
        const auto& ls = loja_.lancamentosCaixa();
        tabela->setRowCount(static_cast<int>(ls.size()));
        for (int i = 0; i < static_cast<int>(ls.size()); ++i) {
            const LancamentoCaixa& l = ls[static_cast<std::size_t>(i)];
            tabela->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(l.tipo)));
            tabela->setItem(i, 1, new QTableWidgetItem(QString::number(l.valor, 'f', 2)));
        }
        saldoLbl->setText(QString("Saldo atual: R$ %1")
                              .arg(QString::number(loja_.saldoCaixa(), 'f', 2)));
    };

    connect(fechar, &QPushButton::clicked, w, [this, preencher, w]() {
        if (loja_.lancamentosCaixa().empty()) {
            QMessageBox::information(w, "Caixa", "O caixa esta vazio."); return;
        }
        if (QMessageBox::question(w, "Fechar caixa", "Fechar o caixa e zerar o periodo?")
                == QMessageBox::Yes) {
            double t = loja_.fecharCaixa();
            QMessageBox::information(w, "Caixa",
                QString("Caixa fechado.\nTotal do periodo: R$ %1").arg(QString::number(t, 'f', 2)));
            preencher();
        }
    });

    auto* botoes = new QHBoxLayout;
    botoes->addWidget(saldoLbl);
    botoes->addStretch();
    botoes->addWidget(fechar);

    layout->addWidget(tabela, 1);
    layout->addLayout(botoes);

    refrescar_.push_back(preencher);
    return w;
}

// ----------------------------------------------------------------------------
//  Usuarios (somente ADMIN): listar, cadastrar (sempre FUNC) e elevar/rebaixar
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaUsuarios() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Usuarios",
                         "Cadastre funcionarios e eleve a administrador quando necessario. "
                         "O dono da organizacao nao pode ser rebaixado.", &layout);

    auto* tabela = new QTableWidget(0, 3, w);
    tabela->setHorizontalHeaderLabels({"Usuario", "Cargo", "Total vendido"});
    prepararTabela(tabela);

    auto preencher = [this, tabela]() {
        const auto& us = loja_.usuarios();
        tabela->setRowCount(static_cast<int>(us.size()));
        for (int i = 0; i < static_cast<int>(us.size()); ++i) {
            const Usuario& u = us[static_cast<std::size_t>(i)];
            QString cargo = u.isAdmin() ? "ADMIN" : "FUNC";
            if (loja_.ehDono(u.usuario)) cargo += "  (dono)";
            tabela->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(u.usuario)));
            tabela->setItem(i, 1, new QTableWidgetItem(cargo));
            tabela->setItem(i, 2, new QTableWidgetItem(QString::number(u.totalVendido, 'f', 2)));
        }
    };

    auto usuarioSelecionado = [tabela]() -> QString {
        int r = tabela->currentRow();
        if (r < 0 || !tabela->item(r, 0)) return QString();
        return tabela->item(r, 0)->text();
    };

    auto* novo     = new QPushButton("Novo usuario", w);             novo->setObjectName("primaryBtn");
    auto* promover = new QPushButton("Promover a admin", w);         promover->setObjectName("secondaryBtn");
    auto* rebaixar = new QPushButton("Rebaixar p/ funcionario", w);  rebaixar->setObjectName("secondaryBtn");

    connect(novo, &QPushButton::clicked, w, [this, preencher, w]() {
        QDialog dlg(w);
        dlg.setWindowTitle("Novo usuario");
        auto* nome  = new QLineEdit(&dlg);
        auto* senha = new QLineEdit(&dlg); senha->setEchoMode(QLineEdit::Password);
        auto* form  = new QFormLayout;
        form->addRow("Usuario:", nome);
        form->addRow("Senha:", senha);
        auto* nota = new QLabel("Novos usuarios entram como funcionario. "
                                "Use \"Promover a admin\" para elevar.", &dlg);
        nota->setObjectName("muted");
        nota->setWordWrap(true);
        auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        auto* lay = new QVBoxLayout(&dlg);
        lay->addLayout(form);
        lay->addWidget(nota);
        lay->addWidget(bb);
        if (dlg.exec() != QDialog::Accepted) return;
        if (nome->text().trimmed().isEmpty()) {
            QMessageBox::warning(w, "Novo usuario", "O nome nao pode ser vazio."); return;
        }
        Usuario u;
        u.usuario = nome->text().toStdString();
        u.senha   = senha->text().toStdString();
        u.cargo   = Cargo::FUNC;   // admins so por elevacao
        if (!loja_.adicionarUsuario(u)) {
            QMessageBox::warning(w, "Novo usuario", "Ja existe um usuario com esse nome.");
            return;
        }
        preencher();
    });

    // Elevacao: muda o cargo do usuario selecionado. Bloqueia mexer no proprio
    // cargo (evita auto-elevacao e travar a sessao) e a Loja protege a raiz.
    auto mudarCargo = [this, preencher, usuarioSelecionado, w](Cargo novo) {
        const QString sel = usuarioSelecionado();
        if (sel.isEmpty()) {
            QMessageBox::information(w, "Usuarios", "Selecione um usuario na tabela."); return;
        }
        const std::string nome = sel.toStdString();
        if (nome == loja_.usuarioAtual().usuario) {
            QMessageBox::warning(w, "Usuarios", "Voce nao pode alterar o proprio cargo."); return;
        }
        if (!loja_.definirCargo(nome, novo)) {
            QMessageBox::warning(w, "Usuarios",
                "Nao foi possivel alterar o cargo (o dono da organizacao nao pode ser rebaixado).");
            return;
        }
        preencher();
    };
    connect(promover, &QPushButton::clicked, w, [mudarCargo]() { mudarCargo(Cargo::ADMIN); });
    connect(rebaixar, &QPushButton::clicked, w, [mudarCargo]() { mudarCargo(Cargo::FUNC); });

    auto* botoes = new QHBoxLayout;
    botoes->addWidget(novo);
    botoes->addWidget(promover);
    botoes->addWidget(rebaixar);
    botoes->addStretch();

    layout->addWidget(tabela, 1);
    layout->addLayout(botoes);

    refrescar_.push_back(preencher);
    return w;
}

// ----------------------------------------------------------------------------
//  Relatorios (somente ADMIN): faturamento, lucro, mais vendidos, historico
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaRelatorios() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Relatorios",
                         "Visao geral do desempenho da loja.", &layout);

    auto* resumo = new QLabel;
    resumo->setObjectName("kpi");
    resumo->setTextFormat(Qt::RichText);

    auto* cardResumo = cartao();
    auto* cl = new QVBoxLayout(cardResumo);
    cl->setContentsMargins(0, 0, 0, 0);   // o respiro ja vem do padding do #card
    cl->addWidget(resumo);

    auto* maisVendidos = new QTableWidget(0, 2, w);
    maisVendidos->setHorizontalHeaderLabels({"Produto", "Qtd vendida"});
    prepararTabela(maisVendidos);

    auto* historico = new QTableWidget(0, 6, w);
    historico->setHorizontalHeaderLabels({"Venda", "Produto", "Func", "Qtd", "Total", "Data"});
    prepararTabela(historico);

    auto preencher = [this, resumo, maisVendidos, historico]() {
        resumo->setText(QString("<b>Faturamento total:</b> R$ %1 &nbsp;&nbsp;&nbsp; "
                                "<b>Lucro estimado:</b> R$ %2")
            .arg(QString::number(loja_.faturamentoTotal(), 'f', 2),
                 QString::number(loja_.lucroEstimado(), 'f', 2)));

        const auto& ps = loja_.produtos();
        maisVendidos->setRowCount(static_cast<int>(ps.size()));
        int linhas = 0;
        for (const auto& p : ps) {
            int q = loja_.totalVendidoDoProduto(p.id);
            if (q > 0) {
                maisVendidos->setItem(linhas, 0, new QTableWidgetItem(QString::fromStdString(p.nome)));
                maisVendidos->setItem(linhas, 1, new QTableWidgetItem(QString::number(q)));
                ++linhas;
            }
        }
        maisVendidos->setRowCount(linhas);

        const auto& h = loja_.historico();
        historico->setRowCount(static_cast<int>(h.size()));
        for (int i = 0; i < static_cast<int>(h.size()); ++i) {
            const Venda& v = h[static_cast<std::size_t>(i)];
            historico->setItem(i, 0, new QTableWidgetItem(QString::number(v.id)));
            historico->setItem(i, 1, new QTableWidgetItem(QString::number(v.produtoId)));
            historico->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(v.funcionario)));
            historico->setItem(i, 3, new QTableWidgetItem(QString::number(v.quantidade)));
            historico->setItem(i, 4, new QTableWidgetItem(QString::number(v.total, 'f', 2)));
            historico->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(v.data)));
        }
    };

    layout->addWidget(cardResumo);
    layout->addWidget(tituloSecao("Produtos mais vendidos"));
    layout->addWidget(maisVendidos, 1);
    layout->addWidget(tituloSecao("Historico de vendas"));
    layout->addWidget(historico, 2);

    refrescar_.push_back(preencher);
    return w;
}

// ----------------------------------------------------------------------------
//  Desempenho (todos): total, comissao, meta e o historico do proprio usuario
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaDesempenho() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Desempenho",
                         "Seu total vendido, comissao, meta e historico pessoal.", &layout);

    auto* info = new QLabel;
    info->setTextFormat(Qt::RichText);

    auto* cardInfo = cartao();
    auto* cl = new QVBoxLayout(cardInfo);
    cl->setContentsMargins(0, 0, 0, 0);   // o respiro ja vem do padding do #card
    cl->addWidget(info);

    auto* metaBtn = new QPushButton("Definir / atualizar meta", w);
    metaBtn->setObjectName("primaryBtn");

    auto* historico = new QTableWidget(0, 5, w);
    historico->setHorizontalHeaderLabels({"Venda", "Produto", "Qtd", "Total", "Data"});
    prepararTabela(historico);

    auto preencher = [this, info, historico]() {
        const Usuario& u   = loja_.usuarioAtual();
        double         com = u.comissao(loja_.percentualComissao());
        QString status = u.metaAtingida()
            ? "<span style='color:#16a34a'><b>META ATINGIDA!</b></span>"
            : "Meta ainda nao atingida.";
        info->setText(QString(
            "<b>Funcionario:</b> %1<br>"
            "<b>Total vendido:</b> R$ %2<br>"
            "<b>Comissao (%3%):</b> R$ %4<br>"
            "<b>Meta:</b> R$ %5<br>%6")
            .arg(QString::fromStdString(u.usuario),
                 QString::number(u.totalVendido, 'f', 2),
                 QString::number(loja_.percentualComissao() * 100, 'f', 0),
                 QString::number(com, 'f', 2),
                 QString::number(u.meta, 'f', 2),
                 status));

        const QString nome = QString::fromStdString(u.usuario);
        const auto& h = loja_.historico();
        historico->setRowCount(static_cast<int>(h.size()));
        int linhas = 0;
        for (const auto& v : h) {
            if (QString::fromStdString(v.funcionario) == nome) {
                historico->setItem(linhas, 0, new QTableWidgetItem(QString::number(v.id)));
                historico->setItem(linhas, 1, new QTableWidgetItem(QString::number(v.produtoId)));
                historico->setItem(linhas, 2, new QTableWidgetItem(QString::number(v.quantidade)));
                historico->setItem(linhas, 3, new QTableWidgetItem(QString::number(v.total, 'f', 2)));
                historico->setItem(linhas, 4, new QTableWidgetItem(QString::fromStdString(v.data)));
                ++linhas;
            }
        }
        historico->setRowCount(linhas);
    };

    connect(metaBtn, &QPushButton::clicked, w, [this, preencher, w]() {
        bool   ok    = false;
        double atual = loja_.usuarioAtual().meta;
        double m = QInputDialog::getDouble(w, "Definir meta", "Meta de vendas (R$):",
                                           atual, 0, 1e9, 2, &ok);
        if (ok) { loja_.definirMeta(m); preencher(); }
    });

    auto* barraMeta = new QHBoxLayout;
    barraMeta->addWidget(metaBtn);
    barraMeta->addStretch();

    layout->addWidget(cardInfo);
    layout->addLayout(barraMeta);
    layout->addWidget(tituloSecao("Meu historico de vendas"));
    layout->addWidget(historico, 1);

    refrescar_.push_back(preencher);
    return w;
}

// ----------------------------------------------------------------------------
//  Perfil (todos): identidade do usuario logado + indicadores rapidos
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaPerfil() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Perfil", "Sua identidade e desempenho nesta organizacao.", &layout);

    // cartao de identidade (avatar + nome + papel/org)
    auto* card = cartao();
    auto* cl   = new QHBoxLayout(card);
    cl->setContentsMargins(0, 0, 0, 0);
    cl->setSpacing(16);
    auto* av   = new QLabel; av->setObjectName("avatarLg");
    av->setFixedSize(76, 76); av->setAlignment(Qt::AlignCenter);
    auto* nome = new QLabel; nome->setObjectName("profileName");
    auto* sub  = new QLabel; sub->setObjectName("muted");
    auto* idcol = new QVBoxLayout; idcol->setSpacing(2);
    idcol->addStretch();
    idcol->addWidget(nome);
    idcol->addWidget(sub);
    idcol->addStretch();
    cl->addWidget(av);
    cl->addLayout(idcol);
    cl->addStretch();
    layout->addWidget(card);

    // indicadores
    auto* kpis = new QHBoxLayout; kpis->setSpacing(12);
    auto kpiCard = [&](const QString& rotulo) -> QLabel* {
        auto* c  = cartao();
        auto* cv = new QVBoxLayout(c); cv->setContentsMargins(0, 0, 0, 0); cv->setSpacing(2);
        auto* r  = new QLabel(rotulo); r->setObjectName("muted");
        auto* val = new QLabel("-"); val->setObjectName("kpi");
        cv->addWidget(r);
        cv->addWidget(val);
        kpis->addWidget(c, 1);
        return val;
    };
    QLabel* kTotal = kpiCard("Total vendido");
    QLabel* kCom   = kpiCard("Comissao");
    QLabel* kMeta  = kpiCard("Meta");
    layout->addLayout(kpis);
    layout->addStretch();

    auto preencher = [this, av, nome, sub, kTotal, kCom, kMeta]() {
        const Usuario& u = loja_.usuarioAtual();
        av->setText(iniciais(u.usuario));
        nome->setText(QString::fromStdString(u.usuario));
        QString papel = loja_.ehDono(u.usuario) ? "Administrador (dono)"
                        : (u.isAdmin() ? "Administrador" : "Funcionario");
        sub->setText(QString("%1   -   Organizacao \"%2\"")
            .arg(papel, QString::fromStdString(loja_.org())));
        kTotal->setText(QString("R$ %1").arg(QString::number(u.totalVendido, 'f', 2)));
        kCom->setText(QString("R$ %1").arg(QString::number(u.comissao(loja_.percentualComissao()), 'f', 2)));
        kMeta->setText(u.meta > 0.0 ? QString("R$ %1").arg(QString::number(u.meta, 'f', 2)) : "sem meta");
    };

    refrescar_.push_back(preencher);
    return w;
}

// ----------------------------------------------------------------------------
//  Configuracoes (todos): trocar senha, dados da organizacao e sobre
// ----------------------------------------------------------------------------

QWidget* MainWindow::criarAbaConfiguracoes() {
    QVBoxLayout* layout = nullptr;
    auto* w = paginaBase("Configuracoes", "Sua conta, a organizacao e informacoes do app.", &layout);

    // --- trocar senha ---
    layout->addWidget(tituloSecao("Trocar senha"));
    auto* cardSenha = cartao();
    auto* sl = new QVBoxLayout(cardSenha);
    sl->setContentsMargins(0, 0, 0, 0); sl->setSpacing(10);
    auto* form = new QFormLayout;
    form->setContentsMargins(0, 0, 0, 0);
    auto* atual = new QLineEdit; atual->setEchoMode(QLineEdit::Password); atual->setPlaceholderText("sua senha atual");
    auto* nova  = new QLineEdit; nova->setEchoMode(QLineEdit::Password);  nova->setPlaceholderText("nova senha");
    auto* conf  = new QLineEdit; conf->setEchoMode(QLineEdit::Password);  conf->setPlaceholderText("repita a nova senha");
    form->addRow("Senha atual:", atual);
    form->addRow("Nova senha:",  nova);
    form->addRow("Confirmar:",   conf);
    sl->addLayout(form);
    auto* msg    = new QLabel; msg->setWordWrap(true);
    auto* salvar = new QPushButton("Salvar nova senha"); salvar->setObjectName("primaryBtn");
    auto* linha  = new QHBoxLayout;
    linha->addWidget(salvar);
    linha->addStretch();
    linha->addWidget(msg);
    sl->addLayout(linha);
    layout->addWidget(cardSenha);

    connect(salvar, &QPushButton::clicked, w, [this, atual, nova, conf, msg]() {
        auto setMsg = [msg](const QString& t, bool erro) {
            msg->setText(t);
            msg->setStyleSheet(erro ? "color:#dc2626; font-weight:bold;"
                                    : "color:#16a34a; font-weight:bold;");
        };
        if (nova->text().isEmpty())       { setMsg("Informe a nova senha.", true);  return; }
        if (nova->text() != conf->text()) { setMsg("As senhas nao conferem.", true); return; }
        if (!loja_.mudarSenha(atual->text().toStdString(), nova->text().toStdString())) {
            setMsg("Senha atual incorreta.", true);
            return;
        }
        atual->clear(); nova->clear(); conf->clear();
        setMsg("Senha alterada com sucesso.", false);
    });

    // --- organizacao (admin pode renomear; o nome so muda aqui) ---
    layout->addWidget(tituloSecao("Organizacao"));
    auto* cardOrg = cartao();
    auto* ol = new QVBoxLayout(cardOrg);
    ol->setContentsMargins(0, 0, 0, 0); ol->setSpacing(10);

    const bool adm = loja_.usuarioAtual().isAdmin();
    if (adm) {
        auto* fo = new QFormLayout; fo->setContentsMargins(0, 0, 0, 0);
        auto* nomeEdit = new QLineEdit(QString::fromStdString(loja_.org()));
        fo->addRow("Nome da organizacao:", nomeEdit);
        ol->addLayout(fo);
        auto* msgOrg    = new QLabel; msgOrg->setWordWrap(true);
        auto* salvarOrg = new QPushButton("Salvar nome"); salvarOrg->setObjectName("primaryBtn");
        auto* lo = new QHBoxLayout;
        lo->addWidget(salvarOrg);
        lo->addStretch();
        lo->addWidget(msgOrg);
        ol->addLayout(lo);
        connect(salvarOrg, &QPushButton::clicked, w, [this, nomeEdit, msgOrg]() {
            auto setMsg = [msgOrg](const QString& t, bool erro) {
                msgOrg->setText(t);
                msgOrg->setStyleSheet(erro ? "color:#dc2626; font-weight:bold;"
                                           : "color:#16a34a; font-weight:bold;");
            };
            const QString n = nomeEdit->text().trimmed();
            if (n.isEmpty()) { setMsg("Informe um nome.", true); return; }
            if (!loja_.renomearOrg(n.toStdString())) { setMsg("Nao foi possivel renomear.", true); return; }
            if (brandOrg_) brandOrg_->setText(n);
            const Usuario& cu = loja_.usuarioAtual();
            setWindowTitle(QString("Sistema de Loja  -  %1  /  %2 (%3)")
                .arg(n, QString::fromStdString(cu.usuario), cu.isAdmin() ? "ADMIN" : "FUNC"));
            setMsg("Nome atualizado.", false);
        });
    } else {
        auto* fo = new QFormLayout; fo->setContentsMargins(0, 0, 0, 0);
        fo->addRow("Nome:", new QLabel(QString::fromStdString(loja_.org())));
        ol->addLayout(fo);
    }

    auto* infoForm = new QFormLayout; infoForm->setContentsMargins(0, 0, 0, 0);
    infoForm->addRow("Dono:",      new QLabel(QString::fromStdString(loja_.dono())));
    infoForm->addRow("Seu cargo:", new QLabel(adm ? "Administrador" : "Funcionario"));
    ol->addLayout(infoForm);
    layout->addWidget(cardOrg);

    // --- sobre ---
    layout->addWidget(tituloSecao("Sobre"));
    auto* cardSobre = cartao();
    auto* sv = new QVBoxLayout(cardSobre); sv->setContentsMargins(0, 0, 0, 0);
    auto* sobre = new QLabel(
        "Sistema de Gestao de Loja  --  C++17 / Qt 6 / SQLite.\n"
        "Cada organizacao tem produtos, vendas, caixa e equipe proprios e isolados.");
    sobre->setObjectName("muted"); sobre->setWordWrap(true);
    sv->addWidget(sobre);
    layout->addWidget(cardSobre);

    layout->addStretch();

    refrescar_.push_back([]() {});   // nada dinamico para atualizar
    return w;
}
