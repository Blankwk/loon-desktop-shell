#ifndef FILEPAGE_H
#define FILEPAGE_H

#include <QWidget>
#include <QModelIndex>

class QFileSystemModel;
class QTreeView;
class QTableView;
class QLabel;
class QPushButton;

/**
 * @brief 独立的文件管理器页面
 *
 * 职责：
 * 1. 显示目录树和文件列表
 * 2. 维护当前路径
 * 3. 处理目录进入 / 返回上一级
 * 4. 双击文件时发出 fileActivated 信号
 *
 * 不负责：
 * - 图片/音乐/视频如何打开
 * - 页面切换
 * - MainWindow 的状态栏更新
 */
class FilePage : public QWidget
{
    Q_OBJECT

public:
    explicit FilePage(QWidget *parent = nullptr);

    /// 返回当前目录路径
    QString currentPath() const;

signals:
    /// 双击文件时发出，交给 MainWindow 或外层做分发
    void fileActivated(const QString &filePath);

    /// 当前目录变化时发出，外层需要的话可以接
    void currentPathChanged(const QString &path);

private slots:
    void onTreeClicked(const QModelIndex &index);
    void onTableDoubleClicked(const QModelIndex &index);
    void onGoUpClicked();

private:
    void setupUi();
    void setupModel();
    void setupConnections();

    /// 切换当前目录
    void setCurrentDirectory(const QString &path);

private:
    QLabel *m_pathLabel = nullptr;
    QPushButton *m_upButton = nullptr;

    QTreeView *m_treeView = nullptr;
    QTableView *m_tableView = nullptr;

    QFileSystemModel *m_model = nullptr;
};

#endif // FILEPAGE_H