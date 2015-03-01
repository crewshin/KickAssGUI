#include "preferences.h"
#include "ui_preferences.h"
#include "gcconstants.h"


Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
    ui->setupUi(this);
    
    // Set default settings attributes.
    QCoreApplication::setOrganizationName("GeneCrucean");
    QCoreApplication::setOrganizationDomain("genecrucean.com");
    QCoreApplication::setApplicationName("KickAssGUI");
    QCoreApplication::setApplicationVersion(kVersionNumber);
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    
    this->readSettings();
}

Preferences::~Preferences()
{
    delete ui;
}


void Preferences::writeSettings()
{
//    qDebug() << "Preferences::writeSettings";
    
    // Save user prefs for later use.
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    
    settings.beginGroup("prefs");
    settings.setValue(kKickPath, ui->prefsKickPathLineEdit->text());
    settings.setValue(kShaderPath, ui->prefsShadersPathLineEdit->text());
    settings.setValue(kIgnoreLicHostCheckBox, ui->prefsIgnoreLicHostCheckBox->isChecked());
    settings.setValue(kLastUsedDirCheckBox, ui->prefsLastUsedDirCheckBox->isChecked());
    settings.endGroup();
    
    settings.sync();
}

void Preferences::readSettings()
{
//    qDebug() << "Preferences::readSettings";
    
    // Setup interface using user specified settings.
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "KickAssGUI", "KickAssGUI_PREFS");
    
    qDebug() << settings.value(kKickPath, "");
    
    settings.beginGroup("prefs");
    ui->prefsKickPathLineEdit->setText(settings.value(kKickPath, "").toString());
    ui->prefsShadersPathLineEdit->setText(settings.value(kShaderPath, "").toString());
    ui->prefsIgnoreLicHostCheckBox->setChecked(settings.value(kIgnoreLicHostCheckBox, false).toBool());
    ui->prefsLastUsedDirCheckBox->setChecked(settings.value(kLastUsedDirCheckBox, true).toBool());
    settings.endGroup();
}


void Preferences::on_prefsOkButton_clicked()
{
    qDebug() << "on_prefsOkButton_clicked";
    
    this->writeSettings();
    
}

void Preferences::on_prefsBrowseKickButton_clicked()
{
//    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.ass)"));
    QString path = QFileDialog::getOpenFileName(this, tr("Select the kick binary"), "", tr("Files (*)"));
    
    // Add path to GUI.
    ui->prefsKickPathLineEdit->setText(path);
    
    // Temporarily copy kick path to shader path.
    QFile file(path);
    QString tmp = file.fileName();
    tmp.remove(QRegularExpression("[kK]ick.?e?x?e?"));
    qDebug() << "##############: " << tmp;
    ui->prefsShadersPathLineEdit->setText(tmp);
}

void Preferences::on_prefsShadersBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,tr("Select the folder containing any shaders (.dylib, .so, .dll files)"), "", QFileDialog::ShowDirsOnly);
    
    // Add path to GUI.
    ui->prefsShadersPathLineEdit->setText(path);
}












