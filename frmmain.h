#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <string>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

typedef bool( *SUPPORTED_DEVICE_CHECK )( const mvIMPACT::acquire::Device* const );

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace Ui {
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private slots:
    void on_btnOpenFile_clicked();

private:
    Ui::frmMain *ui;

    cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true );
    QImage cvMatToQImage( const cv::Mat &inMat );
};

#endif // FRMMAIN_H
