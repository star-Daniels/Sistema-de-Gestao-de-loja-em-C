#ifndef APPICON_HPP
#define APPICON_HPP

#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QRectF>
#include <QColor>

// Icone do app desenhado em codigo (sem arquivo externo): quadrado arredondado
// com gradiente indigo->violeta e o monograma "SL", no mesmo tom da tela de
// login. Aplicado via QApplication::setWindowIcon para aparecer no cabecalho da
// janela e na barra de tarefas, substituindo o icone padrao do Windows.
inline QIcon appIcon() {
    QIcon icon;
    for (int sz : { 16, 24, 32, 48, 64, 128, 256 }) {
        QPixmap pm(sz, sz);
        pm.fill(Qt::transparent);

        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::TextAntialiasing, true);

        QLinearGradient g(0, 0, sz, sz);
        g.setColorAt(0.0, QColor(0x4f, 0x46, 0xe5));   // indigo
        g.setColorAt(1.0, QColor(0x7c, 0x3a, 0xed));   // violeta
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        const qreal r = sz * 0.24;
        p.drawRoundedRect(QRectF(0.5, 0.5, sz - 1.0, sz - 1.0), r, r);

        QFont f("Segoe UI");
        f.setBold(true);
        f.setPixelSize(int(sz * 0.46));
        p.setFont(f);
        p.setPen(Qt::white);
        p.drawText(QRectF(0, -sz * 0.02, sz, sz), Qt::AlignCenter, "SL");
        p.end();

        icon.addPixmap(pm);
    }
    return icon;
}

#endif // APPICON_HPP
