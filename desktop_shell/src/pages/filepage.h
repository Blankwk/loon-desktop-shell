#ifndef FILEPAGE_H
#define FILEPAGE_H

#include <QWidget>
#include <QModelIndex>

class QFileSystemModel;
class QTreeView;
class QTableView;
class QLabel;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QComboBox;
class FileFilterProxyModel;

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

    /**
     * @brief 路径输入框按回车时触发
     *
     * 用户手动输入路径并回车后，尝试跳转到对应目录。
     */
    void onPathReturnPressed();

    /**
     * @brief 点击“刷新”按钮时触发
     *
     * 用于重新定位当前目录，达到轻量刷新视图的效果。
     */
    void onRefreshClicked();

    /**
     * @brief “显示隐藏文件”复选框状态变化时触发
     * @param state 复选框状态
     *
     * 根据复选框状态调整 QFileSystemModel 的过滤规则。
     */
    void onShowHiddenChanged(int state);

    /**
     * @brief 文件列表当前选中项变化时触发
     *
     * 主要用于刷新底部状态信息，让用户看到当前选中了什么。
     */
    void onCurrentSelectionChanged(const QModelIndex &current,
                                   const QModelIndex &previous);

        /**
     * @brief 类型过滤下拉框变化时触发
     * @param index 下拉框当前索引
     *
     * 用于切换右侧文件列表的过滤模式：
     * - 全部
     * - 仅目录
     * - 图片
     * - 音频
     * - 视频
     */
    void onTypeFilterChanged(int index);

    /**
     * @brief 排序方式下拉框变化时触发
     * @param index 下拉框当前索引
     *
     * 用于切换右侧文件列表的排序规则。
     */
    void onSortModeChanged(int index);

private:
    void setupUi();
    void setupModel();
    void setupConnections();

    /// 切换当前目录
    void setCurrentDirectory(const QString &path);

    /**
     * @brief 根据当前选项设置 model 的过滤规则
     *
     * 例如：
     * - 是否显示隐藏文件
     * - 是否显示目录和普通文件
     */
    void applyModelFilter();

    /**
     * @brief 更新底部状态信息
     *
     * 显示：
     * - 当前目录
     * - 当前目录项目数
     * - 当前选中项名称和类型
     */
    void updateStatusInfo();

    /**
     * @brief 根据当前排序下拉框设置右侧表格排序规则
     *
     * 支持名称 / 大小 / 修改时间的升序和降序。
     */
    void applySortMode();

private:
        /**
     * @brief 顶部路径输入框
     *
     * 用来显示当前路径，也支持用户手动输入路径并回车跳转。
     */
    QLineEdit *m_pathEdit = nullptr;

    /**
     * @brief 刷新按钮
     *
     * 点击后刷新当前目录显示。
     */
    QPushButton *m_refreshButton = nullptr;

    /**
     * @brief 显示隐藏文件的复选框
     */
    QCheckBox *m_showHiddenCheckBox = nullptr;

    /**
     * @brief 底部状态信息标签
     *
     * 用来显示当前目录、文件数量、选中项等信息。
     */
    QLabel *m_statusLabel = nullptr;
    
    QPushButton *m_upButton = nullptr;

    QTreeView *m_treeView = nullptr;
    QTableView *m_tableView = nullptr;

    QFileSystemModel *m_model = nullptr;

        /**
     * @brief 类型过滤下拉框
     *
     * 用于切换右侧文件列表过滤模式。
     */
    QComboBox *m_typeFilterCombo = nullptr;

    /**
     * @brief 排序方式下拉框
     *
     * 用于切换右侧文件列表排序方式。
     */
    QComboBox *m_sortCombo = nullptr;

    /**
     * @brief 右侧文件列表使用的代理模型
     *
     * 说明：
     * - 左侧目录树仍然直接使用 QFileSystemModel
     * - 右侧文件列表通过代理模型实现筛选和排序
     */
    FileFilterProxyModel *m_proxyModel = nullptr;
};

#endif // FILEPAGE_H