#include "Loja.hpp"
#include "LoginDialog.hpp"
#include "MainWindow.hpp"
#include "Theme.hpp"
#include "AppIcon.hpp"

#include <QApplication>
#include <QDialog>
#include <QTimer>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Sistema de Gestao de Loja");
    app.setStyleSheet(theme::APP_QSS);   // tema visual moderno, aplicado a todo o app
    app.setWindowIcon(appIcon());        // icone proprio (cabecalho + barra de tarefas)

    // Banco SEMPRE ao lado do executavel (e nao no diretorio de trabalho atual),
    // para que os dados persistam independentemente de como o app foi iniciado.
    const std::string dbPath =
        (QApplication::applicationDirPath() + "/loja.db").toStdString();
    Loja loja(0.05, dbPath);   // comissao padrao de 5%
    loja.abrirBanco();         // abre o banco SQLite (cada org se cadastra/loga na tela)

    // Modo de verificacao automatica (usado em testes headless): com a variavel
    // de ambiente GESTAO_SMOKE definida, loga o admin, abre a janela principal e
    // a fecha sozinho -- serve para checar que toda a UI monta sem travar.
    // (a tela de login com gradiente nao renderiza no platform "offscreen";
    // por isso o smoke headless exercita a janela principal. O login foi
    // verificado abrindo o app normalmente.)
    if (qEnvironmentVariableIsSet("GESTAO_SMOKE")) {
        if (!loja.entrar("admin", "123"))
            loja.registrarConta("admin", "123");
        MainWindow janela(loja);
        janela.show();
        QTimer::singleShot(1500, &app, &QCoreApplication::quit);
        return app.exec();
    }

    // Laco de sessao: faz login -> abre a janela principal -> ao fechar
    // (logout), volta para o login. Cancelar o login encerra o programa.
    while (true) {
        LoginDialog login(loja);
        if (login.exec() != QDialog::Accepted)
            break;

        MainWindow janela(loja);
        janela.show();
        app.exec();
        loja.logout();
    }
    return 0;
}
