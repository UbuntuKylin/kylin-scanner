#include "about.h"
#include "ui_about.h"
#include "xatomhelper.h"

#include <QScreen>
#include <QDesktopServices>
#include <QStyle>
#include <QScrollBar>


KYCAboutDialog::KYCAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KYCAboutDialog)
{
    ui->setupUi(this);

    initWindow();

    initLayout();

    initConnect();
}

void KYCAboutDialog::paintEvent(QPaintEvent *event)
{
    QPalette pal = QApplication::style()->standardPalette();
    QPalette pp = QApplication::style()->standardPalette();
    QColor c;
    QString text =
        tr("Kylin Scanner is an interface-friendly scanner, which could be also used as one-click beautification, intelligent correction and text recognition tools.");

    c.setRed(231);
    c.setBlue(231);
    c.setGreen(231);
    if (c == pal.background().color()) {
        pal.setColor(QPalette::Background, QColor("#FFFFFF"));
        ui->labelSupport->setText(tr("Service & Support : ")
                                  + "<a href=\"mailto://support@kylinos.cn\">"
                                  + "support@kylinos.cn</a>");

        ui->textEdit->setText("<body style=\"background:#FFFFFF;\">"
                              + QString("<p style=\"color: %1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                              + text
                              + "</p></body>");
        setPalette(pal);
    } else {
        setPalette(pal);
        ui->labelSupport->setText(tr("Service & Support : ")
                                  + "<a href=\"mailto://support@kylinos.cn\">"
                                  + "support@kylinos.cn</a>");
        ui->textEdit->setText(QString("<body style=\"background:%1;\">") .arg(pal.background().color().name(
                                                                                  QColor::HexRgb))
                              + QString("<p style=\"color: %1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                              + text
                              + "</p></body>");
    }


    QString str = ui->textEdit->toPlainText();
    int w = ui->textEdit->fontMetrics().width(str, str.length());
    int h = ui->textEdit->fontMetrics().height();
    int row = w / ui->textEdit->width();

    // for += 2 因为计算行数方法可能不是很精准，所以额外多加一行。
    if (w % ui->textEdit->width()) row += 2;

    int he = (row * h);
    if (he < 200) {
        ui->textEdit->setFixedHeight(he);
        ui->textEdit->verticalScrollBar()->hide();
        ui->textEdit->setDisabled(true);
        m_iHeight = 336 + he;
    } else {
        ui->textEdit->setFixedHeight(200);
        ui->textEdit->verticalScrollBar()->show();
        ui->textEdit->setDisabled(false);
        m_iHeight = 336 + 200;
    }
    setFixedSize(420, m_iHeight);

    QFont f = ui->labelTitle->font();
    f.setPixelSize(14);
    ui->labelTitle->setFont(f);
    //f.setWeight(28);
    f.setPixelSize(18);
    ui->labelName->setFont(f);
    //f.setWeight(24);
    f.setPixelSize(14);
    ui->labelVersion->setFont(f);
    f.setFamily(font().family());
    ui->labelVersion->setPalette(pp);
    ui->textEdit->setFont(f);
    ui->textEdit->setPalette(pp);
    //ui->labelOfficalWebsite->setFont(f);
    //ui->labelOfficalWebsite->setPalette(pp);
    ui->labelSupport->setFont(f);
    ui->labelSupport->setPalette(pp);
    //ui->labelHotLine->setFont(f);
    //ui->labelHotLine->setPalette(pp);

    QDialog::paintEvent(event);
}

KYCAboutDialog::~KYCAboutDialog()
{
    delete ui;
}

void KYCAboutDialog::initWindow()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    KYCXAtomHelperObject::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("About"));
}

void KYCAboutDialog::initLayout()
{
    QFont f = ui->labelTitle->font();
    f.setPixelSize(14);
    ui->labelTitle->setText(tr("kylin-scanner"));
    f.setWeight(28);
    f.setPixelSize(18);
    ui->labelName->setFont(f);
    ui->labelName->setText(tr("kylin-scanner"));
    f.setWeight(24);
    f.setPixelSize(14);
    ui->labelVersion->setFont(f);

    QString appVersion = QCoreApplication::applicationVersion();
    ui->labelVersion->setText(tr("Version: " ) + appVersion);
    //ui->labelVersion->setAlignment(Qt::AlignCenter);
    f.setPixelSize(12);
    f.setPixelSize(14);
    ui->labelLogo->setPixmap(QIcon::fromTheme("kylin-scanner").pixmap(ui->labelLogo->size()));
    ui->labelIcon->setPixmap(QIcon::fromTheme("kylin-scanner").pixmap(96, 96));

    ui->labelTitle->setText(tr("kylin-scanner"));
    ui->btnClose->setIcon(QIcon::fromTheme("window-close-symbolic"));
    ui->btnClose->setProperty("isWindowButton", 0x2);
    ui->btnClose->setProperty("useIconHighlightEffect", 0x8);
    ui->btnClose->setIconSize(QSize(16, 16));
    ui->btnClose->installEventFilter(this);
    ui->btnClose->setFlat(true);
    ui->labelSupport->setContextMenuPolicy(Qt::NoContextMenu); // no right click menu

    QScreen *screen = QGuiApplication::primaryScreen ();
    QRect screenRect =  screen->availableGeometry();
    this->move(screenRect.width() / 2, screenRect.height() / 2);
    this->hide();
}

void KYCAboutDialog::initConnect()
{
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->labelSupport, &QLabel::linkActivated, [ = ](QString s) {
        QUrl url(s);
        QDesktopServices::openUrl(url);
    });
}
