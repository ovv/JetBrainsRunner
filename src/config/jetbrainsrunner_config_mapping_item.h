#ifndef JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H
#define JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H

#include <QtWidgets/QFileDialog>
#include <QBitmap>
#include "jetbrains-api/JetbrainsApplication.h"
#include "ui_jetbrainsrunner_config_mapping_item.h"

class JetbrainsRunnerConfigMappingItem : public QWidget, public Ui::JetbrainsRunnerConfigItemUi {
Q_OBJECT

public:
    explicit JetbrainsRunnerConfigMappingItem(QWidget *parent, const QString &desktopFilePath = QString(), const QString &configFilePath = QString())
            : QWidget(parent) {
        setupUi(this);

        if (!desktopFilePath.isEmpty() && !configFilePath.isEmpty()) {
            this->configDesktoFilePushButton->setText(desktopFilePath);
            this->configXMLFilePushButton->setText(configFilePath);
        }

        connect(this->deletePushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfigMappingItem::changed);
        connect(this->deletePushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfigMappingItem::deleteMappingItem);
        connect(this->configDesktoFilePushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfigMappingItem::changed);
        connect(this->configDesktoFilePushButton, &QPushButton::clicked, this,
                &JetbrainsRunnerConfigMappingItem::openDesktopFileChooser);
        connect(this->configXMLFilePushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfigMappingItem::changed);
        connect(this->configXMLFilePushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfigMappingItem::openConfigFileChooser);

        validateInfoWidget();
    }

public Q_SLOTS:
    void openDesktopFileChooser() {
        const QString desktopFile = QFileDialog::getOpenFileName(this, tr("Select Desktop File"), QString(),
                                                                 tr("Desktop File (*.desktop)"));
        if (!desktopFile.isEmpty()) {
            this->configDesktoFilePushButton->setText(desktopFile);
        }
        validateInfoWidget();
    }

    void openConfigFileChooser() {
        const QString configLocation =
                QFileDialog::getOpenFileName(this, tr("Select Config File"), QString(),
                                             tr("XML File (*.xml)"));
        if (!configLocation.isEmpty()) {
            this->configXMLFilePushButton->setText(configLocation);
        }
        validateInfoWidget();
    }

    void validateInfoWidget() {
        const QString desktopFile = this->configDesktoFilePushButton->text().remove('&');
        const QString configFile = this->configXMLFilePushButton->text().remove('&');
        if (QFileInfo::exists(desktopFile)) {
            this->configTitleWidget->setHidden(false);
            JetbrainsApplication newApp(desktopFile);
            this->appNameLabel->setText(newApp.name);
            const QPixmap pixmap(newApp.iconPath);
            this->applicationIconLabel->setPixmap(pixmap);
            this->applicationIconLabel->setMask(pixmap.mask());
        } else {
            this->configTitleWidget->setHidden(true);
        }
        if (QFileInfo::exists(configFile)) {
            QFile file(configFile);
            if (file.open(QFile::ReadOnly)) {
                this->configReadWidget->setHidden(false);
                JetbrainsApplication newApp(desktopFile);
                newApp.parseXMLFile(file.readAll());

                QString recentApplicationsText;
                for (const auto &recentProject: qAsConst(newApp.recentlyUsed)) {
                    recentApplicationsText.append(recentProject.path + "\n");
                }
                this->recentProjectsLabel->setText(recentApplicationsText);
            }
        } else {
            this->configReadWidget->setHidden(true);
        }
    }

Q_SIGNALS:
    void changed();
    void deleteMappingItem();
};

#endif //JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H
