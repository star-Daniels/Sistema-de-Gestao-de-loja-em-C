#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <functional>
#include <vector>

class Loja;
class QLabel;
class QStackedWidget;
class QButtonGroup;
class QVBoxLayout;

// Janela principal: um rail de navegacao a esquerda + uma area de conteudo
// (QStackedWidget) a direita. As secoes sao montadas conforme o cargo do usuario
// logado (ADMIN ve tudo; FUNC ve um subconjunto). Cada secao registra uma funcao
// de "refresh" em refrescar_ (alinhada ao indice da secao), chamada ao abri-la.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(Loja& loja, QWidget* parent = nullptr);

private:
    Loja&           loja_;
    QStackedWidget* stack_     = nullptr;
    QButtonGroup*   navGroup_  = nullptr;   // botoes do rail (exclusivos)
    QVBoxLayout*    navLayout_ = nullptr;   // onde os botoes do rail entram
    QLabel*         brandOrg_  = nullptr;   // nome da org no rail (atualizado ao renomear)
    std::vector<std::function<void()>> refrescar_;   // 1 por secao (alinhado ao indice)

    QWidget* construirSidebar();
    void     adicionarSecao(const QString& titulo, QWidget* pagina);

    QWidget* criarAbaProdutos();
    QWidget* criarAbaVendas();
    QWidget* criarAbaCaixa();
    QWidget* criarAbaUsuarios();
    QWidget* criarAbaRelatorios();
    QWidget* criarAbaDesempenho();
    QWidget* criarAbaPerfil();
    QWidget* criarAbaConfiguracoes();
};

#endif // MAINWINDOW_HPP
