#ifndef MMAINWINDOW_H
#define MMAINWINDOW_H

#include <QMainWindow>
#include "mGLWidget.h"
#include <QGridLayout>
#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QScrollArea>
#include <QStringListModel>

#include "mProgressBarWidget.h"

namespace Ui {
class mMainWindow;
}

class mMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit mMainWindow(QWidget *parent = 0, int wnd_width=960, int wnd_height=720, QString title="OpenGL Test");
    ~mMainWindow();
public slots:
    void fileAddSlot();
    void fileRemoveSlot();
    void fileRemoveAllSlot();
    void fileActivatedSlot(QModelIndex index);
    void videoToggleSlot();
    void videoResetSlot();
    void progressBarDisplaySlot(int cur_num, int total, bool is_reset);
    void progressBarSetSlot(float cur_ratio);

    void poseTemporaryStateSlot(bool is_pause);
signals:
    void signalOpenFile(QString & file_path);
private:
    Ui::mMainWindow *ui;
    int wnd_width;
    int wnd_height;

    /****** Just for store *****/
    QStringListModel * file_list_model;
    int cur_file_index;

    /******** Main widget ******/
    mProgressBarWidget * progress_bar;
    QWidget * grid_widget;
    QGridLayout * grid_layout;

    /***** Tool box widget *****/
    QGroupBox * tool_box;
    QGroupBox * file_box;
    QGroupBox * video_box;

    QGridLayout * tool_box_layout;
    QGridLayout * file_box_layout;
    QGridLayout * video_box_layout;

    QPushButton * tool_file_add_btn;
    QPushButton * tool_file_remove_btn;
    QPushButton * tool_file_removeall_btn;
    QListView * tool_file_listview;

    QPushButton * tool_video_toggle_btn;
    QPushButton * tool_video_reset_btn;
    /**************************/

    // OpenGL widgets
    mGLWidget * gl_widget;
};

#endif // MMAINWINDOW_H
