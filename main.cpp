// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "widget.h"

#include <QApplication>
#include <QGuiApplication>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QMessageBox>

#if QT_CONFIG(permissions)
#include <QPermission>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();
    if (inputDevice.isNull()) {
        QMessageBox::warning(nullptr, "audio",
                             "There is no audio input device available.");
        return -1;
    }

#if QT_CONFIG(permissions)
    QCameraPermission cameraPermission;
    qApp->requestPermission(cameraPermission, [&](const QPermission &permission) {
        if (permission.status() != Qt::PermissionStatus::Granted) {
            qWarning("Camera permission is not granted!");
            return;
        }
        QMicrophonePermission micPermission;
        qApp->requestPermission(micPermission, [&](const QPermission &permission) {
            if (permission.status() != Qt::PermissionStatus::Granted) {
                qWarning("Microphone permission is not granted!");
            } else {

            }
        });
    });
#endif

    Widget w(inputDevice);
    w.setStyleSheet("background-color: gray;");
    w.resize(800, 800);
    w.show();
    return a.exec();
}
