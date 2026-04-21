#include "filepage.h"

#include <QFileInfo>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>
#include <QDir>

FilePage::FilePage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupModel();
    setupConnections();

    // 默认打开用户主目录；如果你想从根目录开始，可以改成 "/"
    setCurrentDirectory(QDir::homePath());
}

QString FilePage::currentPath() const
{
    if (!m_model || !m_tableView) {
        return QString();
    }

    return m_model->filePath(m_tableView->rootIndex());
}

void FilePage::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(12);

    // 顶部工具栏：返回上一级 + 当前路径
    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);

    m_upButton = new QPushButton("上一级", this);
    m_upButton->setFixedWidth(100);

    m_pathLabel = new QLabel(this);
    m_pathLabel->setObjectName("filePagePathLabel");
    m_pathLabel->setMinimumHeight(36);
    m_pathLabel->setText("路径：");

    topLayout->addWidget(m_upButton);
    topLayout->addWidget(m_pathLabel, 1);

    // 中间内容：左侧目录树 + 右侧文件列表
    auto *splitter = new QSplitter(this);

    m_treeView = new QTreeView(splitter);
    m_tableView = new QTableView(splitter);

    splitter->addWidget(m_treeView);
    splitter->addWidget(m_tableView);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 5);

    rootLayout->addLayout(topLayout);
    rootLayout->addWidget(splitter, 1);

    // 一些局部样式
    m_pathLabel->setStyleSheet(
        "background-color: rgba(15, 23, 42, 160);"
        "border-radius: 6px;"
        "padding: 8px 12px;"
        "color: white;"
    );

    m_upButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(37, 99, 235, 200);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 8px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(59, 130, 246, 220);"
        "}"
    );

    m_treeView->setStyleSheet(
        "QTreeView {"
        "  background-color: rgba(15, 23, 42, 140);"
        "  border: 1px solid rgba(51, 65, 85, 180);"
        "  border-radius: 8px;"
        "}"
    );

    m_tableView->setStyleSheet(
        "QTableView {"
        "  background-color: rgba(15, 23, 42, 140);"
        "  border: 1px solid rgba(51, 65, 85, 180);"
        "  border-radius: 8px;"
        "}"
    );
}

void FilePage::setupModel()
{
    m_model = new QFileSystemModel(this);

    // 同时显示目录和文件
    m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);

    // 让 model 开始工作
    m_model->setRootPath(QDir::rootPath());

    // 左侧目录树
    m_treeView->setModel(m_model);
    m_treeView->setHeaderHidden(false);

    // 左侧树只显示“名称”列更干净
    for (int col = 1; col < m_model->columnCount(); ++col) {
        m_treeView->hideColumn(col);
    }

    // 右侧文件列表
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setSortingEnabled(false);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void FilePage::setupConnections()
{
    connect(m_treeView, &QTreeView::clicked,
            this, &FilePage::onTreeClicked);

    connect(m_tableView, &QTableView::doubleClicked,
            this, &FilePage::onTableDoubleClicked);

    connect(m_upButton, &QPushButton::clicked,
            this, &FilePage::onGoUpClicked);
}

void FilePage::setCurrentDirectory(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    QModelIndex index = m_model->index(path);
    if (!index.isValid()) {
        return;
    }

    // 左侧树跟着定位
    m_treeView->setCurrentIndex(index);
    m_treeView->scrollTo(index);

    // 右侧列表切到这个目录
    m_tableView->setRootIndex(index);

    m_pathLabel->setText(QString("当前路径：%1").arg(path));
    emit currentPathChanged(path);
}

void FilePage::onTreeClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const QString path = m_model->filePath(index);
    QFileInfo info(path);

    if (info.isDir()) {
        setCurrentDirectory(path);
    }
}

void FilePage::onTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const QString path = m_model->filePath(index);
    QFileInfo info(path);

    if (info.isDir()) {
        setCurrentDirectory(path);
    } else if (info.isFile()) {
        emit fileActivated(path);
    }
}

void FilePage::onGoUpClicked()
{
    const QString path = currentPath();
    if (path.isEmpty()) {
        return;
    }

    QDir dir(path);
    if (dir.cdUp()) {
        setCurrentDirectory(dir.absolutePath());
    }
}