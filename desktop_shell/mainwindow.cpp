#include "mainwindow.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setupStyle();

    m_navList->setCurrentRow(0);
    m_stack->setCurrentIndex(0);
    m_titleLabel->setText("桌面首页");

    connect(m_navList, SIGNAL(currentRowChanged(int)),
            this, SLOT(onPageChanged(int)));

    statusBar()->showMessage("系统已启动");
}

void MainWindow::setupUi()
{
    setWindowTitle("Desktop Shell");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 左侧导航
    QFrame *sidebarFrame = new QFrame(this);
    sidebarFrame->setObjectName("sidebarFrame");
    sidebarFrame->setFixedWidth(220);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarFrame);
    sidebarLayout->setContentsMargins(16, 16, 16, 16);
    sidebarLayout->setSpacing(12);

    QLabel *logoLabel = new QLabel("My Desktop", sidebarFrame);
    logoLabel->setObjectName("logoLabel");

    m_navList = new QListWidget(sidebarFrame);
    m_navList->addItem("桌面首页");
    m_navList->addItem("文件管理器");

    sidebarLayout->addWidget(logoLabel);
    sidebarLayout->addWidget(m_navList);

    // 右侧区域
    QWidget *rightContainer = new QWidget(this);
    rightContainer->setObjectName("rightContainer");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    QFrame *topBarFrame = new QFrame(this);
    topBarFrame->setObjectName("topBarFrame");
    topBarFrame->setFixedHeight(64);

    QHBoxLayout *topLayout = new QHBoxLayout(topBarFrame);
    topLayout->setContentsMargins(20, 0, 20, 0);

    m_titleLabel = new QLabel("桌面首页", topBarFrame);
    m_titleLabel->setObjectName("titleLabel");

    topLayout->addWidget(m_titleLabel);
    topLayout->addStretch();

    m_stack = new QStackedWidget(this);

    QWidget *pageHome = new QWidget(this);
    pageHome->setObjectName("pageHome");
    QVBoxLayout *homeLayout = new QVBoxLayout(pageHome);
    QLabel *homeLabel = new QLabel("这是桌面首页", pageHome);
    homeLabel->setStyleSheet(
    "font-size: 22px;"
    "color: white;"
    "background-color: rgba(15, 23, 42, 150);"
    "padding: 12px 16px;"
    "border-radius: 8px;"
    );
    homeLayout->addWidget(homeLabel);
    homeLayout->addStretch();

    QWidget *pageFile = new QWidget(this);
    pageFile->setObjectName("pageFile");
    QVBoxLayout *fileLayout = new QVBoxLayout(pageFile);
    QLabel *fileLabel = new QLabel("这是文件管理器页面", pageFile);
    fileLabel->setStyleSheet(
    "font-size: 22px;"
    "color: white;"
    "background-color: rgba(15, 23, 42, 150);"
    "padding: 12px 16px;"
    "border-radius: 8px;"
    );
    fileLayout->addWidget(fileLabel);
    fileLayout->addStretch();

    m_stack->addWidget(pageHome);
    m_stack->addWidget(pageFile);

    rightLayout->addWidget(topBarFrame);
    rightLayout->addWidget(m_stack);

    rootLayout->addWidget(sidebarFrame);
    rootLayout->addWidget(rightContainer);
}

void MainWindow::setupStyle()
{
    setStyleSheet(R"(
        QMainWindow {
            background: #111827;
        }

        /* 默认文字颜色，不再给所有 QWidget 强制涂黑背景 */
        QWidget {
            color: white;
            font-size: 16px;
        }

        /* 左侧导航栏仍然保持深色 */
        #sidebarFrame {
            background: #0f172a;
            border-right: 1px solid #1f2937;
        }

        #logoLabel {
            font-size: 24px;
            font-weight: bold;
            color: white;
            padding: 8px 4px;
            background: transparent;
        }

        /* 右侧主区域放背景图 */
        #rightContainer {
            border-image: url(:/images/bg.jpg) 0 0 0 0 stretch stretch;
        }

        /* 顶部栏做半透明，更有层次 */
        #topBarFrame {
            background: rgba(17, 24, 39, 180);
            border-bottom: 1px solid rgba(31, 41, 55, 180);
        }

        #titleLabel {
            font-size: 22px;
            font-weight: 600;
            color: white;
            background: transparent;
        }

        /* 导航列表透明，这样能融进左侧栏 */
        QListWidget {
            background: transparent;
            color: #cbd5e1;
            border: none;
            outline: none;
            font-size: 16px;
        }

        QListWidget::item {
            height: 40px;
            padding: 8px 10px;
            border-radius: 6px;
            margin: 2px 0;
        }

        QListWidget::item:selected {
            background: #2563eb;
            color: white;
        }

        QListWidget::item:hover {
            background: #1e293b;
        }

        /* 关键：页面区透明，不要挡住背景图 */
        QStackedWidget {
            background: transparent;
        }

        #pageHome, #pageFile {
            background: transparent;
        }

        QLabel {
            background: transparent;
        }

        QStatusBar {
            background: #0f172a;
            color: #cbd5e1;
        }
    )");
}

void MainWindow::onPageChanged(int index)
{
    m_stack->setCurrentIndex(index);

    if (index == 0) {
        m_titleLabel->setText("桌面首页");
        statusBar()->showMessage("当前页面：桌面首页");
    } else if (index == 1) {
        m_titleLabel->setText("文件管理器");
        statusBar()->showMessage("当前页面：文件管理器");
    }
}
