#ifndef THEME_HPP
#define THEME_HPP

// Tema visual do app (Qt Widgets QSS), aplicado uma vez no QApplication em
// main.cpp. Identidade indigo/violeta, no mesmo tom da tela de login, para um
// visual de painel moderno: cartoes brancos, cantos arredondados, abas em
// controle segmentado, tabelas com linhas alternadas e botoes primario/
// secundario/perigo (objectName primaryBtn / secondaryBtn / dangerBtn).
namespace theme {

inline const char* const APP_QSS = R"qss(
/* ============================================================
   Sistema de Gestao de Loja - App-wide Qt 6 Widgets stylesheet
   Airy modern SaaS dashboard. Indigo/violet identity.
   ============================================================ */

/* ---- Global baseline ---- */
* {
    font-family: "Segoe UI", "Segoe UI Variable", sans-serif;
    font-size: 13px;
    color: #374151;
}

QMainWindow, QDialog {
    background: #f1f5f9;
}

QDialog {
    background: #ffffff;
}

QWidget#page {
    background: transparent;
}

QToolTip {
    background: #111827;
    color: #ffffff;
    border: none;
    border-radius: 6px;
    padding: 5px 8px;
    font-size: 12px;
}

/* ---- Cards / surfaces ---- */
QFrame#card {
    background: #ffffff;
    border: 1px solid #e5e7eb;
    border-radius: 12px;
    padding: 14px;
}

/* ---- Typographic roles ---- */
QLabel {
    color: #374151;
    background: transparent;
}

QLabel#pageTitle {
    font-size: 21px;
    font-weight: bold;
    color: #111827;
    padding: 2px 0;
}

QLabel#sectionTitle {
    font-size: 14px;
    font-weight: bold;
    color: #374151;
    padding: 2px 0;
}

QLabel#muted {
    font-size: 12px;
    color: #6b7280;
}

QLabel#kpi {
    font-size: 22px;
    font-weight: bold;
    color: #111827;
}

QLabel:disabled {
    color: #9ca3af;
}

/* ---- Buttons: default = neutral / secondary ---- */
QPushButton {
    background: #ffffff;
    color: #374151;
    border: 1px solid #d1d5db;
    border-radius: 8px;
    padding: 8px 15px;
    min-height: 18px;
    font-size: 13px;
    font-weight: 600;
}
QPushButton:hover {
    background: #f9fafb;
    border: 1px solid #c7d2fe;
    color: #111827;
}
QPushButton:pressed {
    background: #f3f4f6;
}
QPushButton:focus {
    border: 1px solid #4f46e5;
    outline: none;
}
QPushButton:disabled {
    background: #f3f4f6;
    color: #9ca3af;
    border: 1px solid #e5e7eb;
}

/* Primary - filled indigo (matches login #primaryBtn) */
QPushButton#primaryBtn {
    background: #4f46e5;
    color: #ffffff;
    border: none;
    border-radius: 8px;
    padding: 9px 16px;
    min-height: 34px;
    font-weight: bold;
}
QPushButton#primaryBtn:hover   { background: #4338ca; }
QPushButton#primaryBtn:pressed { background: #3730a3; }
QPushButton#primaryBtn:disabled {
    background: #c7d2fe;
    color: #eef2ff;
}

/* Secondary - outlined accent */
QPushButton#secondaryBtn {
    background: transparent;
    color: #4f46e5;
    border: 1px solid #c7d2fe;
    border-radius: 8px;
    padding: 9px 16px;
    min-height: 34px;
    font-weight: bold;
}
QPushButton#secondaryBtn:hover {
    background: #eef2ff;
    border: 1px solid #a5b4fc;
}
QPushButton#secondaryBtn:pressed { background: #e0e7ff; }
QPushButton#secondaryBtn:disabled {
    color: #9ca3af;
    border: 1px solid #e5e7eb;
}

/* Danger - red text / border */
QPushButton#dangerBtn {
    background: #ffffff;
    color: #dc2626;
    border: 1px solid #fecaca;
    border-radius: 8px;
    padding: 9px 16px;
    min-height: 34px;
    font-weight: bold;
}
QPushButton#dangerBtn:hover {
    background: #fef2f2;
    border: 1px solid #fca5a5;
    color: #b91c1c;
}
QPushButton#dangerBtn:pressed { background: #fee2e2; }
QPushButton#dangerBtn:disabled {
    color: #9ca3af;
    border: 1px solid #e5e7eb;
}

/* ---- Inputs ---- */
QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
    background: #ffffff;
    color: #111827;
    border: 1px solid #d1d5db;
    border-radius: 8px;
    padding: 8px 10px;
    min-height: 18px;
    selection-background-color: #eef2ff;
    selection-color: #111827;
}
QLineEdit:hover, QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover {
    border: 1px solid #c7d2fe;
}
QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus {
    border: 1px solid #4f46e5;
}
QLineEdit:disabled, QComboBox:disabled, QSpinBox:disabled, QDoubleSpinBox:disabled {
    background: #f3f4f6;
    color: #9ca3af;
    border: 1px solid #e5e7eb;
}

/* ComboBox drop-down + popup */
QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: center right;
    width: 22px;
    border: none;
    border-left: 1px solid #e5e7eb;
    margin: 2px;
}
QComboBox::down-arrow {
    image: none;
    width: 0px;
    height: 0px;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid #6b7280;
    margin-right: 8px;
}
QComboBox::down-arrow:disabled {
    border-top: 5px solid #d1d5db;
}
QComboBox:on {
    border: 1px solid #4f46e5;
}
QComboBox QAbstractItemView {
    background: #ffffff;
    border: 1px solid #e5e7eb;
    border-radius: 8px;
    padding: 4px;
    outline: none;
    selection-background-color: #eef2ff;
    selection-color: #111827;
}
QComboBox QAbstractItemView::item {
    min-height: 24px;
    padding: 4px 8px;
    border-radius: 6px;
    color: #374151;
}

/* SpinBox up / down buttons */
QSpinBox::up-button, QDoubleSpinBox::up-button {
    subcontrol-origin: border;
    subcontrol-position: top right;
    width: 20px;
    border: none;
    border-left: 1px solid #e5e7eb;
    border-top-right-radius: 8px;
    background: #f9fafb;
}
QSpinBox::down-button, QDoubleSpinBox::down-button {
    subcontrol-origin: border;
    subcontrol-position: bottom right;
    width: 20px;
    border: none;
    border-left: 1px solid #e5e7eb;
    border-bottom-right-radius: 8px;
    background: #f9fafb;
}
QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover,
QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover {
    background: #eef2ff;
}
QSpinBox::up-arrow, QDoubleSpinBox::up-arrow {
    image: none; width: 0px; height: 0px;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-bottom: 5px solid #6b7280;
}
QSpinBox::down-arrow, QDoubleSpinBox::down-arrow {
    image: none; width: 0px; height: 0px;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid #6b7280;
}

/* ---- Tables ---- */
QTableWidget, QTableView {
    background: #ffffff;
    alternate-background-color: #f9fafb;
    gridline-color: #f1f5f9;
    border: 1px solid #e5e7eb;
    border-radius: 10px;
    selection-background-color: #eef2ff;
    selection-color: #111827;
    color: #374151;
    outline: none;
}
QTableWidget::item, QTableView::item {
    padding: 6px 8px;
    border: none;
}
QTableWidget::item:selected, QTableView::item:selected {
    background: #eef2ff;
    color: #111827;
}

QHeaderView {
    background: transparent;
    border: none;
}
QHeaderView::section {
    background: #f3f4f6;
    color: #374151;
    font-weight: bold;
    padding: 8px 10px;
    border: none;
    border-bottom: 1px solid #e5e7eb;
    border-right: 1px solid #eef0f3;
}
QHeaderView::section:first {
    border-top-left-radius: 10px;
}
QHeaderView::section:last {
    border-top-right-radius: 10px;
    border-right: none;
}
QHeaderView::section:hover {
    background: #eef2ff;
}
QTableCornerButton::section {
    background: #f3f4f6;
    border: none;
    border-bottom: 1px solid #e5e7eb;
}

/* ---- Tab widget: underlined segmented control ---- */
QTabWidget::pane {
    border: none;
    border-top: 1px solid #e5e7eb;
    background: transparent;
    top: -1px;
}
QTabBar {
    background: transparent;
    qproperty-drawBase: 0;
}
QTabBar::tab {
    background: transparent;
    color: #6b7280;
    border: none;
    border-bottom: 2px solid transparent;
    padding: 9px 18px;
    margin-right: 2px;
    font-size: 13px;
    font-weight: 600;
}
QTabBar::tab:hover:!selected {
    color: #4f46e5;
    background: #eef2ff;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
}
QTabBar::tab:selected {
    color: #4f46e5;
    font-weight: bold;
    border-bottom: 2px solid #4f46e5;
}
QTabBar::tab:disabled {
    color: #c7d2fe;
}

/* ---- Toolbar ---- */
QToolBar, QToolBar#appToolbar {
    background: #ffffff;
    border: none;
    border-bottom: 1px solid #e5e7eb;
    padding: 4px 8px;
    spacing: 4px;
}
QToolBar::separator {
    background: #e5e7eb;
    width: 1px;
    margin: 4px 6px;
}
QToolBar QToolButton {
    background: transparent;
    color: #374151;
    border: 1px solid transparent;
    border-radius: 8px;
    padding: 6px 12px;
    font-weight: 600;
}
QToolBar QToolButton:hover {
    background: #eef2ff;
    color: #4f46e5;
}
QToolBar QToolButton:pressed {
    background: #e0e7ff;
}

/* ---- Status bar ---- */
QStatusBar {
    background: #ffffff;
    color: #6b7280;
    border: none;
    border-top: 1px solid #e5e7eb;
}
QStatusBar::item {
    border: none;
}
QStatusBar QLabel {
    color: #6b7280;
}

/* ---- Menus (context / QMenuBar drop-downs) ---- */
QMenu {
    background: #ffffff;
    border: 1px solid #e5e7eb;
    border-radius: 8px;
    padding: 4px;
}
QMenu::item {
    padding: 6px 22px 6px 14px;
    border-radius: 6px;
    color: #374151;
}
QMenu::item:selected {
    background: #eef2ff;
    color: #4f46e5;
}
QMenu::separator {
    height: 1px;
    background: #e5e7eb;
    margin: 4px 6px;
}

/* ---- Dialogs / message box ---- */
QFormLayout QLabel,
QMessageBox QLabel {
    color: #374151;
}
QMessageBox {
    background: #ffffff;
}
QDialogButtonBox QPushButton {
    min-width: 84px;
}

/* ---- Group boxes (unnamed grouped forms) ---- */
QGroupBox {
    background: #ffffff;
    border: 1px solid #e5e7eb;
    border-radius: 12px;
    margin-top: 12px;
    padding: 14px;
    font-weight: bold;
    color: #374151;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 12px;
    padding: 0 4px;
    color: #111827;
}

/* ---- Scrollbars ---- */
QScrollBar:vertical {
    background: transparent;
    width: 12px;
    margin: 2px;
}
QScrollBar::handle:vertical {
    background: #d1d5db;
    border-radius: 5px;
    min-height: 28px;
}
QScrollBar::handle:vertical:hover {
    background: #9ca3af;
}
QScrollBar:horizontal {
    background: transparent;
    height: 12px;
    margin: 2px;
}
QScrollBar::handle:horizontal {
    background: #d1d5db;
    border-radius: 5px;
    min-width: 28px;
}
QScrollBar::handle:horizontal:hover {
    background: #9ca3af;
}
QScrollBar::add-line, QScrollBar::sub-line {
    width: 0px; height: 0px; background: none; border: none;
}
QScrollBar::add-page, QScrollBar::sub-page {
    background: transparent;
}

/* ---- Misc form widgets ---- */
QCheckBox, QRadioButton {
    color: #374151;
    spacing: 8px;
}
QCheckBox::indicator, QRadioButton::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #d1d5db;
    background: #ffffff;
}
QCheckBox::indicator {
    border-radius: 4px;
}
QRadioButton::indicator {
    border-radius: 8px;
}
QCheckBox::indicator:checked, QRadioButton::indicator:checked {
    background: #4f46e5;
    border: 1px solid #4f46e5;
}
QCheckBox::indicator:hover, QRadioButton::indicator:hover {
    border: 1px solid #4f46e5;
}

/* ---- Shell: rail de navegacao, chip de usuario e perfil ---- */
QFrame#sidebar {
    background: #ffffff;
    border: none;
    border-right: 1px solid #e5e7eb;
}
QLabel#brandSmall {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4f46e5, stop:1 #7c3aed);
    color: #ffffff; border-radius: 10px; font-size: 15px; font-weight: bold;
}
QLabel#brandName { color: #111827; font-size: 14px; font-weight: bold; }
QLabel#brandOrg  { color: #6b7280; font-size: 11px; }

QPushButton#navBtn {
    text-align: left;
    background: transparent;
    color: #4b5563;
    border: none;
    border-radius: 9px;
    padding: 9px 12px;
    font-size: 13px;
    font-weight: 600;
    min-height: 20px;
}
QPushButton#navBtn:hover   { background: #f3f4f6; color: #111827; }
QPushButton#navBtn:checked { background: #eef2ff; color: #4f46e5; }

QFrame#userChip {
    background: #f9fafb;
    border: 1px solid #eef0f3;
    border-radius: 12px;
}
QLabel#avatarSm {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4f46e5, stop:1 #7c3aed);
    color: #ffffff; border-radius: 17px; font-size: 13px; font-weight: bold;
}
QLabel#chipName { color: #111827; font-size: 13px; font-weight: bold; }
QLabel#chipRole { color: #6b7280; font-size: 11px; }

QPushButton#logoutBtn {
    text-align: left;
    background: #ffffff;
    color: #dc2626;
    border: 1px solid #fecaca;
    border-radius: 9px;
    padding: 9px 12px;
    font-weight: bold;
    min-height: 20px;
}
QPushButton#logoutBtn:hover   { background: #fef2f2; border: 1px solid #fca5a5; }
QPushButton#logoutBtn:pressed { background: #fee2e2; }

QStackedWidget#content { background: #f1f5f9; }

QLabel#avatarLg {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4f46e5, stop:1 #7c3aed);
    color: #ffffff; border-radius: 38px; font-size: 27px; font-weight: bold;
}
QLabel#profileName { color: #111827; font-size: 20px; font-weight: bold; }
)qss";

} // namespace theme

#endif // THEME_HPP
