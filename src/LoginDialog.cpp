#include "LoginDialog.hpp"
#include "Loja.hpp"

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace {

// Visual da tela de acesso (QSS). Restrito a esta janela e seus filhos; tem
// prioridade sobre o tema global do app para o subtree do dialogo.
const char* ESTILO = R"(
QDialog { background: #ffffff; }

#leftPanel {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                stop:0 #4f46e5, stop:1 #7c3aed);
}
#brandMark {
    background: rgba(255,255,255,0.18); color: white;
    border-radius: 18px; font-size: 30px; font-weight: bold;
}
#appName    { color: white; font-size: 24px; font-weight: bold; }
#appTagline { color: rgba(255,255,255,0.88); font-size: 13px; }

QStackedWidget { background: #ffffff; }
#page { background: #ffffff; }

#formTitle { font-size: 24px; font-weight: bold; color: #111827; }
#formSub   { color: #6b7280; font-size: 13px; }

#fieldLabel { color: #6b7280; font-weight: bold; font-size: 11px; }

QLineEdit {
    border: 1px solid #d1d5db; border-radius: 10px;
    background: #ffffff; color: #111827; font-size: 14px;
    padding: 0 14px; min-height: 44px;
    selection-background-color: #c7d2fe; selection-color: #111827;
}
QLineEdit:focus  { border: 2px solid #4f46e5; padding: 0 13px; }
QLineEdit:hover  { border: 1px solid #a5b4fc; }

#primaryBtn {
    background: #4f46e5; color: white; border: none; border-radius: 10px;
    font-size: 15px; font-weight: bold; min-height: 48px;
}
#primaryBtn:hover   { background: #4338ca; }
#primaryBtn:pressed { background: #3730a3; }

#linkBtn {
    background: transparent; border: none; color: #4f46e5;
    font-size: 13px; font-weight: bold; padding: 4px 6px; min-height: 0;
}
#linkBtn:hover { color: #3730a3; }

#muted { color: #6b7280; font-size: 13px; }
)";

// Cria um par "rotulo + campo" com o espacamento padrao e devolve o campo.
QLineEdit* campo(QVBoxLayout* col, const QString& rotulo, const QString& placeholder,
                 bool senha = false) {
    auto* lbl = new QLabel(rotulo);
    lbl->setObjectName("fieldLabel");
    auto* edit = new QLineEdit;
    edit->setPlaceholderText(placeholder);
    if (senha) edit->setEchoMode(QLineEdit::Password);

    col->addWidget(lbl);
    col->addSpacing(4);
    col->addWidget(edit);
    col->addSpacing(14);
    return edit;
}

// Mostra uma mensagem inline (vermelha p/ erro, verde p/ sucesso).
void mostrarMsg(QLabel* l, const QString& texto, bool erro) {
    l->setText(texto);
    l->setStyleSheet(erro ? "color:#dc2626; font-size:12px; font-weight:bold;"
                          : "color:#16a34a; font-size:12px; font-weight:bold;");
}

// Linha de rodape: "<pergunta>  <link>" centralizada.
QHBoxLayout* rodapeLink(const QString& pergunta, QPushButton* link) {
    auto* row = new QHBoxLayout;
    auto* q = new QLabel(pergunta);
    q->setObjectName("muted");
    row->addStretch();
    row->addWidget(q);
    row->addWidget(link);
    row->addStretch();
    return row;
}

} // namespace

LoginDialog::LoginDialog(Loja& loja, QWidget* parent)
    : QDialog(parent), loja_(loja) {
    setWindowTitle("Sistema de Gestao de Loja");
    setMinimumSize(840, 560);
    setStyleSheet(ESTILO);

    auto* main = new QHBoxLayout(this);
    main->setContentsMargins(0, 0, 0, 0);
    main->setSpacing(0);
    main->addWidget(construirPainelMarca(), 4);

    stack_ = new QStackedWidget(this);
    stack_->addWidget(construirPaginaLogin());      // indice 0
    stack_->addWidget(construirPaginaRegistro());   // indice 1
    main->addWidget(stack_, 5);
}

// -------------------- painel esquerdo (marca) --------------------

QWidget* LoginDialog::construirPainelMarca() {
    auto* left = new QFrame;
    left->setObjectName("leftPanel");
    left->setMinimumWidth(320);

    auto* mark = new QLabel("SL", left);
    mark->setObjectName("brandMark");
    mark->setFixedSize(72, 72);
    mark->setAlignment(Qt::AlignCenter);

    auto* name = new QLabel("Sistema de\nGestao de Loja", left);
    name->setObjectName("appName");

    auto* tag = new QLabel("Entre com usuario e senha.\nCada conta tem a sua organizacao.", left);
    tag->setObjectName("appTagline");

    auto* ll = new QVBoxLayout(left);
    ll->setContentsMargins(40, 40, 40, 40);
    ll->addStretch();
    ll->addWidget(mark);
    ll->addSpacing(18);
    ll->addWidget(name);
    ll->addSpacing(8);
    ll->addWidget(tag);
    ll->addStretch();
    return left;
}

// -------------------- pagina de login --------------------

QWidget* LoginDialog::construirPaginaLogin() {
    auto* page = new QWidget;
    page->setObjectName("page");
    auto* col = new QVBoxLayout(page);
    col->setContentsMargins(48, 52, 48, 52);
    col->setSpacing(0);

    auto* title = new QLabel("Bem-vindo de volta"); title->setObjectName("formTitle");
    auto* sub   = new QLabel("Entre com sua conta para continuar."); sub->setObjectName("formSub");
    col->addWidget(title);
    col->addSpacing(6);
    col->addWidget(sub);
    col->addSpacing(28);

    loginUser_ = campo(col, "USUARIO", "seu usuario");
    loginPass_ = campo(col, "SENHA",   "sua senha", /*senha=*/true);

    col->addSpacing(4);
    auto* entrar = new QPushButton("Entrar");
    entrar->setObjectName("primaryBtn");
    entrar->setCursor(Qt::PointingHandCursor);
    col->addWidget(entrar);

    col->addSpacing(10);
    loginMsg_ = new QLabel;
    loginMsg_->setWordWrap(true);
    col->addWidget(loginMsg_);

    col->addStretch();

    auto* irRegistro = new QPushButton("Criar conta");
    irRegistro->setObjectName("linkBtn");
    irRegistro->setCursor(Qt::PointingHandCursor);
    col->addLayout(rodapeLink("Nao tem uma conta?", irRegistro));

    connect(entrar,     &QPushButton::clicked,     this, &LoginDialog::tentarEntrar);
    connect(loginUser_, &QLineEdit::returnPressed, this, &LoginDialog::tentarEntrar);
    connect(loginPass_, &QLineEdit::returnPressed, this, &LoginDialog::tentarEntrar);
    connect(irRegistro, &QPushButton::clicked, this, [this]() {
        regMsg_->clear();
        stack_->setCurrentIndex(1);
        regUser_->setFocus();
    });

    return page;
}

// -------------------- pagina de cadastro --------------------

QWidget* LoginDialog::construirPaginaRegistro() {
    auto* page = new QWidget;
    page->setObjectName("page");
    auto* col = new QVBoxLayout(page);
    col->setContentsMargins(48, 52, 48, 52);
    col->setSpacing(0);

    auto* title = new QLabel("Criar conta"); title->setObjectName("formTitle");
    auto* sub   = new QLabel("Voce sera o administrador da sua organizacao."); sub->setObjectName("formSub");
    col->addWidget(title);
    col->addSpacing(6);
    col->addWidget(sub);
    col->addSpacing(28);

    regUser_ = campo(col, "USUARIO",         "escolha um usuario");
    regPass_ = campo(col, "SENHA",           "crie uma senha",  /*senha=*/true);
    regConf_ = campo(col, "CONFIRMAR SENHA", "repita a senha",  /*senha=*/true);

    col->addSpacing(4);
    auto* criar = new QPushButton("Criar conta e entrar");
    criar->setObjectName("primaryBtn");
    criar->setCursor(Qt::PointingHandCursor);
    col->addWidget(criar);

    col->addSpacing(10);
    regMsg_ = new QLabel;
    regMsg_->setWordWrap(true);
    col->addWidget(regMsg_);

    col->addStretch();

    auto* irLogin = new QPushButton("Entrar");
    irLogin->setObjectName("linkBtn");
    irLogin->setCursor(Qt::PointingHandCursor);
    col->addLayout(rodapeLink("Ja tem uma conta?", irLogin));

    connect(criar,    &QPushButton::clicked,     this, &LoginDialog::criarConta);
    connect(regConf_, &QLineEdit::returnPressed, this, &LoginDialog::criarConta);
    connect(irLogin,  &QPushButton::clicked, this, [this]() {
        loginMsg_->clear();
        stack_->setCurrentIndex(0);
        loginUser_->setFocus();
    });

    return page;
}

// -------------------- acoes --------------------

void LoginDialog::tentarEntrar() {
    const QString u = loginUser_->text().trimmed();
    const QString s = loginPass_->text();
    if (loja_.entrar(u.toStdString(), s.toStdString())) {
        accept();
        return;
    }
    mostrarMsg(loginMsg_, "Usuario ou senha invalidos.", /*erro=*/true);
    loginPass_->clear();
    loginPass_->setFocus();
}

void LoginDialog::criarConta() {
    const QString u = regUser_->text().trimmed();
    const QString s = regPass_->text();
    const QString c = regConf_->text();

    if (u.isEmpty()) { mostrarMsg(regMsg_, "Informe um nome de usuario.", true); regUser_->setFocus(); return; }
    if (s.isEmpty()) { mostrarMsg(regMsg_, "Informe uma senha.",          true); regPass_->setFocus(); return; }
    if (s != c)      { mostrarMsg(regMsg_, "As senhas nao conferem.",     true); regConf_->setFocus(); return; }

    if (loja_.usuarioExiste(u.toStdString())) {
        mostrarMsg(regMsg_, "Esse usuario ja existe. Escolha outro ou faca login.", true);
        regUser_->setFocus();
        return;
    }
    if (!loja_.registrarConta(u.toStdString(), s.toStdString())) {
        mostrarMsg(regMsg_, "Nao foi possivel criar a conta.", true);
        return;
    }
    // criada e ja logado como admin dono -> entra direto
    accept();
}
