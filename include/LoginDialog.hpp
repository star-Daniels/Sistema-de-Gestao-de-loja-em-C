#ifndef LOGINDIALOG_HPP
#define LOGINDIALOG_HPP

#include <QDialog>

class Loja;
class QLineEdit;
class QLabel;
class QStackedWidget;

// Tela de acesso, em dois paineis: marca a esquerda e, a direita, um
// QStackedWidget que alterna ENTRE login e cadastro DENTRO da mesma janela.
//   - Entrar:      usuario + senha (a organizacao e resolvida pelo usuario).
//   - Criar conta: usuario + senha; abre uma organizacao nova e o usuario vira
//                  ADMIN dono dela, ja entrando logado (accept()).
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(Loja& loja, QWidget* parent = nullptr);

private slots:
    void tentarEntrar();
    void criarConta();

private:
    QWidget* construirPainelMarca();
    QWidget* construirPaginaLogin();
    QWidget* construirPaginaRegistro();

    Loja&           loja_;
    QStackedWidget* stack_ = nullptr;

    // pagina de login
    QLineEdit* loginUser_ = nullptr;
    QLineEdit* loginPass_ = nullptr;
    QLabel*    loginMsg_  = nullptr;

    // pagina de cadastro
    QLineEdit* regUser_ = nullptr;
    QLineEdit* regPass_ = nullptr;
    QLineEdit* regConf_ = nullptr;
    QLabel*    regMsg_  = nullptr;
};

#endif // LOGINDIALOG_HPP
