#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnOpenFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"));

    if (!filename.isEmpty()) {
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly)) {
            _payload = file.readAll();
            ui->txtPyload->document()->setPlainText(_payload);
        }
    }
}


void MainWindow::on_btnChainIt_clicked()
{
    QByteArray result = _signMgr.createSignature(QByteArray(), _payload);
    ui->txtResult->setText(result.toHex());
}


void MainWindow::on_txtPyload_textChanged()
{
    _payload = ui->txtPyload->document()->toRawText().toUtf8();
}


void MainWindow::on_btnCheckIsValid_clicked()
{
    QByteArray signature = QByteArray::fromHex(ui->txtResult->text().toUtf8());
    bool result = _signMgr.isValidSignature(_payload, signature);
    ui->lblIsValid->setText(result ? tr("Valid signature") : tr("Invalid signature"));
}

