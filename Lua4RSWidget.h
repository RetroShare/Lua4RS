/********************************************************************************
** Form generated from reading UI file 'Lua4RSWidget.ui'
**
** Created: Wed May 28 12:58:58 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef LUA4RSWIDGET_H
#define LUA4RSWIDGET_H

#include <Qt3Support/Q3Frame>
#include <Qt3Support/Q3TextEdit>
#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QTimeEdit>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QListView *listViewAllScripts;
    QPushButton *pushButtonNewScript;
    QPushButton *pushButtonEditScript;
    QPushButton *pushButtonDeleteScript;
    QLabel *label_2;
    QGroupBox *groupBox;
    Q3TextEdit *textEditLuaEditor;
    QLabel *label_3;
    QTabWidget *tabWidget;
    QWidget *tab_RunByTimer;
    QLabel *label_4;
    QSpinBox *spinBoxTimeUnitsCount;
    QComboBox *comboBoxTimeUnits;
    QLabel *label_5;
    QDateTimeEdit *dateTimeEditRunOnce;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QFrame *line;
    QFrame *line_2;
    QRadioButton *radioButton_3;
    QLabel *label_7;
    QFrame *line_4;
    QWidget *tab_RunByEvent;
    QWidget *tab;
    QTextBrowser *textBrowserLog;
    QCheckBox *checkBoxEnableScript;
    QTimeEdit *timeEditEnableScriptFrom;
    QLabel *label_8;
    QTimeEdit *timeEditEnableScriptTo;
    QTreeView *treeViewRSProperties;
    QLineEdit *lineEditScriptName;
    QLabel *label_10;
    QLabel *label_6;
    QPushButton *pushButtonUndock;
    QPushButton *pushButtonLoadScript;
    QPushButton *pushButtonClearEditor;
    QPushButton *pushButtonSaveScript;
    QLabel *label_9;
    QCheckBox *checkBox;
    QLabel *label;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(811, 710);
        listViewAllScripts = new QListView(Form);
        listViewAllScripts->setObjectName(QString::fromUtf8("listViewAllScripts"));
        listViewAllScripts->setGeometry(QRect(10, 30, 791, 111));
        listViewAllScripts->setFrameShape(QFrame::WinPanel);
        listViewAllScripts->setFrameShadow(QFrame::Sunken);
        listViewAllScripts->setLineWidth(2);
        listViewAllScripts->setMidLineWidth(0);
        listViewAllScripts->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        listViewAllScripts->setEditTriggers(QAbstractItemView::DoubleClicked);
        listViewAllScripts->setDragDropOverwriteMode(false);
        listViewAllScripts->setAlternatingRowColors(true);
        listViewAllScripts->setSelectionMode(QAbstractItemView::SingleSelection);
        listViewAllScripts->setSelectionBehavior(QAbstractItemView::SelectRows);
        listViewAllScripts->setTextElideMode(Qt::ElideLeft);
        pushButtonNewScript = new QPushButton(Form);
        pushButtonNewScript->setObjectName(QString::fromUtf8("pushButtonNewScript"));
        pushButtonNewScript->setGeometry(QRect(9, 142, 75, 23));
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        pushButtonNewScript->setFont(font);
        pushButtonEditScript = new QPushButton(Form);
        pushButtonEditScript->setObjectName(QString::fromUtf8("pushButtonEditScript"));
        pushButtonEditScript->setGeometry(QRect(84, 142, 75, 23));
        pushButtonEditScript->setFont(font);
        pushButtonDeleteScript = new QPushButton(Form);
        pushButtonDeleteScript->setObjectName(QString::fromUtf8("pushButtonDeleteScript"));
        pushButtonDeleteScript->setGeometry(QRect(709, 142, 75, 23));
        pushButtonDeleteScript->setFont(font);
        label_2 = new QLabel(Form);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 10, 81, 16));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label_2->setFont(font1);
        groupBox = new QGroupBox(Form);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 180, 791, 521));
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox { border:1px solid grey }"));
        textEditLuaEditor = new Q3TextEdit(groupBox);
        textEditLuaEditor->setObjectName(QString::fromUtf8("textEditLuaEditor"));
        textEditLuaEditor->setGeometry(QRect(10, 59, 541, 191));
        QFont font2;
        font2.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        font2.setPointSize(9);
        textEditLuaEditor->setFont(font2);
        textEditLuaEditor->setFrameShape(QFrame::WinPanel);
        textEditLuaEditor->setLineWidth(0);
        textEditLuaEditor->setMidLineWidth(0);
        textEditLuaEditor->setMargin(0);
        textEditLuaEditor->setVScrollBarMode(Q3ScrollView::AlwaysOn);
        textEditLuaEditor->setDragAutoScroll(true);
        textEditLuaEditor->setTextFormat(Qt::PlainText);
        textEditLuaEditor->setWordWrap(Q3TextEdit::NoWrap);
        textEditLuaEditor->setWrapColumnOrWidth(-1);
        textEditLuaEditor->setAutoFormatting(Q3TextEdit::AutoNone);
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(560, 40, 221, 20));
        QFont font3;
        font3.setBold(true);
        font3.setWeight(75);
        font3.setKerning(true);
        label_3->setFont(font3);
        label_3->setLayoutDirection(Qt::LeftToRight);
        label_3->setStyleSheet(QString::fromUtf8("border:0px;"));
        label_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        tabWidget = new QTabWidget(groupBox);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 290, 771, 191));
        tabWidget->setFont(font1);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tab_RunByTimer = new QWidget();
        tab_RunByTimer->setObjectName(QString::fromUtf8("tab_RunByTimer"));
        label_4 = new QLabel(tab_RunByTimer);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(40, 10, 61, 21));
        spinBoxTimeUnitsCount = new QSpinBox(tab_RunByTimer);
        spinBoxTimeUnitsCount->setObjectName(QString::fromUtf8("spinBoxTimeUnitsCount"));
        spinBoxTimeUnitsCount->setGeometry(QRect(90, 10, 61, 21));
        QFont font4;
        font4.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        spinBoxTimeUnitsCount->setFont(font4);
        spinBoxTimeUnitsCount->setLayoutDirection(Qt::LeftToRight);
        spinBoxTimeUnitsCount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        spinBoxTimeUnitsCount->setValue(10);
        comboBoxTimeUnits = new QComboBox(tab_RunByTimer);
        comboBoxTimeUnits->setObjectName(QString::fromUtf8("comboBoxTimeUnits"));
        comboBoxTimeUnits->setGeometry(QRect(160, 10, 141, 21));
        comboBoxTimeUnits->setEditable(true);
        comboBoxTimeUnits->setMaxVisibleItems(15);
        label_5 = new QLabel(tab_RunByTimer);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(40, 50, 61, 21));
        dateTimeEditRunOnce = new QDateTimeEdit(tab_RunByTimer);
        dateTimeEditRunOnce->setObjectName(QString::fromUtf8("dateTimeEditRunOnce"));
        dateTimeEditRunOnce->setGeometry(QRect(90, 50, 211, 21));
        dateTimeEditRunOnce->setFont(font4);
        dateTimeEditRunOnce->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        dateTimeEditRunOnce->setDate(QDate(2000, 10, 30));
        radioButton = new QRadioButton(tab_RunByTimer);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(10, 10, 21, 16));
        radioButton->setChecked(true);
        radioButton_2 = new QRadioButton(tab_RunByTimer);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setGeometry(QRect(10, 50, 21, 16));
        line = new QFrame(tab_RunByTimer);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 30, 751, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(tab_RunByTimer);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 70, 751, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        radioButton_3 = new QRadioButton(tab_RunByTimer);
        radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));
        radioButton_3->setGeometry(QRect(10, 90, 21, 16));
        label_7 = new QLabel(tab_RunByTimer);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(40, 90, 131, 21));
        line_4 = new QFrame(tab_RunByTimer);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(10, 110, 751, 20));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        tabWidget->addTab(tab_RunByTimer, QString());
        tab_RunByEvent = new QWidget();
        tab_RunByEvent->setObjectName(QString::fromUtf8("tab_RunByEvent"));
        tabWidget->addTab(tab_RunByEvent, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        textBrowserLog = new QTextBrowser(tab);
        textBrowserLog->setObjectName(QString::fromUtf8("textBrowserLog"));
        textBrowserLog->setGeometry(QRect(10, 10, 747, 142));
        textBrowserLog->setFont(font2);
        textBrowserLog->setFrameShape(QFrame::WinPanel);
        tabWidget->addTab(tab, QString());
        checkBoxEnableScript = new QCheckBox(groupBox);
        checkBoxEnableScript->setObjectName(QString::fromUtf8("checkBoxEnableScript"));
        checkBoxEnableScript->setGeometry(QRect(10, 491, 111, 20));
        checkBoxEnableScript->setFont(font1);
        checkBoxEnableScript->setChecked(true);
        timeEditEnableScriptFrom = new QTimeEdit(groupBox);
        timeEditEnableScriptFrom->setObjectName(QString::fromUtf8("timeEditEnableScriptFrom"));
        timeEditEnableScriptFrom->setEnabled(true);
        timeEditEnableScriptFrom->setGeometry(QRect(236, 490, 101, 21));
        timeEditEnableScriptFrom->setFont(font4);
        timeEditEnableScriptFrom->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        timeEditEnableScriptFrom->setTime(QTime(8, 0, 0));
        timeEditEnableScriptFrom->setCalendarPopup(true);
        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setEnabled(true);
        label_8->setGeometry(QRect(333, 491, 41, 20));
        label_8->setFont(font);
        label_8->setAlignment(Qt::AlignCenter);
        timeEditEnableScriptTo = new QTimeEdit(groupBox);
        timeEditEnableScriptTo->setObjectName(QString::fromUtf8("timeEditEnableScriptTo"));
        timeEditEnableScriptTo->setEnabled(true);
        timeEditEnableScriptTo->setGeometry(QRect(370, 490, 101, 21));
        timeEditEnableScriptTo->setFont(font4);
        timeEditEnableScriptTo->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        timeEditEnableScriptTo->setTime(QTime(17, 0, 0));
        treeViewRSProperties = new QTreeView(groupBox);
        treeViewRSProperties->setObjectName(QString::fromUtf8("treeViewRSProperties"));
        treeViewRSProperties->setGeometry(QRect(560, 59, 221, 191));
        treeViewRSProperties->setFrameShape(QFrame::WinPanel);
        treeViewRSProperties->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        lineEditScriptName = new QLineEdit(groupBox);
        lineEditScriptName->setObjectName(QString::fromUtf8("lineEditScriptName"));
        lineEditScriptName->setGeometry(QRect(86, 10, 451, 21));
        lineEditScriptName->setFont(font);
        lineEditScriptName->setMaxLength(80);
        lineEditScriptName->setFrame(true);
        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(10, 13, 81, 16));
        label_10->setFont(font1);
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setEnabled(true);
        label_6->setGeometry(QRect(478, 491, 41, 20));
        label_6->setFont(font);
        pushButtonUndock = new QPushButton(groupBox);
        pushButtonUndock->setObjectName(QString::fromUtf8("pushButtonUndock"));
        pushButtonUndock->setGeometry(QRect(159, 251, 75, 23));
        pushButtonLoadScript = new QPushButton(groupBox);
        pushButtonLoadScript->setObjectName(QString::fromUtf8("pushButtonLoadScript"));
        pushButtonLoadScript->setGeometry(QRect(9, 251, 75, 23));
        pushButtonClearEditor = new QPushButton(groupBox);
        pushButtonClearEditor->setObjectName(QString::fromUtf8("pushButtonClearEditor"));
        pushButtonClearEditor->setGeometry(QRect(462, 251, 75, 23));
        pushButtonSaveScript = new QPushButton(groupBox);
        pushButtonSaveScript->setObjectName(QString::fromUtf8("pushButtonSaveScript"));
        pushButtonSaveScript->setGeometry(QRect(84, 251, 75, 23));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 40, 101, 20));
        label_9->setFont(font1);
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(151, 491, 81, 20));
        checkBox->setFont(font);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(false);
        label->setGeometry(QRect(558, 301, 221, 16));
        QFont font5;
        font5.setFamily(QString::fromUtf8("DejaVu Sans"));
        font5.setPointSize(7);
        font5.setBold(false);
        font5.setWeight(50);
        label->setFont(font5);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        groupBox->raise();
        listViewAllScripts->raise();
        pushButtonNewScript->raise();
        pushButtonEditScript->raise();
        pushButtonDeleteScript->raise();
        label_2->raise();

        retranslateUi(Form);

        tabWidget->setCurrentIndex(0);
        comboBoxTimeUnits->setCurrentIndex(5);


        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Lua4RS", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        listViewAllScripts->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pushButtonNewScript->setToolTip(QApplication::translate("Form", "Create a new empty script", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonNewScript->setText(QApplication::translate("Form", "New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonEditScript->setToolTip(QApplication::translate("Form", "Edit the selected script", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonEditScript->setText(QApplication::translate("Form", "Edit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonDeleteScript->setToolTip(QApplication::translate("Form", "Delete the selected script", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonDeleteScript->setText(QApplication::translate("Form", "Delete", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Form", "All My Scripts", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        textEditLuaEditor->setText(QApplication::translate("Form", "-- ----------------------------------------------------\n"
"-- Lua4RS Script : \"Bury-The-Dead\"\n"
"-- ----------------------------------------------------\n"
"if ( os.Date() - RS.Friend.LastSeen() >= 14 ) then\n"
"  RS.Friend.MoveToGroup(\"Graveyard\");\n"
"end;", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Form", "RetroShare Properties", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tabWidget->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("Form", "Every", 0, QApplication::UnicodeUTF8));
        comboBoxTimeUnits->clear();
        comboBoxTimeUnits->insertItems(0, QStringList()
         << QApplication::translate("Form", "Years", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Form", "Months", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Form", "Weeks", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Form", "Days", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Form", "Hours", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Form", "Minutes", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Form", "Seconds", 0, QApplication::UnicodeUTF8)
        );
        label_5->setText(QApplication::translate("Form", "Once", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        radioButton->setToolTip(QApplication::translate("Form", "Run the current script on an interval basis", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        radioButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        radioButton_2->setToolTip(QApplication::translate("Form", "Run the current script once at the specified time", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        radioButton_2->setText(QString());
#ifndef QT_NO_TOOLTIP
        radioButton_3->setToolTip(QApplication::translate("Form", "Run the current script only once when RS starts", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        radioButton_3->setText(QString());
        label_7->setText(QApplication::translate("Form", "At RestroShare startup", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_RunByTimer), QApplication::translate("Form", "Run by Timer", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabToolTip(tabWidget->indexOf(tab_RunByTimer), QApplication::translate("Form", "Script execution is controlled by a timer", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_RunByEvent), QApplication::translate("Form", "Run by Event", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabToolTip(tabWidget->indexOf(tab_RunByEvent), QApplication::translate("Form", "Script execution is controlled by RetroShare events", 0, QApplication::UnicodeUTF8));
        textBrowserLog->setHtml(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'DejaVu Sans Mono'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'DejaVu Sans'; font-size:8pt;\">27.05.2014;21:35:27 EDT;ExitCode=0;Execution Time=0.03 sec</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'DejaVu Sans'; font-size:8pt;\">27.05.2014;21:25:27 EDT;ExitCode=0;Execution Time=0.04 sec</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'DejaVu Sans'; font-size:8"
                        "pt;\">27.05.2014;20:15:27 EDT;Script modified</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'DejaVu Sans'; font-size:8pt;\">27.05.2014;21:13:19 EDT;ExitCode=0;Execution Time=0.03 sec</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'DejaVu Sans'; font-size:8pt;\">27.05.2014;21:03:19 EDT;ExitCode=0;Execution Time=0.02 sec</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'DejaVu Sans'; font-size:8pt;\">27.05.2014;20:53:19 EDT;Script created</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'DejaVu Sans'; font-size:8pt;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("Form", "Log", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        checkBoxEnableScript->setToolTip(QApplication::translate("Form", "<html><head/><body><p>Check/Uncheck to enable/disable execution of current script (&quot;00:00:00 - 00:00:00&quot; means always enabled).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        checkBoxEnableScript->setText(QApplication::translate("Form", "Enable Script", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        timeEditEnableScriptFrom->setToolTip(QApplication::translate("Form", "Start of execution interval", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_8->setText(QApplication::translate("Form", "and", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        timeEditEnableScriptTo->setToolTip(QApplication::translate("Form", "End of execution interval", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        treeViewRSProperties->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        lineEditScriptName->setText(QApplication::translate("Form", "Bury-The-Dead.lua", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Form", "Scriptname", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Form", "hours", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonUndock->setToolTip(QApplication::translate("Form", "Undock the editor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonUndock->setText(QApplication::translate("Form", "Undock", 0, QApplication::UnicodeUTF8));
        pushButtonLoadScript->setText(QApplication::translate("Form", "Load", 0, QApplication::UnicodeUTF8));
        pushButtonClearEditor->setText(QApplication::translate("Form", "Clear", 0, QApplication::UnicodeUTF8));
        pushButtonSaveScript->setText(QApplication::translate("Form", "Save", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Form", "LUA Code Editor", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("Form", "...between", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Form", "Lua4RS V0.0.10 by sehraf & far*call", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // LUA4RSWIDGET_H
