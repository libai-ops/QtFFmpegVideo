/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "./PlayImage/playimage.h"

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout_2;
    QComboBox *com_url;
    QCheckBox *check_HW;
    QPushButton *but_file;
    QPushButton *but_open;
    QPushButton *but_pause;
    QGridLayout *gridLayout;
    PlayImage *playImage_3;
    PlayImage *playImage_4;
    PlayImage *playImage_1;
    PlayImage *playImage_9;
    PlayImage *playImage_7;
    PlayImage *playImage_5;
    PlayImage *playImage_8;
    PlayImage *playImage_2;
    PlayImage *playImage_6;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(972, 712);
        gridLayout_2 = new QGridLayout(Widget);
        gridLayout_2->setObjectName("gridLayout_2");
        com_url = new QComboBox(Widget);
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->addItem(QString());
        com_url->setObjectName("com_url");
        com_url->setEditable(true);
        com_url->setInsertPolicy(QComboBox::InsertAtBottom);
        com_url->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        com_url->setDuplicatesEnabled(false);

        gridLayout_2->addWidget(com_url, 0, 0, 1, 1);

        check_HW = new QCheckBox(Widget);
        check_HW->setObjectName("check_HW");
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(check_HW->sizePolicy().hasHeightForWidth());
        check_HW->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(check_HW, 0, 1, 1, 1);

        but_file = new QPushButton(Widget);
        but_file->setObjectName("but_file");
        sizePolicy.setHeightForWidth(but_file->sizePolicy().hasHeightForWidth());
        but_file->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(but_file, 0, 2, 1, 1);

        but_open = new QPushButton(Widget);
        but_open->setObjectName("but_open");
        sizePolicy.setHeightForWidth(but_open->sizePolicy().hasHeightForWidth());
        but_open->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(but_open, 0, 3, 1, 1);

        but_pause = new QPushButton(Widget);
        but_pause->setObjectName("but_pause");
        sizePolicy.setHeightForWidth(but_pause->sizePolicy().hasHeightForWidth());
        but_pause->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(but_pause, 0, 4, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(1);
        gridLayout->setObjectName("gridLayout");
        playImage_3 = new PlayImage(Widget);
        playImage_3->setObjectName("playImage_3");

        gridLayout->addWidget(playImage_3, 0, 2, 1, 1);

        playImage_4 = new PlayImage(Widget);
        playImage_4->setObjectName("playImage_4");

        gridLayout->addWidget(playImage_4, 1, 0, 1, 1);

        playImage_1 = new PlayImage(Widget);
        playImage_1->setObjectName("playImage_1");

        gridLayout->addWidget(playImage_1, 0, 0, 1, 1);

        playImage_9 = new PlayImage(Widget);
        playImage_9->setObjectName("playImage_9");

        gridLayout->addWidget(playImage_9, 2, 2, 1, 1);

        playImage_7 = new PlayImage(Widget);
        playImage_7->setObjectName("playImage_7");

        gridLayout->addWidget(playImage_7, 2, 0, 1, 1);

        playImage_5 = new PlayImage(Widget);
        playImage_5->setObjectName("playImage_5");

        gridLayout->addWidget(playImage_5, 1, 1, 1, 1);

        playImage_8 = new PlayImage(Widget);
        playImage_8->setObjectName("playImage_8");

        gridLayout->addWidget(playImage_8, 2, 1, 1, 1);

        playImage_2 = new PlayImage(Widget);
        playImage_2->setObjectName("playImage_2");

        gridLayout->addWidget(playImage_2, 0, 1, 1, 1);

        playImage_6 = new PlayImage(Widget);
        playImage_6->setObjectName("playImage_6");

        gridLayout->addWidget(playImage_6, 1, 2, 1, 1);


        gridLayout_2->addLayout(gridLayout, 1, 0, 1, 5);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        com_url->setItemText(0, QCoreApplication::translate("Widget", "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4", nullptr));
        com_url->setItemText(1, QCoreApplication::translate("Widget", "rtmp://ns8.indexforce.com/home/mystream", nullptr));
        com_url->setItemText(2, QCoreApplication::translate("Widget", "rtmp://58.200.131.2:1935/livetv/cctv1", nullptr));
        com_url->setItemText(3, QCoreApplication::translate("Widget", "http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4", nullptr));
        com_url->setItemText(4, QCoreApplication::translate("Widget", "http://playertest.longtailvideo.com/adaptive/bipbop/gear4/prog_index.m3u8", nullptr));
        com_url->setItemText(5, QCoreApplication::translate("Widget", "http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4", nullptr));
        com_url->setItemText(6, QCoreApplication::translate("Widget", "http://vjs.zencdn.net/v/oceans.mp4", nullptr));
        com_url->setItemText(7, QCoreApplication::translate("Widget", "https://media.w3.org/2010/05/sintel/trailer.mp4", nullptr));
        com_url->setItemText(8, QCoreApplication::translate("Widget", "https://sf1-hscdn-tos.pstatp.com/obj/media-fe/xgplayer_doc_video/flv/xgplayer-demo-360p.flv", nullptr));

        check_HW->setText(QCoreApplication::translate("Widget", "\347\241\254\350\247\243\347\240\201", nullptr));
        but_file->setText(QCoreApplication::translate("Widget", "\351\200\211\346\213\251", nullptr));
        but_open->setText(QCoreApplication::translate("Widget", "\345\274\200\345\247\213\346\222\255\346\224\276", nullptr));
        but_pause->setText(QCoreApplication::translate("Widget", "\346\232\202\345\201\234", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
