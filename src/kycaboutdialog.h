#ifndef KYCABOUTDIALOG_H
#define KYCABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class KYCAboutDialog;
}

class KYCAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KYCAboutDialog(QWidget *parent = nullptr);
    ~KYCAboutDialog();
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    int m_iHeight;
private:
    Ui::KYCAboutDialog *ui;
};

#endif // KYCABOUTDIALOG_H
