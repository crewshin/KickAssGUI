#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <QFileDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT
    
public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();
    
private:
    Ui::Preferences *ui;
//    QSettings settings;
    
public slots:
    void writeSettings();
    void readSettings();
    
private slots:
    void on_prefsOkButton_clicked();
    void on_prefsBrowseKickButton_clicked();
    void on_prefsShadersBrowseButton_clicked();
};

#endif // PREFERENCES_H
