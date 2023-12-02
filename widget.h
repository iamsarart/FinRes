#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaRecorder>
#include <QPushButton>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include "qcustomplot.h"

QT_FORWARD_DECLARE_CLASS(QAudioDevice)
QT_FORWARD_DECLARE_CLASS(QAudioInput)
QT_FORWARD_DECLARE_CLASS(QAudioSource)
QT_FORWARD_DECLARE_CLASS(QChart)
QT_FORWARD_DECLARE_CLASS(QLineSeries)
QT_FORWARD_DECLARE_CLASS(QAudioRecorder)

class XYSeriesIODevice;

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(const QAudioDevice &deviceInfo, QWidget *parent = nullptr);
    ~Widget();

private:
    XYSeriesIODevice *m_device = nullptr;
    QChart *m_chart = nullptr;
    QLineSeries *m_series = nullptr;
    QAudioInput *m_audioInput = nullptr;
    QAudioSource *m_audioSource = nullptr;
    QMediaRecorder *m_audioRecorder = nullptr;
    QMediaCaptureSession m_captureSession;
    QCustomPlot *spectrogramPlot;

    QTimer *updateTimer;  // Timer for updating spectrogram

private slots:
    void startRecording();
    void stopRecording();
    void handleStateChanged(QMediaRecorder::RecorderState state);
    void setupSpectrogramPlot();
    void updateSpectrogram();  // Slot to update spectrogram data
};

#endif
