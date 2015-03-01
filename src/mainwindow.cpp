#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gcconstants.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtGlobal>
#include <QFileInfo>

// OS is OSX.
#ifdef Q_OS_MAC
#define osType "OSX"
#endif

// OS is LINUX.
#ifdef Q_OS_LINUX
#define osType "LINUX"
#endif

// OS is WIN.
#ifdef Q_OS_WIN32
#define osType "WIN"
#endif

bool fileExists(QString path) {
//    QFileInfo checkFile(path);
//    // check if file exists and if yes: Is it really a file and no directory?
//    if (checkFile.exists() && checkFile.isFile()) {
//        return true;
//    } else {
//        return false;
//    }
    
    QFileInfo file(path);
    if (file.isFile())
    {
        return true;
    }
    else
    {
        return false;
    }
    
    qDebug() << "HOME: " << QDir::homePath();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Check if user prefs have been set.
    QString prefsFile = QDir::homePath();
    prefsFile.append("/.config/KickAssGUI/KickAssGUI_PREFS.ini");
    qDebug() << "prefsFile: " << prefsFile;
    
    if (!fileExists(prefsFile))
    {
        qDebug() << "BOOM";
        Preferences myPrefs;
        myPrefs.setModal(true);
        myPrefs.exec();
    }

    
    // Read settings.
    this->readSettings();
            
    QString lic = qgetenv("solidangle_LICENSE");
    qDebug() << "solidangle_LICENSE: " << lic;
    
    // Set icon.
    MainWindow::setWindowIcon(QIcon(":/icon/images/KickAssGUI.png"));
    
    // Set style.
    QApplication::setStyle("Plastique");

    // Setup file path storage.
    assManager = new AssSequenceManager;
    assManager->seqLength = 1;

    // This should be false until a sequence is loaded.
    isSequence = false;
    
    // Set version number.
    ui->mainTabVersionLabel->setText(kVersion);
    
    // Init the kick process.
    kickProcess = new QProcess(this);
    
    // Connect signal to slot for stdout printing.
    connect(kickProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printProcessOutput()));
    
    // Connect signal to slow for process ended.
    connect(kickProcess, SIGNAL(finished(int , QProcess::ExitStatus )), this, SLOT(processFinished(int , QProcess::ExitStatus )));

//    dropArea = new DropArea;
//    connect(dropArea, SIGNAL(changed(const QMimeData*)), this, SLOT(updateFormatsTable(const QMimeData*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "processFinished";
    qDebug() << exitCode;
    qDebug() << exitStatus;
    
    if (exitCode == 0 && exitStatus == 0) {
        qDebug() << "Exited the process successfully.";
        
        if (ui->mainTabAniAssSequenceCheckBox->isChecked())
        {
            // Build arguments.
            this->buildArguments();

            if (currentFrame < assManager->seqLength)
            {
                qDebug() << "-------------------------- RENDERING!!!!!!";
                this->launchKick();
            }
            else
            {
                qDebug() << "RENDERING HAS FINISHED!";
                
                if (ui->mainNotifyOnSeqCompletionCheckBox->isChecked())
                {
                    QMessageBox alert;
                    alert.setText("The sequence has finished rendering!!!");
                    alert.setInformativeText("Woo hoo!!! Your render is com... your render i... your render is complet... your render is complete.");
                    alert.setStandardButtons(QMessageBox::Ok);
                    alert.setDefaultButton(QMessageBox::Ok);
                    alert.setMinimumSize(500, 200);
                    alert.exec();
                }
            }
        }
    }
    else if (exitCode == 0 && exitStatus == 1)
    {
        QMessageBox alert;
        alert.setText("Woah nellie, pump the brakes!");
        alert.setInformativeText("Rendering has been stopped.");
        alert.setStandardButtons(QMessageBox::Ok);
        alert.setDefaultButton(QMessageBox::Ok);
        alert.setMinimumSize(500, 200);
        alert.exec();
    }
}


bool MainWindow::launchKick()
{    
//    qDebug() << "arguments: " << arguments;
    
    QString args = "";
    for (int i = 1; i < arguments.count(); i++)
    {
        args.append(arguments[i]);
        args.append(" ");
    }
    
    qDebug() << "///////////////////// args ////////////////////////";
    qDebug() << args;
    qDebug() << "/////////////////// end args //////////////////////";
    
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    
    // Get path to kick.
    QString program = settings.value("prefs/kickPath", "").toString();
    
    qDebug() << "kickPath: " << program;
    
    // Start this sucker.
    kickProcess->start(program, arguments);
    
    currentFrame = currentFrame + 1;
    qDebug() << "currentFrame: " << currentFrame;
    
    return true;
}

void MainWindow::printProcessOutput()
{
//    qDebug() << "printProcessOutput";
    
    ui->mainTabRenderOutputPlainTextEdit->appendPlainText(kickProcess->readAllStandardOutput());
}


void MainWindow::on_mainTabInputAssBrowseButton_clicked()
{
    qDebug() << "on_mainTabAssBrowseButton_clicked";
    
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    
    // Get folder from last used ass files and set new launch default folder.
    settings.beginGroup("prefs");
    QString dir = "";
    if (settings.value(kLastUsedDirCheckBox).toBool())
    {
        dir = settings.value(kLastUsedDir, "").toString();
        qDebug() << "Last used dir: " << dir;
    }
    else
    {
        dir = "";
    }
    
    // Get the list of files selected in the file open dialog here. QStringList returned object.
    
    QFileDialog dialog(this, "Select .ass files to render", dir, "Files (*.ass)");
    assInputFileList = dialog.getOpenFileNames();
//    assInputFileList = QFileDialog::getOpenFileNames(this, "Select .ass files to render", dir, "Files (*.ass)");
    
    // This is temp storage for the QStringList. re.match modifies the assFileList object so we need to refer back to this object to store into the data store.
    QStringList assFileListTmp = assInputFileList;

    // Test for sequences with a .####. notation. 1 - 8 digits supported.
    bool isSingleFiles;
    QString seqName;
    for (int i = 0; i < assInputFileList.length(); i++)
    {
        QRegularExpression re("\\b(\\d{1,8})(?=\\.)");
//        QRegularExpression re("\\b(\\d{1,8})\\b");
        QRegularExpressionMatch match = re.match(assInputFileList[i]);
        
        if (match.hasMatch() && assInputFileList.length() > 1)
        {
            QString tmpNum = "[" + match.captured(0) + "]";
            QString tmpName = assInputFileList[i].replace(match.captured(0), tmpNum);
            seqName = tmpName;
            
            // Get sequence length of input files and save for later.
            qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
            qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~: " << assManager->seqLength;
            qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~: " << assInputFileList.length();
            
            if (assManager->seqLength < assInputFileList.length())
            {
                assManager->seqLength = assInputFileList.length();
                
                // Set sequence length label.
                QString tmpLength = QString::number(assInputFileList.length());
                ui->mainTabInputAssSequenceLengthLabel->setText(tmpLength);
                
                // Set sequence checkbox to true.
                ui->mainTabAniAssSequenceCheckBox->setChecked(true);
                ui->mainTabAniDisableOutputWindowCheckBox->setChecked(true);
            }
            
            isSequence = true;
            isSingleFiles = false;
        }
        else
        {
            // If it's not a sequence, use single file name.
            seqName = assInputFileList[i];
            isSingleFiles = true;
            
            // Set sequence length to 1 if no seq input.
            if (!isSequence)
            {
                assManager->seqLength = 1;
                
                // Set sequence label text.
                ui->mainTabInputAssSequenceLengthLabel->setText("1");
            }
        }
    }

    qDebug() << "}}} " << assManager->seqLength;
    
    if (isSingleFiles)
    {
        for (int i = 0; i < assInputFileList.length(); ++i) {
            // Insert the file list 
            seqName = assFileListTmp[i];
            QStringList tmp;
            tmp.append(assFileListTmp[i]);
            assManager->assSequence.insert(seqName, tmp);
        }

//        qDebug() << "+++: " << assManager->assSequence;
    
        // Set the key value (or file path if single file) to the QListWidget.
          ui->mainTabInputAssListWidget->addItems(assInputFileList);
    }
    else
    {
        // Insert the file list 
        assManager->assSequence.insert(seqName, assFileListTmp);
//        qDebug() << "+++: " << assManager->assSequence;
    
        // Set the key value (or file path if single file) to the QListWidget.
        ui->mainTabInputAssListWidget->addItem(seqName);
    }
    
    // Get path for user friendly'ness. This is the path that the file open dialog opens to next time it's launched.
    QFileInfo path(assInputFileList[0]);
//    qDebug() << "PATH: " << path.absoluteDir().absolutePath();
    
    settings.setValue(kLastUsedDir, path.absoluteDir().absolutePath());
    settings.endGroup();
    settings.sync();
    
    // Debug. Logs all arrays in datastore.
    QMapIterator<QString, QStringList> iter(assManager->assSequence);
    while(iter.hasNext())
    {
        iter.next();
        qDebug() << "---: " << iter.key() << iter.value();
    }
}

void MainWindow::on_mainTabInputAssClearButton_clicked()
{   
    ui->mainTabInputAssListWidget->clear();
    assManager->assSequence.clear();
    assManager->seqLength = 0;
    ui->mainTabAniAssSequenceCheckBox->setChecked(false);
    ui->mainTabAniDisableOutputWindowCheckBox->setChecked(false);
    ui->mainTabInputAssSequenceLengthLabel->setText("0");
    qDebug() << "assSequence: " << assManager->assSequence;
}

void MainWindow::on_mainTabInputAssRemoveItemButton_clicked()
{
    qDebug() << "on_mainTabInputAssRemoveItemButton_clicked";
    
    for (int i = 0; i < ui->mainTabInputAssListWidget->selectedItems().count(); ++i) 
    {
        QListWidgetItem *sel = ui->mainTabInputAssListWidget->selectedItems()[i];
        
        assManager->assSequence.remove(sel->text());
    }
    
    qDeleteAll(ui->mainTabInputAssListWidget->selectedItems());
}


void MainWindow::on_mainTabKillRenderButton_clicked()
{
    kickProcess->kill();
}


void MainWindow::on_mainTabRenderButton_clicked()
{
    qDebug() << "on_mainTabRenderButton_clicked";
    
    currentFrame = 0;
    
//    qDebug() << arguments.outputGamma << " - " << arguments.outputAA;
    
    // Check if output path exists.
//    this->readOutFilePath();
    
    if (ui->mainClearOnRenderCheckBox->isChecked())
    {
        ui->mainTabRenderOutputPlainTextEdit->clear();
    }
    
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    settings.beginGroup("prefs");
    QString kickPathLength = settings.value(kKickPath).toString();
//    qDebug() << "|||||||||||||||| " << kickPathLength.length();
    
    
    if (ui->mainTabInputAssListWidget->count() > 0)
    {
        if (kickPathLength.length() > 0)
        {
            qDebug() << "Kick path is set... carry on.";
            
            // Build arguments.
            this->buildArguments();
            
            qDebug() << "-------------------------- RENDERING!!!!!!";
            if (!this->launchKick())
            {
                qDebug() << "launchProcess() failed!!";
                
                QMessageBox alert;
                alert.setText("Something happened while trying to launch kick.");
                alert.setInformativeText("Please check your settings and try again.");
                alert.setStandardButtons(QMessageBox::Ok);
                alert.setDefaultButton(QMessageBox::Ok);
                alert.setMinimumSize(500, 200);
                alert.exec();
            }
        }
        else
        {
            qDebug() << "Kick path is NOT set.";
            
            QMessageBox alert;
            alert.setText("The path to Kick is not set yet.");
            alert.setInformativeText("Please set the path to the kick binary in the preferences.");
            alert.setStandardButtons(QMessageBox::Ok);
            alert.setDefaultButton(QMessageBox::Ok);
            alert.setMinimumSize(500, 200);
            alert.exec();
        }
    }
    else
    {
        QMessageBox alert;
        alert.setText("Umm... doiiii, you prooobably want to select some files to render.");
        alert.setInformativeText("Please select some .ass files and try again.");
        alert.setStandardButtons(QMessageBox::Ok);
        alert.setDefaultButton(QMessageBox::Ok);
        alert.setMinimumSize(500, 200);
        alert.exec();
    }
    
    settings.endGroup();
}

void MainWindow::on_mainTabSaveUIButton_clicked()
{
    qDebug() << "on_mainTabSaveUIButton_clicked";
    
    this->writeSettings();
}

void MainWindow::on_mainTabResetUIButton_clicked()
{
    qDebug() << "on_mainTabResetUIButton_clicked";
    
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_UI");
    
    settings.remove("renderOptions");
    settings.remove("raytracing");
    settings.remove("mainWindow");
    settings.remove("gamma");
    settings.remove("ignore");
    settings.remove("misc");
    settings.remove("disable");
    
    this->readSettings();
}


void MainWindow::readOutFilePath()
{
    qDebug() << "readOutFilePath";
    
//    QStringList filePaths = [];
    
//    for (int i = 0; i < ui->mainTabInputAssListWidget->items())
    
    QFile file("C:/Users/gene.crucean/Desktop/AssFiles/2/LightingAndCameras.ass");
    QStringList strings;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
//        qDebug() << "////////////////// STRINGS: " << file.readLine().startsWith(" filename") << " " << file.readLine();
        
//        while (!file.atEnd())
//        {
//            qDebug() << file.readLine().startsWith(" filename") << file.readLine();
//        }
        
        QTextStream in(&file);
        while (!in.atEnd()) 
        {
            strings += in.readLine().split("\n"); 
            
            qDebug() << "//////////////////// STRINGS: " << in.readLine().startsWith(" filename") << in.readLine();
            
//            qDebug() << "~~~~~~~~~~~~~~:" << tmp;
            
//            if (in.readLine().startsWith(" filename"))
//            {
////                strings += in.readLine().split("\n");
                
//                qDebug() << "//////////////////// BINGO: " << in.readLine();
                
//                return;
//            }
        }
    }
    
//    qDebug() << "///////////////////////////////////// STRINGS: " << strings;
    
    
//    if(!file.open(QIODevice::ReadOnly)) {
//        QMessageBox::information(0, "error", file.errorString());
//    }
    
//    QTextStream in(&file);
    
//    while(!in.atEnd()) 
//    {
//        QString line = in.readLine();    
//        QStringList fields = line.split(",");    
//        model->appendRow(fields);    
//    }
    
//    file.close();
}

//void dropEvent(QDropEvent *event)
//{
//    qDebug() << "dropEvent";
//}


//void DropSiteWindow::updateFormatsTable(const QMimeData *mimeData)
//{
//    formatsTable->setRowCount(0);
//    if (!mimeData)
//        return;

//    foreach (QString format, mimeData->formats()) {
//        QTableWidgetItem *formatItem = new QTableWidgetItem(format);
//        formatItem->setFlags(Qt::ItemIsEnabled);
//        formatItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);

//        QString text;
//        if (format == "text/plain") {
//            text = mimeData->text().simplified();
//        } else if (format == "text/html") {
//            text = mimeData->html().simplified();
//        } else if (format == "text/uri-list") {
//            QList<QUrl> urlList = mimeData->urls();
//            for (int i = 0; i < urlList.size() && i < 32; ++i)
//                text.append(urlList[i].toString() + " ");
//        } else {
//            QByteArray data = mimeData->data(format);
//            for (int i = 0; i < data.size() && i < 32; ++i) {
//                QString hex = QString("%1").arg(uchar(data[i]), 2, 16,
//                                                QChar('0'))
//                                           .toUpper();
//                text.append(hex + " ");
//            }
//        }

//        int row = formatsTable->rowCount();
//        formatsTable->insertRow(row);
//        formatsTable->setItem(row, 0, new QTableWidgetItem(format));
//        formatsTable->setItem(row, 1, new QTableWidgetItem(text));
//    }

//    formatsTable->resizeColumnToContents(0);
//}


void MainWindow::on_actionPreferences_triggered()
{
    qDebug() << "on_actionPreferences_triggered";
    
    Preferences myPrefs;
    myPrefs.setModal(true);
    myPrefs.exec();
}

void MainWindow::writeSettings()
{
    qDebug() << "AssViewerPreferences::writeSettings";

    // Save user prefs for later use.
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_UI");
    
    settings.beginGroup("mainWindow");
    settings.setValue(kClearOnRender, ui->mainClearOnRenderCheckBox->isChecked());
    settings.setValue(kNotifyOnSeqCompletion, ui->mainNotifyOnSeqCompletionCheckBox->isChecked());
//    settings.setValue(kDebugMode, ui->mainDebugModeCheckBox->isChecked());
    settings.endGroup();
    
    settings.beginGroup("renderOptions");
    settings.setValue(kResCheckBox, ui->mainTabRenderOptionsResCheckBox->isChecked());
    settings.setValue(kResX, ui->mainTabRenderOptionsResXLineEdit->text());
    settings.setValue(kResY, ui->mainTabRenderOptionsResYLineEdit->text());
    
    settings.setValue(kAACheckBox, ui->mainTabRenderOptionsAACheckBox->isChecked());
    settings.setValue(kAA, ui->mainTabRenderOptionsAALineEdit->text());
    
    settings.setValue(kDiffuseSampelsCheckBox, ui->mainTabRenderOptionsDiffuseSamplesCheckBox->isChecked());
    settings.setValue(kDiffuseSamples, ui->mainTabRenderOptionsDiffuseSamplesLineEdit->text());

    settings.setValue(kGlossySamplesCheckBox, ui->mainTabRenderOptionsGlossySamplesCheckBox->isChecked());
    settings.setValue(kGlossySamples, ui->mainTabRenderOptionsGlossySamplesLineEdit->text());

    settings.setValue(kMotionBlurCheckBox, ui->mainTabRenderOptionsMBShutterCheckBox->isChecked());
    settings.setValue(kMotionBlurStart, ui->mainTabRenderOptionsMBShutterStartLineEdit->text());
    settings.setValue(kMotionBlurEnd, ui->mainTabRenderOptionsMBShutterEndLineEdit->text());
    
    settings.setValue(kAAFilterCheckBox, ui->mainTabRenderOptionsAAFilterCheckBox->isChecked());
    settings.setValue(kAAFilterComboBox, ui->mainTabRenderOptionsAAFilterComboBox->currentIndex());
    settings.setValue(kAAFilterWidth, ui->mainTabRenderOptionsAAFilterWidthLineEdit->text());
    
    settings.setValue(kBucketSizeCheckBox, ui->mainTabRenderOptionsBucketSizeCheckBox->isChecked());
    settings.setValue(kBucketSize, ui->mainTabRenderOptionsBucketSizeLineEdit->text());

    settings.setValue(kBucketScanningCheckBox, ui->mainTabRenderOptionsBucketScanningCheckBox->isChecked());
    settings.setValue(kBucketScanning, ui->mainTabRenderOptionsBucketScanningComboBox->currentIndex());

    settings.setValue(kVerboseCheckBox, ui->mainTabRenderOptionsVerboseLevelCheckBox->isChecked());
    settings.setValue(kVerboseLevel, ui->mainTabRenderOptionsVerboseLevelComboBox->currentIndex());
    settings.endGroup();

    
    settings.beginGroup("raytracing");
    settings.setValue(kTotalDepthCheckBox, ui->mainTabRaytracingTotalDepthCheckBox->isChecked());
    settings.setValue(kTotalDepth, ui->mainTabRaytracingTotalDepthLineEdit->text());
    
    settings.setValue(kDiffuseDepthCheckBox, ui->mainTabRaytracingDiffuseDepthCheckBox->isChecked());
    settings.setValue(kDiffuseDepth, ui->mainTabRaytracingDiffuseDepthLineEdit->text());
    
    settings.setValue(kGlossyDepthCheckBox, ui->mainTabRaytracingGlossyDepthCheckBox->isChecked());
    settings.setValue(kGlossyDepth, ui->mainTabRaytracingGlossyDepthLineEdit->text());
    
    settings.setValue(kReflectionDepthCheckBox, ui->mainTabRaytracingReflectionDepthCheckBox->isChecked());
    settings.setValue(kReflectionDepth, ui->mainTabRaytracingReflectionDepthLineEdit->text());
    
    settings.setValue(kRefractionDepthCheckBox, ui->mainTabRaytracingRefractionDepthCheckBox->isChecked());
    settings.setValue(kRefractionDepth, ui->mainTabRaytracingRefractionDepthLineEdit->text());
    settings.endGroup();
    
    
    settings.beginGroup("gamma");
    settings.setValue(kOutputGammaCheckBox, ui->mainTabGammaOutputGammaCheckBox->isChecked());
    settings.setValue(kOutputGamma, ui->mainTabGammaOutputGammaLineEdit->text());
    
    settings.setValue(kTextureGammaCheckBox, ui->mainTabGammaTextureGammaCheckBox->isChecked());
    settings.setValue(kTextureGamma, ui->mainTabGammaTextureGammaLineEdit->text());
    
    settings.setValue(kLightGammaCheckBox, ui->mainTabGammaLightGammaCheckBox->isChecked());
    settings.setValue(kLightGamma, ui->mainTabGammaLightGammaLineEdit->text());
    
    settings.setValue(kShaderGammaCheckBox, ui->mainTabGammaShaderGammaCheckBox->isChecked());
    settings.setValue(kShaderGamma, ui->mainTabGammaShaderGammaLineEdit->text());
    settings.endGroup();
    
    settings.beginGroup("ignore");
    settings.setValue(kTextureMapsCheckBox, ui->mainTabIgnoreTextureMapsCheckBox->isChecked());
    settings.setValue(kShadersCheckBox, ui->mainTabIgnoreShadersCheckBox->isChecked());
    settings.setValue(kBackgroundShadersCheckBox, ui->mainTabIgnoreBackgroundShadersCheckBox->isChecked());
    settings.setValue(kAtmosphereShadersCheckBox, ui->mainTabIgnoreAtmosphereShadersCheckBox->isChecked());
    settings.setValue(kLightsCheckBox, ui->mainTabIgnoreLightsCheckBox->isChecked());
    settings.setValue(kShadowsCheckBox, ui->mainTabIgnoreShadowsCheckBox->isChecked());
    settings.setValue(kMeshSubdivisionCheckBox, ui->mainTabIgnoreMeshSubdivisionCheckBox->isChecked());
    settings.setValue(kDisplacementCheckBox, ui->mainTabIgnoreDisplacementCheckBox->isChecked());
    settings.setValue(kMotionBlurCheckBox, ui->mainTabIgnoreMotionBlurCheckBox->isChecked());
    settings.setValue(kSSSCheckBox, ui->mainTabIgnoreSSSCheckBox->isChecked());
    settings.endGroup();
    
    settings.beginGroup("misc");
    settings.setValue(kInteractiveNavCheckBox, ui->mainTabMiscInteractiveNavCheckBox->isChecked());
    settings.setValue(kInteractiveNav, ui->mainTabMiscInteractiveNavComboBox->currentIndex());
    
    settings.setValue(kFlatShadingCheckBox, ui->mainTabMiscFlatShadingCheckBox->isChecked());
    
    settings.setValue(kThreadsCheckBox, ui->mainTabMiscThreadsCheckBox->isChecked());
    settings.setValue(kThreads, ui->mainTabMiscThreadsLineEdit->text());
    
    settings.setValue(kCameraCheckBox, ui->mainTabMiscCameraCheckBox->isChecked());
    settings.setValue(kCamera, ui->mainTabMiscCameraLineEdit->text());
    settings.endGroup();
    
    settings.beginGroup("disable");
    settings.setValue(kProgressiveRenderCheckBox, ui->mainTabDisableProgressiveRenderCheckBox->isChecked());
    settings.setValue(kDirectLightingCheckBox, ui->mainTabDisableDirectLightingCheckBox->isChecked());
    settings.endGroup();
    
    settings.beginGroup("animation");
    settings.setValue(kSequence, ui->mainTabAniAssSequenceCheckBox->isChecked());
    settings.setValue(kDisableOutputWindow, ui->mainTabAniDisableOutputWindowCheckBox->isChecked());
    settings.endGroup();
    
    settings.sync();
}

void MainWindow::readSettings()
{
    qDebug() << "AssViewerPreferences::readSettings";

    // Save user prefs for later use.
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_UI");
    
    settings.beginGroup("mainWindow");
    ui->mainClearOnRenderCheckBox->setChecked(settings.value(kClearOnRender, true).toBool());
    ui->mainNotifyOnSeqCompletionCheckBox->setChecked(settings.value(kNotifyOnSeqCompletion, true).toBool());
//    ui->mainDebugModeCheckBox->setChecked(settings.value(kDebugMode, true).toBool());
    settings.endGroup();
    
    settings.beginGroup("renderOptions");
    ui->mainTabRenderOptionsResCheckBox->setChecked(settings.value(kResCheckBox, false).toBool());
    ui->mainTabRenderOptionsResXLineEdit->setText(settings.value(kResX, "960").toString());
    ui->mainTabRenderOptionsResYLineEdit->setText(settings.value(kResY, "540").toString());
    
    ui->mainTabRenderOptionsAACheckBox->setChecked(settings.value(kAACheckBox, false).toBool());
    ui->mainTabRenderOptionsAALineEdit->setText(settings.value(kAA, "3").toString());
    
    ui->mainTabRenderOptionsDiffuseSamplesCheckBox->setChecked(settings.value(kDiffuseSampelsCheckBox, false).toBool());
    ui->mainTabRenderOptionsDiffuseSamplesLineEdit->setText(settings.value(kDiffuseSamples, "2").toString());
    
    ui->mainTabRenderOptionsGlossySamplesCheckBox->setChecked(settings.value(kGlossySamplesCheckBox, false).toBool());
    ui->mainTabRenderOptionsGlossySamplesLineEdit->setText(settings.value(kGlossySamples, "2").toString());
    
    ui->mainTabRenderOptionsMBShutterCheckBox->setChecked(settings.value(kMotionBlurCheckBox, false).toBool());
    ui->mainTabRenderOptionsMBShutterStartLineEdit->setText(settings.value(kMotionBlurStart, "0.25").toString());
    ui->mainTabRenderOptionsMBShutterEndLineEdit->setText(settings.value(kMotionBlurEnd, "0.75").toString());
    
    ui->mainTabRenderOptionsAAFilterCheckBox->setChecked(settings.value(kAAFilterCheckBox, false).toBool());
    ui->mainTabRenderOptionsAAFilterComboBox->setCurrentIndex(settings.value(kAAFilterComboBox, 0).toInt());
    ui->mainTabRenderOptionsAAFilterWidthLineEdit->setText(settings.value(kAAFilterWidth, "3").toString());
    
    ui->mainTabRenderOptionsBucketSizeCheckBox->setChecked(settings.value(kBucketSizeCheckBox, false).toBool());
    ui->mainTabRenderOptionsBucketSizeLineEdit->setText(settings.value(kBucketSize, "64").toString());
    
    ui->mainTabRenderOptionsBucketScanningCheckBox->setChecked(settings.value(kBucketScanningCheckBox, false).toBool());
    ui->mainTabRenderOptionsBucketScanningComboBox->setCurrentIndex(settings.value(kBucketScanning, 0).toInt());
    
    ui->mainTabRenderOptionsVerboseLevelCheckBox->setChecked(settings.value(kVerboseCheckBox, true).toBool());
    ui->mainTabRenderOptionsVerboseLevelComboBox->setCurrentIndex(settings.value(kVerboseLevel, 4).toInt());
    settings.endGroup();
    
    settings.beginGroup("raytracing");
    ui->mainTabRaytracingTotalDepthCheckBox->setChecked(settings.value(kTotalDepthCheckBox, false).toBool());
    ui->mainTabRaytracingTotalDepthLineEdit->setText(settings.value(kTotalDepth, "10").toString());
    
    ui->mainTabRaytracingDiffuseDepthCheckBox->setChecked(settings.value(kDiffuseDepthCheckBox, false).toBool());
    ui->mainTabRaytracingDiffuseDepthLineEdit->setText(settings.value(kDiffuseDepth, "1").toString());
    
    ui->mainTabRaytracingGlossyDepthCheckBox->setChecked(settings.value(kGlossyDepthCheckBox, false).toBool());
    ui->mainTabRaytracingGlossyDepthLineEdit->setText(settings.value(kGlossyDepth, "2").toString());
    
    ui->mainTabRaytracingReflectionDepthCheckBox->setChecked(settings.value(kReflectionDepthCheckBox, false).toBool());
    ui->mainTabRaytracingReflectionDepthLineEdit->setText(settings.value(kReflectionDepth, "2").toString());
    
    ui->mainTabRaytracingRefractionDepthCheckBox->setChecked(settings.value(kRefractionDepthCheckBox, false).toBool());
    ui->mainTabRaytracingRefractionDepthLineEdit->setText(settings.value(kRefractionDepth, "1").toString());
    settings.endGroup();
    
    settings.beginGroup("gamma");
    ui->mainTabGammaOutputGammaCheckBox->setChecked(settings.value(kOutputGammaCheckBox, true).toBool());
    ui->mainTabGammaOutputGammaLineEdit->setText(settings.value(kOutputGamma, "2.2").toString());
    
    ui->mainTabGammaTextureGammaCheckBox->setChecked(settings.value(kTextureGammaCheckBox, false).toBool());
    ui->mainTabGammaTextureGammaLineEdit->setText(settings.value(kTextureGamma, "2.2").toString());
    
    ui->mainTabGammaLightGammaCheckBox->setChecked(settings.value(kLightGammaCheckBox, false).toBool());
    ui->mainTabGammaLightGammaLineEdit->setText(settings.value(kLightGamma, "2.2").toString());
    
    ui->mainTabGammaShaderGammaCheckBox->setChecked(settings.value(kShaderGammaCheckBox, false).toBool());
    ui->mainTabGammaShaderGammaLineEdit->setText(settings.value(kShaderGamma, "2.2").toString());
    settings.endGroup();
    
    settings.beginGroup("ignore");
    ui->mainTabIgnoreTextureMapsCheckBox->setChecked(settings.value(kTextureMapsCheckBox, false).toBool());
    ui->mainTabIgnoreShadersCheckBox->setChecked(settings.value(kShadersCheckBox, false).toBool());
    ui->mainTabIgnoreBackgroundShadersCheckBox->setChecked(settings.value(kBackgroundShadersCheckBox, false).toBool());
    ui->mainTabIgnoreAtmosphereShadersCheckBox->setChecked(settings.value(kAtmosphereShadersCheckBox, false).toBool());
    ui->mainTabIgnoreLightsCheckBox->setChecked(settings.value(kLightsCheckBox, false).toBool());
    ui->mainTabIgnoreShadowsCheckBox->setChecked(settings.value(kShadowsCheckBox, false).toBool());
    ui->mainTabIgnoreMeshSubdivisionCheckBox->setChecked(settings.value(kMeshSubdivisionCheckBox, false).toBool());
    ui->mainTabIgnoreDisplacementCheckBox->setChecked(settings.value(kDisplacementCheckBox, false).toBool());
    ui->mainTabIgnoreMotionBlurCheckBox->setChecked(settings.value(kMotionBlurCheckBox, false).toBool());
    ui->mainTabIgnoreSSSCheckBox->setChecked(settings.value(kSSSCheckBox, false).toBool());
    settings.endGroup();
    
    settings.beginGroup("misc");
    ui->mainTabMiscInteractiveNavCheckBox->setChecked(settings.value(kInteractiveNavCheckBox, false).toBool());
    ui->mainTabMiscInteractiveNavComboBox->setCurrentIndex(settings.value(kInteractiveNav, 0).toInt());
    
    ui->mainTabMiscFlatShadingCheckBox->setChecked(settings.value(kFlatShadingCheckBox, false).toBool());
    
    ui->mainTabMiscThreadsCheckBox->setChecked(settings.value(kThreadsCheckBox, false).toBool());
    ui->mainTabMiscThreadsLineEdit->setText(settings.value(kThreads, 4).toString());
    
    ui->mainTabMiscCameraCheckBox->setChecked(settings.value(kCameraCheckBox, false).toBool());
    ui->mainTabMiscCameraLineEdit->setText(settings.value(kCamera, "Camera1.SItoA.1000").toString());
    settings.endGroup();
    
    settings.beginGroup("disable");
    ui->mainTabDisableProgressiveRenderCheckBox->setChecked(settings.value(kProgressiveRenderCheckBox, false).toBool());
    ui->mainTabDisableDirectLightingCheckBox->setChecked(settings.value(kDirectLightingCheckBox, false).toBool());
    settings.endGroup();
    
    settings.beginGroup("animation");
    ui->mainTabAniAssSequenceCheckBox->setChecked(settings.value(kSequence, false).toBool());
    ui->mainTabAniDisableOutputWindowCheckBox->setChecked(settings.value(kDisableOutputWindow, false).toBool());
    settings.endGroup();
}


void MainWindow::buildArguments()
{
    qDebug() << "buildArguments";
    
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    
    arguments.clear();
    
    arguments.append("-nstdin");
    arguments.append("-l");
    arguments.append(settings.value("prefs/shaderPath").toString());
    
    ///////////////////////////////////////////////////////////////////////////////////////
    if (isSequence)
    {
        qDebug() << "This is a sequence of .ass files.";

        for (int a = 0; a < ui->mainTabInputAssListWidget->count(); a++)
        {
            if (currentFrame < assManager->seqLength)
            {
                arguments.append("-i");
                QString tmpKey = ui->mainTabInputAssListWidget->item(a)->text();
//                arguments.append(assManager->assSequence[tmpKey][currentFrame]);

                int inAssLength = assManager->assSequence[tmpKey].length();

                qDebug() << "===: " << tmpKey;
                qDebug() << "===: " << inAssLength;
                qDebug() << "===: " << currentFrame;

                if (currentFrame < inAssLength)
                {
                    arguments.append(assManager->assSequence[tmpKey][currentFrame]);
                }
                else
                {
                    arguments.append(assManager->assSequence[tmpKey][inAssLength - 1]);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < ui->mainTabInputAssListWidget->count(); i++)
        {
            arguments.append("-i");
            arguments.append(ui->mainTabInputAssListWidget->item(i)->text());
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    
    qDebug() << "arguments: " << arguments;
    
    if (ui->mainTabAniDisableOutputWindowCheckBox->isChecked())
    {
        arguments.append("-dw");
    }
    
    if (ui->mainTabRenderOptionsResCheckBox->isChecked())
    {
        arguments.append("-r");
        arguments.append(ui->mainTabRenderOptionsResXLineEdit->text());
        arguments.append(ui->mainTabRenderOptionsResYLineEdit->text());
    }
    
    if (ui->mainTabRenderOptionsAACheckBox->isChecked())
    {
        arguments.append("-as");
        arguments.append(ui->mainTabRenderOptionsAALineEdit->text());
    }

    if (ui->mainTabRenderOptionsDiffuseSamplesCheckBox->isChecked())
    {
        arguments.append("-ds");
        arguments.append(ui->mainTabRenderOptionsDiffuseSamplesLineEdit->text());
    }

    if (ui->mainTabRenderOptionsGlossySamplesCheckBox->isChecked())
    {
        arguments.append("-gs");
        arguments.append(ui->mainTabRenderOptionsGlossySamplesLineEdit->text());
    }
    
    if (ui->mainTabRenderOptionsMBShutterCheckBox->isChecked())
    {
        arguments.append("-sh");
        arguments.append(ui->mainTabRenderOptionsMBShutterStartLineEdit->text());
        arguments.append(ui->mainTabRenderOptionsMBShutterEndLineEdit->text());
    }
    
    if (ui->mainTabRenderOptionsAAFilterCheckBox->isChecked())
    {
        arguments.append("-af");
        arguments.append(ui->mainTabRenderOptionsAAFilterComboBox->currentText());
        arguments.append(ui->mainTabRenderOptionsAAFilterWidthLineEdit->text());
    }
    
    if (ui->mainTabRenderOptionsBucketScanningCheckBox->isChecked())
    {
        arguments.append("-bc");
        arguments.append(ui->mainTabRenderOptionsBucketScanningComboBox->currentText());
    }
    
    if (ui->mainTabRenderOptionsBucketSizeCheckBox->isChecked())
    {
        arguments.append("-bs");
        arguments.append(ui->mainTabRenderOptionsBucketSizeLineEdit->text());
    }
    
    if (ui->mainTabRenderOptionsVerboseLevelCheckBox->isChecked())
    {
        arguments.append("-v");
        arguments.append(ui->mainTabRenderOptionsVerboseLevelComboBox->currentText());
    }

    if (ui->mainTabRaytracingTotalDepthCheckBox->isChecked())
    {
        arguments.append("-td");
        arguments.append(ui->mainTabRaytracingTotalDepthLineEdit->text());
    }

    if (ui->mainTabRaytracingDiffuseDepthCheckBox->isChecked())
    {
        arguments.append("-dif");
        arguments.append(ui->mainTabRaytracingDiffuseDepthLineEdit->text());
    }

    if (ui->mainTabRaytracingGlossyDepthCheckBox->isChecked())
    {
        arguments.append("-glo");
        arguments.append(ui->mainTabRaytracingGlossyDepthLineEdit->text());
    }
    
    if (ui->mainTabRaytracingReflectionDepthCheckBox->isChecked())
    {
        arguments.append("-rfl");
        arguments.append(ui->mainTabRaytracingReflectionDepthLineEdit->text());
    }
    
    if (ui->mainTabRaytracingRefractionDepthCheckBox->isChecked())
    {
        arguments.append("-rfr");
        arguments.append(ui->mainTabRaytracingRefractionDepthLineEdit->text());
    }
    
    if (ui->mainTabGammaOutputGammaCheckBox->isChecked())
    {
        arguments.append("-g");
        arguments.append(ui->mainTabGammaOutputGammaLineEdit->text());
    }
    
    if (ui->mainTabGammaTextureGammaCheckBox->isChecked())
    {
        arguments.append("-tg");
        arguments.append(ui->mainTabGammaTextureGammaLineEdit->text());
    }
    
    if (ui->mainTabGammaLightGammaCheckBox->isChecked())
    {
        arguments.append("-lg");
        arguments.append(ui->mainTabGammaLightGammaLineEdit->text());
    }
    
    if (ui->mainTabGammaShaderGammaCheckBox->isChecked())
    {
        arguments.append("-sg");
        arguments.append(ui->mainTabGammaShaderGammaLineEdit->text());
    }
    
    if (ui->mainTabRaytracingTotalDepthCheckBox->isChecked())
    {
        arguments.append("-td");
        arguments.append(ui->mainTabRaytracingTotalDepthLineEdit->text());
    }
    
    if (ui->mainTabIgnoreTextureMapsCheckBox->isChecked())
    {
        arguments.append("-it");
    }
    
    if (ui->mainTabIgnoreShadersCheckBox->isChecked())
    {
        arguments.append("-is");
    }
    
    if (ui->mainTabIgnoreBackgroundShadersCheckBox->isChecked())
    {
        arguments.append("-ib");
    }
    
    if (ui->mainTabIgnoreAtmosphereShadersCheckBox->isChecked())
    {
        arguments.append("-ia");
    }
    
    if (ui->mainTabIgnoreLightsCheckBox->isChecked())
    {
        arguments.append("-il");
    }
    
    if (ui->mainTabDisableDirectLightingCheckBox->isChecked())
    {
        arguments.append("-idirect");
    }    
    
    if (ui->mainTabIgnoreShadowsCheckBox->isChecked())
    {
        arguments.append("-id");
    }
    
    if (ui->mainTabIgnoreMeshSubdivisionCheckBox->isChecked())
    {
        arguments.append("-isd");
    }
    
    if (ui->mainTabIgnoreDisplacementCheckBox->isChecked())
    {
        arguments.append("-idisp");
    }
    
    if (ui->mainTabIgnoreMotionBlurCheckBox->isChecked())
    {
        arguments.append("-imb");
    }
    
    if (ui->mainTabIgnoreSSSCheckBox->isChecked())
    {
        arguments.append("-isss");
    }
    
    if (ui->mainTabMiscInteractiveNavCheckBox->isChecked())
    {
        arguments.append("-interactive");
        if (ui->mainTabMiscInteractiveNavComboBox->currentText() == "Maya")
        {
            arguments.append("m");
        }
        else
        {
            arguments.append("q");
        }
    }
    
    if (ui->mainTabMiscFlatShadingCheckBox->isChecked())
    {
        arguments.append("-flat");
    }
    
    if (ui->mainTabMiscThreadsCheckBox->isChecked())
    {
        arguments.append("-t");
        arguments.append(ui->mainTabMiscThreadsLineEdit->text());
    }
    
    if (ui->mainTabMiscCameraCheckBox->isChecked())
    {
        arguments.append("-c");
        arguments.append(ui->mainTabMiscCameraLineEdit->text());
    }
    
    if (ui->mainTabDisableProgressiveRenderCheckBox->isChecked())
    {
        arguments.append("-dp");
    }
    
    
//    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
//    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    settings.beginGroup("prefs");
    bool tmpCheck = settings.value(kIgnoreLicHostCheckBox, false).toBool();
    settings.endGroup();
    
    if (tmpCheck) 
    {
        arguments.append("-sl");
    }
    
    qDebug() << "++++++++++ arguments: " << arguments;
}





void MainWindow::on_actionOpen_triggered()
{
    qDebug() << "on_actionOpen_triggered";
}

void MainWindow::on_mainTabInputAssListWidget_currentRowChanged(int currentRow)
{
    qDebug() << "currentRowChanged: " << currentRow;
}

void MainWindow::on_mainTabInputAssListWidget_indexesMoved(const QModelIndexList &indexes)
{
    qDebug() << "indexesMoved: " << indexes;
}



void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEnterEvent";
    
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "dragMoveEvent";
    
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    qDebug() << "dragLeaveEvent";
    
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent";
    
//    const QMimeData *mimeData = event->mimeData();
//    if (mimeData->hasUrls()) {
//       //Insert the URLs and set flags to Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
//    }
//    event->acceptProposedAction();
}

//void MainWindow::dragEnterEvent(QDragEnterEvent *event)
//{
//    qDebug() << "dragEnterEvent";
    
////    if (event->mimeData()->hasUrls())
////    {
////        event->acceptProposedAction();
////    } else {
////        QListWidget::dragEnterEvent(event);
////    }
//}
 
//void MainWindow::dragMoveEvent(QDragMoveEvent *event)
//{
//    qDebug() << "dragMoveEvent";
    
////    if (event->mimeData()->hasUrls())
////    {
////        event->acceptProposedAction();
////    } else {
////        QListWidget::dragMoveEvent(event);
////    }
//}
 
//void MainWindow::dropEvent(QDropEvent *event)
//{
//    qDebug() << "dropEvent";
    
////    if (event->mimeData()->hasUrls())
////    {
////        QList<QUrl> urls = event->mimeData()->urls();
////        if (!urls.isEmpty())
////        {
////            if (dropHintItem)
////            {
////                delete dropHintItem;
////                dropHintItem = 0;
////            }
////            QUrl url;
////            foreach (url,urls)
////            {
////                new QListWidgetItem(url.toLocalFile(),this);
////            }
////        }
////        event->acceptProposedAction();
////    }
////    QListWidget::dropEvent(event);
//}
