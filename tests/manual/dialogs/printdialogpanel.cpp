/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_NO_PRINTER

#include "printdialogpanel.h"
#include "utils.h"

#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <QTextCodec>
#include <QTextDocument>
#include <QDir>
#include <QUrl>

extern qreal qt_pixelMultiplier(int resolution);

const FlagData testDocumentComboData[] =
{
    {"Page Layout Test (1 page)", PrintDialogPanel::PageLayoutTest},
    {"Rich Text Test (2 pages)", PrintDialogPanel::RichTextTest}
};

const FlagData printerModeComboData[] =
{
    {"ScreenResolution", QPrinter::ScreenResolution},
    {"PrinterResolution", QPrinter::PrinterResolution},
    {"HighResolution", QPrinter::HighResolution}
};

#if QT_VERSION < 0x050300
const FlagData pageSizeComboData[] =
{
    {"A4", QPrinter::A4},
    {"B5", QPrinter::B5},
    {"Letter", QPrinter::Letter},
    {"Legal", QPrinter::Legal},
    {"Executive", QPrinter::Executive},
    {"A0", QPrinter::A0},
    {"A1", QPrinter::A1},
    {"A2", QPrinter::A2},
    {"A3", QPrinter::A3},
    {"A5", QPrinter::A5},
    {"A6", QPrinter::A6},
    {"A7", QPrinter::A7},
    {"A8", QPrinter::A8},
    {"A9", QPrinter::A9},
    {"B0", QPrinter::B0},
    {"B1", QPrinter::B1},
    {"B10", QPrinter::B10},
    {"B2", QPrinter::B2},
    {"B3", QPrinter::B3},
    {"B4", QPrinter::B4},
    {"B6", QPrinter::B6},
    {"B7", QPrinter::B7},
    {"B8", QPrinter::B8},
    {"B9", QPrinter::B9},
    {"C5E", QPrinter::C5E},
    {"Comm10E", QPrinter::Comm10E},
    {"DLE", QPrinter::DLE},
    {"Folio", QPrinter::Folio},
    {"Ledger", QPrinter::Ledger},
    {"Tabloid", QPrinter::Tabloid},
    {"Custom", QPrinter::Custom}
};
#endif

const FlagData printRangeComboData[] =
{
    {"AllPages", QPrinter::AllPages},
    {"Selection", QPrinter::Selection},
    {"PageRange", QPrinter::PageRange},
    {"CurrentPage", QPrinter::CurrentPage}
};

const FlagData pageOrderComboData[] =
{
    {"FirstPageFirst", QPrinter::FirstPageFirst},
    {"LastPageFirst", QPrinter::LastPageFirst},
};

const FlagData duplexModeComboData[] =
{
    {"DuplexNone", QPrinter::DuplexNone},
    {"DuplexAuto", QPrinter::DuplexAuto},
    {"DuplexLongSide", QPrinter::DuplexLongSide},
    {"DuplexShortSide", QPrinter::DuplexShortSide},
};

const FlagData paperSourceComboData[] =
{
    {"OnlyOne", QPrinter::OnlyOne},
    {"Lower", QPrinter::Lower},
    {"Middle", QPrinter::Middle},
    {"Manual", QPrinter::Manual},
    {"Envelope", QPrinter::Envelope},
    {"EnvelopeManual", QPrinter::EnvelopeManual},
    {"Auto", QPrinter::Auto},
    {"Tractor", QPrinter::Tractor},
    {"SmallFormat", QPrinter::SmallFormat},
    {"LargeFormat", QPrinter::LargeFormat},
    {"LargeCapacity", QPrinter::LargeCapacity},
    {"Cassette", QPrinter::Cassette},
    {"FormSource", QPrinter::FormSource},
    {"DuplexLongSide", QPrinter::DuplexLongSide},
    {"DuplexShortSide", QPrinter::DuplexShortSide},
};

const FlagData colorModeComboData[] =
{
    {"GrayScale", QPrinter::GrayScale},
    {"Color", QPrinter::Color},
};

const FlagData unitsComboData[] =
{
    {"Millimeter", QPageLayout::Millimeter},
    {"Inch", QPageLayout::Inch},
    {"Point", QPageLayout::Point},
    {"Pica", QPageLayout::Pica},
    {"Didot", QPageLayout::Didot},
    {"Cicero", QPageLayout::Cicero},
};

const FlagData orientationComboData[] =
{
    {"Portrait", QPageLayout::Portrait},
    {"Landscape", QPageLayout::Landscape},
};

const FlagData layoutModeComboData[] =
{
    {"StandardMode", QPageLayout::StandardMode},
    {"FullPageMode", QPageLayout::FullPageMode},
};

const FlagData printDialogOptions[] =
{
    {"PrintToFile", QPrintDialog::PrintToFile},
    {"PrintSelection", QPrintDialog::PrintSelection},
    {"PrintPageRange", QPrintDialog::PrintPageRange},
    {"PrintShowPageSize", QPrintDialog::PrintShowPageSize},
    {"PrintCollateCopies", QPrintDialog::PrintCollateCopies},
    {"PrintCurrentPage", QPrintDialog::PrintCurrentPage}
};

QTextStream &operator<<(QTextStream &s, const QSizeF &size)
{
    s << size.width() << " x " << size.height();
    return s;
}

#if QT_VERSION >= 0x050300
QTextStream &operator<<(QTextStream &s, const QPageLayout::Unit &units)
{
    switch (units) {
    case QPageLayout::Millimeter:
        s << " mm";
        break;
    case QPageLayout::Point:
        s << " pt";
        break;
    case QPageLayout::Inch:
        s << " in";
        break;
    case QPageLayout::Pica:
        s << " pc";
        break;
    case QPageLayout::Didot:
        s << " DD";
        break;
    case QPageLayout::Cicero:
        s << " CC";
        break;
    }
    return s;
}

QTextStream &operator<<(QTextStream &s, const QMarginsF &margins)
{
    s << "l " << margins.left() << " t " << margins.top() << " r " << margins.right() << " b " << margins.bottom();
    return s;
}

QTextStream &operator<<(QTextStream &s, const QRect &rect)
{
    s << rect.width() << " x " << rect.height() << forcesign << ' ' << rect.x() << ' ' << rect.y() << noforcesign;
    return s;
}
#else
QTextStream &operator<<(QTextStream &s, const QPrinter::Margins &margins)
{
    s << "l " << margins.left << " t " << margins.top << " r " << margins.right << " b " << margins.bottom;
    return s;
}
#endif

QTextStream &operator<<(QTextStream &s, const QRectF &rect)
{
    s << rect.width() << " x " << rect.height() << forcesign << ' ' << rect.x() << ' ' << rect.y() << noforcesign;
    return s;
}

QTextStream &operator<<(QTextStream &s, const QPrinter &printer)
{
    s << "\n\nPrinter";
    if (printer.outputFormat() == QPrinter::NativeFormat) {
        s << "\nOutput Format    : NativeFormat";
        s << "\nPrinter Name     : " << printer.printerName();
    } else {
        s << "\nOutput Format    : PdfFormat";
        s << "\nOutput File Name : " << printer.outputFileName();
    }

    s << "\n\nPage Size";
    s << "\nPage Size ID     : " << printer.pageSize();
#if QT_VERSION >= 0x050300
    s << "\nName             : " << printer.pageLayout().pageSize().name();
    s << "\nPPD Key          : " << printer.pageLayout().pageSize().key();
    s << "\nDefinition Size  : " << printer.pageLayout().pageSize().definitionSize()
                                 << (printer.pageLayout().pageSize().definitionUnits() == QPageSize::Inch ? " in" : " mm");
    s << "\nPoint Size       : " << printer.pageLayout().pageSize().size(QPageSize::Point) << " pt";
#else
#if QT_VERSION >= 0x050000
    s << "\nPaper Name       : " << printer.paperName();
#endif
    s << "\nPaper Size       : " << printer.paperSize(QPrinter::Millimeter) << " mm";
#endif

    s << "\n\nPage Layout";
    s << "\nLayout Mode      : " << (printer.fullPage() ? "Full Page Mode" : "Standard Page Mode");
    s << "\nOrientation      : " << (printer.orientation() == QPrinter::Landscape ? "Landscape" : "Portrait");
#if QT_VERSION >= 0x050300
    s << "\nMinimum Margins  : " << printer.pageLayout().minimumMargins() << printer.pageLayout().units();
    s << "\nMargins          : " << printer.pageLayout().margins() << printer.pageLayout().units();
    s << "\nFull Rect        : " << printer.pageLayout().fullRect() << printer.pageLayout().units();
    s << "\n                   " << printer.pageLayout().fullRectPoints() << " pt";
    s << "\n                   " << printer.pageLayout().fullRectPixels(printer.resolution()) << " px";
    s << "\nPaint Rect       : " << printer.pageLayout().paintRect() << printer.pageLayout().units();
    s << "\n                   " << printer.pageLayout().paintRectPoints() << " pt";
    s << "\n                   " << printer.pageLayout().paintRectPixels(printer.resolution()) << " px";
#else
    s << "\nMargins          : " << printer.margins() << " mm";
    s << "\nPaper Rect       : " << printer.paperRect(QPrinter::Millimeter) << " mm";
    s << "\n                   " << printer.paperRect(QPrinter::Point) << " pt";
    s << "\n                   " << printer.paperRect(QPrinter::DevicePixel) << " px";
    s << "\nPage Rect        : " << printer.pageRect(QPrinter::Millimeter) << " mm";
    s << "\n                   " << printer.pageRect(QPrinter::Point) << " pt";
    s << "\n                   " << printer.pageRect(QPrinter::DevicePixel) << " px";
#endif

    s << "\n\nPaint Device Metrics";
    s << "\nWidth x Height (pt) : " << printer.width() << " x " << printer.height() << " pt";
    s << "\nWidth x Height (mm) : " << printer.widthMM() << " x " << printer.heightMM() << " mm";
    s << "\nLogical Resolution  : " << printer.logicalDpiX() << " x " << printer.logicalDpiY() << " dpi";
    s << "\nPhysical Resolution : " << printer.physicalDpiX() << " x " << printer.physicalDpiY() << " dpi";
    s << "\nDevice Pixel Ratio  : " << printer.devicePixelRatio();
    s << "\nNumber of Colors    : " << printer.colorCount();
    s << "\nDepth               : " << printer.depth();

    return s;
}

// Print a page with a rectangular frame, vertical / horizontal rulers in cm and printer info.

static void drawHorizCmRuler(QPainter &painter, int x1, int x2, int y)
{
    painter.drawLine(x1, y, x2, y);
    const int dpI = painter.device()->logicalDpiX();
    const int dpCm = qRound(double(dpI) / 2.54);
    const int h = dpCm / 2;
    const QFontMetrics fm(painter.font());
    for (int cm = 0, x = x1; x < x2; x += dpCm, ++cm) {
        painter.drawLine(x, y, x, y - h);
        if (cm) {
            const QString n = QString::number(cm);
            const QRect br = fm.boundingRect(n);
            painter.drawText(x - br.width() / 2, y - h - 10, n);
        }
    }
}

static void drawVertCmRuler(QPainter &painter, int x, int y1, int y2)
{
    painter.drawLine(x, y1, x, y2);
    const int dpI = painter.device()->logicalDpiY();
    const int dpCm = qRound(double(dpI) / 2.54);
    const int h = dpCm / 2;
    const QFontMetrics fm(painter.font());
    for (int cm = 0, y = y1; y < y2; y += dpCm, ++cm) {
        painter.drawLine(x, y, x + h, y);
        if (cm) {
            const QString n = QString::number(cm);
            const QRect br = fm.boundingRect(n);
            painter.drawText(x + h + 10, y + br.height() / 2, n);
        }
    }
}

static void printPageLayoutTest(QPrinter *printer)
{
    QPainter painter(printer);
    QRectF physicalRect = printer->paperRect();
    qreal left, top, right, bottom;
    printer->getPageMargins(&left, &top, &right, &bottom, QPrinter::DevicePixel);
    QRectF marginRect = physicalRect.adjusted(left, top, -right, -bottom);
    QPointF origin(0, 0);

    // Translate the QPrinter rects to the QPainter origin
    // If not full page mode the QPainter and QPrinter origins differ
    if (!printer->fullPage()) {
        origin = QPointF(-left, -top);
        physicalRect.translate(origin);
        marginRect.translate(origin);
    }

    QPen defaultPen;

    // Pen to draw border of physical paper
    QPen paperPen;
    paperPen.setWidth(2);

    // Pen and brush to draw minimum margin area
    QPen minMarginPen(Qt::DashDotLine);
    minMarginPen.setWidth(2);
    QBrush minMarginBrush(Qt::SolidPattern);
    minMarginBrush.setColor(Qt::darkGray);

    // Pen and brush to draw margin area
    QPen marginPen(Qt::DotLine);
    marginPen.setWidth(2);
    QBrush marginBrush(Qt::SolidPattern);
    marginBrush.setColor(Qt::lightGray);

    QFont font = painter.font();
    font.setFamily("Courier");
    font.setPointSize(10);

    // Format message.
    const int charHeight = QFontMetrics(font).boundingRect('X').height();
    QString msg;
    QTextStream str(&msg);
    str << "Qt "<< QT_VERSION_STR;
#if QT_VERSION >= 0x050000
    str << ' ' << QGuiApplication::platformName();
#endif
    str << ' ' << QDateTime::currentDateTime().toString();
    str << *printer;
    str << "\n\nFont                : " << font.family() << ' ' << font.pointSize();

    if (!painter.device()->logicalDpiY() || !painter.device()->logicalDpiX()) {
        qWarning() << Q_FUNC_INFO << "Bailing out due to invalid DPI: " << msg;
        return;
    }

    // First draw the margin areas
    painter.fillRect(QRect(origin.x(), origin.y(), left, physicalRect.height()), marginBrush);
    painter.fillRect(QRect(origin.x(), origin.y(), physicalRect.width(), top), marginBrush);
    painter.fillRect(QRect(origin.x() + physicalRect.width() - right, origin.y(), right, physicalRect.height()), marginBrush);
    painter.fillRect(QRect(origin.x(), origin.y() + physicalRect.height() - bottom, physicalRect.width(), bottom), marginBrush);
    painter.setPen(marginPen);
    painter.drawRect(marginRect);
    painter.setPen(defaultPen);

#if QT_VERSION >= 0x050300
    // Next draw the minimum margin area
    QMarginsF minMargins = printer->pageLayout().minimumMargins() / qt_pixelMultiplier(printer->resolution());
    QRectF minMarginRect = physicalRect.adjusted(minMargins.left(), minMargins.top(), -minMargins.right(), -minMargins.bottom());
    painter.fillRect(QRect(origin.x(), origin.y(), minMargins.left(), physicalRect.height()), minMarginBrush);
    painter.fillRect(QRect(origin.x(), origin.y(), physicalRect.width(), minMargins.top()), minMarginBrush);
    painter.fillRect(QRect(origin.x() + physicalRect.width() - minMargins.right(), origin.y(), minMargins.right(), physicalRect.height()), minMarginBrush);
    painter.fillRect(QRect(origin.x(), origin.y() + physicalRect.height() - minMargins.bottom(), physicalRect.width(), minMargins.bottom()), minMarginBrush);
    painter.setPen(minMarginPen);
    painter.drawRect(minMarginRect);
    painter.setPen(defaultPen);
#if 0
    qDebug() << "pageLayout   " << printer->pageLayout();
    qDebug() << "paperRect    " << printer->paperRect();
    qDebug() << "pageRect     " << printer->pageRect();
    qDebug() << "physicalRect " << physicalRect;
    qDebug() << "marginRect   " << marginRect;
    qDebug() << "minMarginRect" << minMarginRect;
    qDebug() << "margins      " << QMarginsF(left, top, right, bottom);
    qDebug() << "min margins  " << printer->pageLayout().minimumMargins();
    qDebug() << "";
#endif
#endif

    // Next draw the physical paper outline
    painter.drawRect(physicalRect);

    drawHorizCmRuler(painter, physicalRect.x(), physicalRect.right(), physicalRect.height() /2);
    drawVertCmRuler(painter, physicalRect.x() + physicalRect.width() / 2, physicalRect.top(), physicalRect.bottom());

    painter.setFont(font);
    QPointF textPoint = marginRect.topLeft() + QPoint(10, charHeight + 10);
    foreach (const QString &line, msg.split('\n')) {
        painter.drawText(textPoint, line);
        textPoint.ry() += (15 * charHeight) / 10;
    }

    painter.end();
}

PrintDialogPanel::PrintDialogPanel(QWidget *parent)
    : QWidget(parent),
      m_blockSignals(true),
      m_testDocument(PrintDialogPanel::PageLayoutTest),
      m_textDocument(0)
{
#if QT_VERSION < 0x050300
    m_printerLayout.setOutputFormat(QPrinter::PdfFormat);
#endif

    m_panel.setupUi(this);

    // Setup the QPrinter box
    populateCombo(m_panel.m_printerModeCombo, printerModeComboData, sizeof(printerModeComboData)/sizeof(FlagData));
    connect(m_panel.m_createButton, SIGNAL(clicked()), this, SLOT(createPrinter()));
    connect(m_panel.m_deleteButton, SIGNAL(clicked()), this, SLOT(deletePrinter()));

    // Setup the Test Document box
    populateCombo(m_panel.m_testDocumentCombo, testDocumentComboData, sizeof(testDocumentComboData)/sizeof(FlagData));
    connect(m_panel.m_testDocumentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(testDocumentChanged()));
    testDocumentChanged();

    // Setup the Page Layout box
    populateCombo(m_panel.m_unitsCombo, unitsComboData, sizeof(unitsComboData)/sizeof(FlagData));
    connect(m_panel.m_unitsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(unitsChanged()));
#if QT_VERSION >= 0x050300
    for (int i = QPageSize::A4; i < QPageSize::LastPageSize; ++i) {
        QPageSize::PageSizeId id = QPageSize::PageSizeId(i);
        m_panel.m_pageSizeCombo->addItem(QPageSize::name(id), QVariant(id));
    }
#else
    populateCombo(m_panel.m_pageSizeCombo, pageSizeComboData, sizeof(pageSizeComboData)/sizeof(FlagData));
#endif
    connect(m_panel.m_pageSizeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(pageSizeChanged()));
    connect(m_panel.m_pageWidth, SIGNAL(valueChanged(double)), this, SLOT(pageDimensionsChanged()));
    connect(m_panel.m_pageHeight, SIGNAL(valueChanged(double)), this, SLOT(pageDimensionsChanged()));
    populateCombo(m_panel.m_orientationCombo, orientationComboData, sizeof(orientationComboData)/sizeof(FlagData));
    connect(m_panel.m_orientationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationChanged()));
    connect(m_panel.m_leftMargin, SIGNAL(valueChanged(double)), this, SLOT(marginsChanged()));
    connect(m_panel.m_topMargin, SIGNAL(valueChanged(double)), this, SLOT(marginsChanged()));
    connect(m_panel.m_rightMargin, SIGNAL(valueChanged(double)), this, SLOT(marginsChanged()));
    connect(m_panel.m_bottomMargin, SIGNAL(valueChanged(double)), this, SLOT(marginsChanged()));
    populateCombo(m_panel.m_layoutModeCombo, layoutModeComboData, sizeof(layoutModeComboData)/sizeof(FlagData));
    connect(m_panel.m_layoutModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(layoutModeChanged()));

    // Setup the Print Job box
    m_panel.m_printerCombo->addItem(tr("Print to PDF"), QVariant("PdfFormat"));
#if QT_VERSION >= 0x050300
    foreach (const QString &name, QPrinterInfo::availablePrinterNames())
        m_panel.m_printerCombo->addItem(name, QVariant(name));
#else
    foreach (const QPrinterInfo &printer, QPrinterInfo::availablePrinters())
        m_panel.m_printerCombo->addItem(printer.printerName(), QVariant(printer.printerName()));
#endif
    connect(m_panel.m_printerCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(printerChanged()));
    populateCombo(m_panel.m_printRangeCombo, printRangeComboData, sizeof(printRangeComboData)/sizeof(FlagData));
    populateCombo(m_panel.m_pageOrderCombo, pageOrderComboData, sizeof(pageOrderComboData)/sizeof(FlagData));
    populateCombo(m_panel.m_duplexModeCombo, duplexModeComboData, sizeof(duplexModeComboData)/sizeof(FlagData));
    populateCombo(m_panel.m_paperSourceCombo, paperSourceComboData, sizeof(paperSourceComboData)/sizeof(FlagData));
    populateCombo(m_panel.m_colorModeCombo, colorModeComboData, sizeof(colorModeComboData)/sizeof(FlagData));

    // Setup the Dialogs box
    m_panel.m_dialogOptionsGroupBox->populateOptions(printDialogOptions, sizeof(printDialogOptions) / sizeof(FlagData));
    QPrintDialog dialog;
    m_panel.m_dialogOptionsGroupBox->setValue(dialog.options());
    connect(m_panel.m_printButton, SIGNAL(clicked()), this, SLOT(showPrintDialog()));
    connect(m_panel.m_printPreviewButton, SIGNAL(clicked()), this, SLOT(showPreviewDialog()));
    connect(m_panel.m_pageSetupButton, SIGNAL(clicked()), this, SLOT(showPageSetupDialog()));
    connect(m_panel.m_directPrintButton, SIGNAL(clicked()), this, SLOT(directPrint()));

    enablePanels();
    m_blockSignals = false;
}

PrintDialogPanel::~PrintDialogPanel()
{
}

void PrintDialogPanel::enablePanels()
{
    const bool exists = !m_printer.isNull();
    m_panel.m_createButton->setEnabled(!exists);
    m_panel.m_printerModeCombo->setEnabled(!exists);
    m_panel.m_deleteButton->setEnabled(exists);
    m_panel.m_documentGroupBox->setEnabled(exists);
    m_panel.m_pageLayoutGroupBox->setEnabled(exists);
    m_panel.m_printJobGroupBox->setEnabled(exists);
    m_panel.m_dialogsGroupBox->setEnabled(exists);
}

void PrintDialogPanel::createPrinter()
{
    const QPrinter::PrinterMode mode = comboBoxValue<QPrinter::PrinterMode>(m_panel.m_printerModeCombo);
    m_printer.reset(new QPrinter(mode)); // Can set only once.
    retrieveSettings(m_printer.data());
    enablePanels();
    testDocumentChanged();
}

void PrintDialogPanel::deletePrinter()
{
    m_printer.reset();
    enablePanels();
}

QSizeF PrintDialogPanel::customPageSize() const
{
    return QSizeF(m_panel.m_pageWidth->value(), m_panel.m_pageHeight->value());
}

// Apply the settings to the QPrinter
void PrintDialogPanel::applySettings(QPrinter *printer) const
{
    QString printerName = m_panel.m_printerCombo->itemData(m_panel.m_printerCombo->currentIndex()).toString();
    if (printerName == "PdfFormat")
        printer->setOutputFileName(m_panel.m_fileName->text());
    else
        printer->setPrinterName(printerName);
    printer->setPrintRange(comboBoxValue<QPrinter::PrintRange>(m_panel.m_printRangeCombo));
    printer->setFromTo(m_panel.m_fromPage->value(), m_panel.m_toPage->value());
    printer->setPageOrder(comboBoxValue<QPrinter::PageOrder>(m_panel.m_pageOrderCombo));
    printer->setCopyCount(m_panel.m_copyCount->value());
    printer->setCollateCopies(m_panel.m_collateCopies->isChecked());
    printer->setDuplex(comboBoxValue<QPrinter::DuplexMode>(m_panel.m_duplexModeCombo));
    printer->setPaperSource(comboBoxValue<QPrinter::PaperSource>(m_panel.m_paperSourceCombo));
    printer->setColorMode(comboBoxValue<QPrinter::ColorMode>(m_panel.m_colorModeCombo));
    printer->setResolution(m_panel.m_resolution->value());
    printer->setDocName(m_panel.m_documentName->text());
    printer->setCreator(m_panel.m_documentCreator->text());

#if QT_VERSION >= 0x050300
    printer->setPageLayout(m_pageLayout);
#else
    if (m_printerLayout.pageSize() == QPrinter::Custom)
        printer->setPaperSize(customPageSize(), m_units);
    else
        printer->setPageSize(m_printerLayout.pageSize());
    printer->setOrientation(m_printerLayout.orientation());
    printer->setFullPage(m_printerLayout.fullPage());
    double left, top, right, bottom;
    m_printerLayout.getPageMargins(&left, &top, &right, &bottom, m_units);
    printer->setPageMargins(left, top, right, bottom, m_units);
#endif
}

// Retrieve the settings from the QPrinter
void PrintDialogPanel::retrieveSettings(const QPrinter *printer)
{
    if (printer->outputFormat() == QPrinter::NativeFormat) {
        m_panel.m_printerCombo->setCurrentIndex(m_panel.m_printerCombo->findData(QVariant(printer->printerName())));
        m_panel.m_fileName->setEnabled(false);
    } else {
        m_panel.m_printerCombo->setCurrentIndex(m_panel.m_printerCombo->findData(QVariant("PdfFormat")));
        m_panel.m_fileName->setEnabled(true);
    }
    m_panel.m_fileName->setText(printer->outputFileName());
    setComboBoxValue(m_panel.m_printRangeCombo, printer->printRange());
    m_panel.m_fromPage->setValue(printer->fromPage());
    m_panel.m_toPage->setValue(printer->toPage());
    setComboBoxValue(m_panel.m_pageOrderCombo, printer->pageOrder());
    m_panel.m_copyCount->setValue(printer->copyCount());
    m_panel.m_collateCopies->setChecked(printer->collateCopies());
    setComboBoxValue(m_panel.m_duplexModeCombo, printer->duplex());
    setComboBoxValue(m_panel.m_paperSourceCombo, printer->paperSource());
    setComboBoxValue(m_panel.m_colorModeCombo, printer->colorMode());
    m_panel.m_resolution->setValue(printer->resolution());
    m_panel.m_documentName->setText(printer->docName());
    m_panel.m_documentCreator->setText(printer->creator());

#if QT_VERSION >= 0x050300
    m_pageLayout = printer->pageLayout();
#else
    if (printer->pageSize() == QPrinter::Custom)
        m_printerLayout.setPaperSize(customPageSize(), m_units);
    else
        m_printerLayout.setPageSize(printer->pageSize());
    m_printerLayout.setOrientation(printer->orientation());
    m_printerLayout.setFullPage(printer->fullPage());
    double left, top, right, bottom;
    printer->getPageMargins(&left, &top, &right, &bottom, m_units);
    m_printerLayout.setPageMargins(left, top, right, bottom, m_units);
#endif
    updatePageLayoutWidgets();
}

void PrintDialogPanel::updatePageLayoutWidgets()
{
    m_blockSignals = true;
#if QT_VERSION >= 0x050300
    setComboBoxValue(m_panel.m_unitsCombo, m_pageLayout.units());
    setComboBoxValue(m_panel.m_pageSizeCombo, m_pageLayout.pageSize().id());
    QSizeF sizef = m_pageLayout.pageSize().size(QPageSize::Unit(m_pageLayout.units()));
    bool custom = (m_pageLayout.pageSize().id() == QPageSize::Custom);
    setComboBoxValue(m_panel.m_orientationCombo, m_pageLayout.orientation());
    m_panel.m_leftMargin->setValue(m_pageLayout.margins().left());
    m_panel.m_topMargin->setValue(m_pageLayout.margins().top());
    m_panel.m_rightMargin->setValue(m_pageLayout.margins().right());
    m_panel.m_bottomMargin->setValue(m_pageLayout.margins().bottom());
    setComboBoxValue(m_panel.m_layoutModeCombo, m_pageLayout.mode());
    QRectF rectf = m_pageLayout.paintRect();
#else
    setComboBoxValue(m_panel.m_unitsCombo, m_units);
    setComboBoxValue(m_panel.m_pageSizeCombo, m_printerLayout.pageSize());
    QSizeF sizef = m_printerLayout.paperSize(m_units);
    bool custom = (m_printerLayout.pageSize() == QPrinter::Custom);
    setComboBoxValue(m_panel.m_orientationCombo, m_printerLayout.orientation());
    double left, top, right, bottom;
    m_printerLayout.getPageMargins(&left, &top, &right, &bottom, m_units);
    m_panel.m_leftMargin->setValue(left);
    m_panel.m_topMargin->setValue(top);
    m_panel.m_rightMargin->setValue(right);
    m_panel.m_bottomMargin->setValue(bottom);
    if (m_printerLayout.fullPage())
        setComboBoxValue(m_panel.m_layoutModeCombo, QPageLayout::FullPageMode);
    else
        setComboBoxValue(m_panel.m_layoutModeCombo, QPageLayout::StandardMode);
    QRectF rectf = m_printerLayout.pageRect(m_units);
#endif
    m_panel.m_pageWidth->setValue(sizef.width());
    m_panel.m_pageHeight->setValue(sizef.height());
    m_panel.m_pageWidth->setEnabled(custom);
    m_panel.m_pageHeight->setEnabled(custom);
    m_panel.m_rectX->setValue(rectf.x());
    m_panel.m_rectY->setValue(rectf.y());
    m_panel.m_rectWidth->setValue(rectf.width());
    m_panel.m_rectHeight->setValue(rectf.height());
    QString suffix;
    switch (comboBoxValue<QPageLayout::Unit>(m_panel.m_unitsCombo)) {
    case QPageLayout::Millimeter:
        suffix = tr(" mm");
        break;
    case QPageLayout::Point:
        suffix = tr(" pt");
        break;
    case QPageLayout::Inch:
        suffix = tr(" in");
        break;
    case QPageLayout::Pica:
        suffix = tr(" pc");
        break;
    case QPageLayout::Didot:
        suffix = tr(" DD");
        break;
    case QPageLayout::Cicero:
        suffix = tr(" CC");
        break;
    }
    m_panel.m_pageWidth->setSuffix(suffix);
    m_panel.m_pageHeight->setSuffix(suffix);
    m_panel.m_leftMargin->setSuffix(suffix);
    m_panel.m_topMargin->setSuffix(suffix);
    m_panel.m_rightMargin->setSuffix(suffix);
    m_panel.m_bottomMargin->setSuffix(suffix);
    m_panel.m_rectX->setSuffix(suffix);
    m_panel.m_rectY->setSuffix(suffix);
    m_panel.m_rectWidth->setSuffix(suffix);
    m_panel.m_rectHeight->setSuffix(suffix);
    m_blockSignals = false;
}

void PrintDialogPanel::testDocumentChanged()
{
    m_testDocument = comboBoxValue<PrintDialogPanel::TestDocument>(m_panel.m_testDocumentCombo);
    switch (m_testDocument) {
    case PrintDialogPanel::PageLayoutTest:
        m_panel.m_documentName->setText("PageLayoutTest");
        break;
    case PrintDialogPanel::RichTextTest:
        initRichTextDocument();
        break;
    }
    m_panel.m_documentCreator->setText(tr("Qt Project manual testing"));
    m_panel.m_fileName->setText(QDir::homePath() + QDir::separator()
                                + m_panel.m_documentName->text() + ".pdf");
}

void PrintDialogPanel::unitsChanged()
{
    if (m_blockSignals)
        return;
#if QT_VERSION >= 0x050300
    m_pageLayout.setUnits(comboBoxValue<QPageLayout::Unit>(m_panel.m_unitsCombo));
#else
    m_units = comboBoxValue<QPrinter::Unit>(m_panel.m_unitsCombo);
#endif
    updatePageLayoutWidgets();
}

void PrintDialogPanel::pageSizeChanged()
{
    if (m_blockSignals)
        return;
#if QT_VERSION >= 0x050300
    const QPageSize::PageSizeId pageSizeId = comboBoxValue<QPageSize::PageSizeId>(m_panel.m_pageSizeCombo);
    QPageSize pageSize;
    if (pageSizeId == QPageSize::Custom)
        pageSize = QPageSize(QSizeF(200, 200), QPageSize::Unit(m_pageLayout.units()));
    else
        pageSize = QPageSize(pageSizeId);
    m_pageLayout.setPageSize(pageSize);
#else
    const QPrinter::PageSize pageSize = comboBoxValue<QPrinter::PageSize>(m_panel.m_pageSizeCombo);
    if (pageSize == QPrinter::Custom)
        m_printerLayout.setPaperSize(QSizeF(200, 200), m_units);
    else
        m_printerLayout.setPageSize(pageSize);
#endif
    updatePageLayoutWidgets();
}

void PrintDialogPanel::pageDimensionsChanged()
{
    if (m_blockSignals)
        return;
#if QT_VERSION >= 0x050300
    m_pageLayout.setPageSize(QPageSize(customPageSize(), QPageSize::Unit(m_pageLayout.units())));
#else
    m_printerLayout.setPaperSize(customPageSize(), m_units);
#endif
    updatePageLayoutWidgets();
}

void PrintDialogPanel::orientationChanged()
{
    if (m_blockSignals)
        return;
#if QT_VERSION >= 0x050300
    m_pageLayout.setOrientation(comboBoxValue<QPageLayout::Orientation>(m_panel.m_orientationCombo));
#else
    m_printerLayout.setOrientation(comboBoxValue<QPrinter::Orientation>(m_panel.m_orientationCombo));
#endif
    updatePageLayoutWidgets();
}

void PrintDialogPanel::marginsChanged()
{
    if (m_blockSignals)
        return;
#if QT_VERSION >= 0x050300
    m_pageLayout.setMargins(QMarginsF(m_panel.m_leftMargin->value(), m_panel.m_topMargin->value(),
                                      m_panel.m_rightMargin->value(), m_panel.m_bottomMargin->value()));
#else
    m_printerLayout.setPageMargins(m_panel.m_leftMargin->value(), m_panel.m_topMargin->value(),
                                   m_panel.m_rightMargin->value(), m_panel.m_bottomMargin->value(),
                                   m_units);
#endif
    updatePageLayoutWidgets();
}

void PrintDialogPanel::layoutModeChanged()
{
    if (m_blockSignals)
        return;
#if QT_VERSION >= 0x050300
    m_pageLayout.setMode(comboBoxValue<QPageLayout::Mode>(m_panel.m_layoutModeCombo));
#else
    bool fullPage = (comboBoxValue<QPageLayout::Mode>(m_panel.m_layoutModeCombo) == QPageLayout::FullPageMode);
    m_printerLayout.setFullPage(fullPage);
#endif
    updatePageLayoutWidgets();
}

void PrintDialogPanel::printerChanged()
{
    bool isPdf = (m_panel.m_printerCombo->itemData(m_panel.m_printerCombo->currentIndex()).toString() == "PdfFormat");
    m_panel.m_fileName->setEnabled(isPdf);
}

void PrintDialogPanel::showPrintDialog()
{
    applySettings(m_printer.data());
    QPrintDialog dialog(m_printer.data(), this);
    dialog.setOptions(m_panel.m_dialogOptionsGroupBox->value<QPrintDialog::PrintDialogOptions>());
    if (dialog.exec() == QDialog::Accepted) {
        retrieveSettings(m_printer.data());
        print(m_printer.data());
    }
}

void PrintDialogPanel::showPreviewDialog()
{
    applySettings(m_printer.data());
    QPrintPreviewDialog dialog(m_printer.data(), this);
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)));
    dialog.resize(QApplication::desktop()->availableGeometry().size() * 4/ 5);
    if (dialog.exec() == QDialog::Accepted)
        retrieveSettings(m_printer.data());
}

void PrintDialogPanel::showPageSetupDialog()
{
    applySettings(m_printer.data());
    QPageSetupDialog dialog(m_printer.data(), this);
    if (dialog.exec() == QDialog::Accepted)
        retrieveSettings(m_printer.data());
}

void PrintDialogPanel::directPrint()
{
    applySettings(m_printer.data());
    print(m_printer.data());
    retrieveSettings(m_printer.data());
}

void PrintDialogPanel::print(QPrinter* printer)
{
    switch (m_testDocument) {
    case PrintDialogPanel::PageLayoutTest:
        printPageLayoutTest(printer);
        break;
    case PrintDialogPanel::RichTextTest:
        m_textDocument->print(printer);
        break;
    }
}

void PrintDialogPanel::initRichTextDocument()
{
    if (m_textDocument == 0) {
        QFileInfo fileName = QFileInfo("documents/richtext.html");
        QFile file(fileName.absoluteFilePath());
        if (!file.open(QFile::ReadOnly))
            return;

        m_textDocument = new QTextDocument;
        QByteArray data = file.readAll();
        QTextCodec *codec = Qt::codecForHtml(data);
        QString str = codec->toUnicode(data);
        if (Qt::mightBeRichText(str)) {
            m_textDocument->setHtml(str);
        } else {
            str = QString::fromLocal8Bit(data);
            m_textDocument->setPlainText(str);
        }
        m_textDocument->setMetaInformation(QTextDocument::DocumentUrl,
                                           QUrl::fromLocalFile(fileName.absoluteFilePath()).url());
    }
    m_panel.m_documentName->setText(m_textDocument->metaInformation(QTextDocument::DocumentTitle));
    if (m_panel.m_documentName->text().isEmpty())
        m_panel.m_documentName->setText("RichTextTest");
}

#include "moc_printdialogpanel.cpp"

#endif // !QT_NO_PRINTER
