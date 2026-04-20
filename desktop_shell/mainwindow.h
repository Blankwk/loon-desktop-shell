#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidget;
class QStackedWidget;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onPageChanged(int index);

private:
    void setupUi();
    void setupStyle();

private:
    QListWidget *m_navList = nullptr;
    QStackedWidget *m_stack = nullptr;
    QLabel *m_titleLabel = nullptr;
};

#endif
