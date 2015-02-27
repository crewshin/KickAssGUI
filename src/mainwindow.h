#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "argumentbuilder.h"
#include "preferences.h"
#include <QDebug>
#include <QProcess>
#include <QFileDialog>
#include <QTextStream>
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileSystemModel>
#include "asssequencemanager.h"

////#include "dropzone.h"
//#include <QDropEvent>
//#include <QDragEnterEvent>
//#include <QDragLeaveEvent>
//#include <QDragMoveEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QProcess *kickProcess;
    QStringList assInputFileList;
    QStringList arguments;
    
    QString argResX;
    QString argResY;
    QString argAA;
    QString argDiffuseSamples;
    QString argGlossySamples;
    QString argMBShutterStart;
    QString argMBShutterEnd;
    QString argAAFilter;
    QString argBucketSize;
    QString argBucketScanning;
    QString argVerboseLevel;
    
    QString argRayTotalDepth;
    QString argRayDiffuseDepth;
    QString argRayGlossyDepth;
    QString argRayReflectionDepth;
    QString argRayRefractionDepth;
    
    QString argGammaOutput;
    QString argGammaTexture;
    QString argGammaLight;
    QString argGammaShader;
    
    QString argIgnoreTextureMaps;
    QString argIgnoreShaders;
    QString argIgnoreBackgroundShaders;
    QString argIgnoreAtmosphereShaders;
    QString argIgnoreLights;
    QString argIgnoreShadows;
    QString argIgnoreMeshSubdivision;
    QString argIgnoreDisplacement;
    QString argIgnoreMotionBlur;
    QString argIgnoreSSS;
    
    QString argMiscInteractiveNav;
    QString argMiscFlatShading;
    
    QString argDisableProgressive;
    QString argDisableDirectLighting;


//    DropArea *dropArea;
//    QLabel *abstractLabel;
//    QTableWidget *formatsTable;

//    QPushButton *clearButton;
//    QPushButton *quitButton;
//    QDialogButtonBox *buttonBox;


//    DropZone dropzone;
    AssSequenceManager *assManager;
    bool isSequence;
    int currentFrame;
    
    
    
private slots:
    bool launchKick();
    void on_mainTabRenderButton_clicked();
    void on_mainTabInputAssBrowseButton_clicked();
    void readSettings();
    void writeSettings();
    void on_actionPreferences_triggered();
    void buildArguments();
    void printProcessOutput();
    void on_mainTabSaveUIButton_clicked();
    void on_mainTabResetUIButton_clicked();
    void on_mainTabInputAssClearButton_clicked();
    void on_mainTabInputAssRemoveItemButton_clicked();
    void readOutFilePath();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

    // This is crashing...
    void on_mainTabKillRenderButton_clicked();
    
    void on_actionOpen_triggered();
    
    void on_mainTabInputAssListWidget_currentRowChanged(int currentRow);
    
    void on_mainTabInputAssListWidget_indexesMoved(const QModelIndexList &indexes);
    
protected:
//    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
//    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
//    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
//    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // MAINWINDOW_H
