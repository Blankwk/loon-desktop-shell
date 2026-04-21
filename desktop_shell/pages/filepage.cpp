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
#include <QCheckBox>
#include <QLineEdit>
#include <QItemSelectionModel>
#include <QAbstractItemView>
#include <QDebug>

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

        // =========================
    // 顶部工具栏：上一级 + 刷新 + 路径输入框 + 显示隐藏文件
    // =========================
    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);

    // 返回上一级目录按钮
    m_upButton = new QPushButton("上一级", this);
    m_upButton->setFixedWidth(100);

    // 刷新当前目录按钮
    m_refreshButton = new QPushButton("刷新", this);
    m_refreshButton->setFixedWidth(80);

    // 当前路径输入框
    // 说明：
    // 1. 用于显示当前路径
    // 2. 用户也可以手动输入路径后按回车跳转
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setPlaceholderText("请输入目录路径，例如 /home/root");

    // 显示隐藏文件开关
    m_showHiddenCheckBox = new QCheckBox("显示隐藏文件", this);

    topLayout->addWidget(m_upButton);
    topLayout->addWidget(m_refreshButton);
    topLayout->addWidget(m_pathEdit, 1);
    topLayout->addWidget(m_showHiddenCheckBox);
    

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

    // =========================
    // 底部状态信息
    // =========================
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName("filePageStatusLabel");
    m_statusLabel->setMinimumHeight(32);
    m_statusLabel->setText("状态：准备就绪");

    rootLayout->addWidget(m_statusLabel);

    // 一些局部样式
        m_pathEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: rgba(15, 23, 42, 160);"
        "  border: 1px solid rgba(51, 65, 85, 180);"
        "  border-radius: 6px;"
        "  padding: 8px 12px;"
        "  color: white;"
        "}"
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

    m_refreshButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(30, 41, 59, 200);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 8px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(51, 65, 85, 220);"
        "}"
    );

    m_statusLabel->setStyleSheet(
        "background-color: rgba(15, 23, 42, 160);"
        "border-radius: 6px;"
        "padding: 6px 12px;"
        "color: white;"
    );

    m_showHiddenCheckBox->setStyleSheet(
        "QCheckBox {"
        "  color: white;"
        "  spacing: 8px;"   // 文字和勾选框之间的间距
        "}"
        "QCheckBox::indicator {"
        "  width: 18px;"    // 勾选框宽度
        "  height: 18px;"   // 勾选框高度
        "  border: 2px solid rgba(255, 255, 255, 220);"   // 未选中时边框更亮
        "  border-radius: 4px;"
        "  background-color: rgba(15, 23, 42, 180);"      // 深色底，方便看清白色勾
        "}"
        "QCheckBox::indicator:unchecked:hover {"
        "  border: 2px solid rgba(96, 165, 250, 255);"    // 鼠标悬停时更明显
        "  background-color: rgba(30, 41, 59, 220);"
        "}"
        "QCheckBox::indicator:checked {"
        "  border: 2px solid rgba(37, 99, 235, 255);"     // 选中时蓝色边框
        "  background-color: rgba(37, 99, 235, 255);"     // 选中时蓝色底
        "}"
        "QCheckBox::indicator:checked:hover {"
        "  border: 2px solid rgba(59, 130, 246, 255);"
        "  background-color: rgba(59, 130, 246, 255);"
        "}"
    );
}

void FilePage::setupModel()
{
    m_model = new QFileSystemModel(this);

    // 使用单独的辅助函数统一设置过滤规则
    // 这样后面显示隐藏文件时，只需要改一个地方
    applyModelFilter();

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
    
        // 路径输入框：按回车后尝试跳转目录
    connect(m_pathEdit, &QLineEdit::returnPressed,
            this, &FilePage::onPathReturnPressed);

    // 刷新按钮：刷新当前目录
    connect(m_refreshButton, &QPushButton::clicked,
            this, &FilePage::onRefreshClicked);

    // 显示隐藏文件复选框
    connect(m_showHiddenCheckBox, &QCheckBox::stateChanged,
            this, &FilePage::onShowHiddenChanged);

    // 文件列表当前选中项变化时，更新状态信息
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &FilePage::onCurrentSelectionChanged);
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

    //m_pathLabel->setText(QString("当前路径：%1").arg(path));

    // 路径切换后，同步更新路径输入框
    m_pathEdit->setText(path);

    // 同步刷新底部状态信息
    updateStatusInfo();

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

void FilePage::applyModelFilter()
{
    // 基础过滤规则：
    // 1. 显示目录
    // 2. 不显示 . 和 ..
    // 3. 显示普通文件
    QDir::Filters filters = QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files;

    // 如果勾选了“显示隐藏文件”，就把隐藏文件也放出来
    if (m_showHiddenCheckBox && m_showHiddenCheckBox->isChecked()) {
        filters |= QDir::Hidden;
    }

    m_model->setFilter(filters);
}

void FilePage::updateStatusInfo()
{
    const QString path = currentPath();
    if (path.isEmpty()) {
        m_statusLabel->setText("状态：当前路径无效");
        return;
    }

    // 当前目录下显示出来的项目数量
    const QModelIndex rootIndex = m_tableView->rootIndex();
    const int itemCount = m_model->rowCount(rootIndex);

    // 默认先显示“未选中任何项目”
    QString selectedInfo = "未选中项目";

    // 如果右侧列表当前有选中项，就把它的信息显示出来
    const QModelIndex currentIndex = m_tableView->currentIndex();
    if (currentIndex.isValid()) {
        const QString selectedPath = m_model->filePath(currentIndex);
        QFileInfo info(selectedPath);

        if (info.isDir()) {
            selectedInfo = QString("已选中：%1（目录）").arg(info.fileName());
        } else {
            selectedInfo = QString("已选中：%1（文件）").arg(info.fileName());
        }
    }

    m_statusLabel->setText(
        QString("当前目录：%1    项目数：%2    %3")
            .arg(path)
            .arg(itemCount)
            .arg(selectedInfo)
    );
}

void FilePage::onPathReturnPressed()
{
    // 读取用户输入的路径，并去掉首尾空白
    const QString path = m_pathEdit->text().trimmed();

    if (path.isEmpty()) {
        m_statusLabel->setText("状态：路径不能为空");
        return;
    }

    QFileInfo info(path);

    // 只允许跳转到存在的目录
    if (!info.exists() || !info.isDir()) {
        m_statusLabel->setText(QString("状态：路径无效或不是目录 -> %1").arg(path));
        return;
    }

    setCurrentDirectory(path);
}

void FilePage::onRefreshClicked()
{
    // 轻量刷新思路：
    // 重新定位当前目录，让视图重新同步一次
    const QString path = currentPath();
    if (path.isEmpty()) {
        m_statusLabel->setText("状态：当前路径为空，无法刷新");
        return;
    }

    // 重新应用一次过滤规则，避免切换隐藏文件后状态不同步
    applyModelFilter();

    // 重新定位到当前目录
    setCurrentDirectory(path);

    m_statusLabel->setText(QString("状态：已刷新 -> %1").arg(path));
}

void FilePage::onShowHiddenChanged(int state)
{
    Q_UNUSED(state);

    // 先保存当前目录，避免切换过滤规则后跳丢
    const QString path = currentPath();

    // 更新 model 的过滤规则
    applyModelFilter();

    // 恢复到原来的目录
    setCurrentDirectory(path);
}

void FilePage::onCurrentSelectionChanged(const QModelIndex &current,
                                         const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);

    // 选中项变化后，只需要刷新底部状态信息
    updateStatusInfo();
}