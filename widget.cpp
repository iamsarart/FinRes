// widget.cpp
#include "widget.h"
#include "xyseriesiodevice.h"

#include <QAudioDevice>
#include <QAudioInput>
#include <QAudioSource>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QVBoxLayout>
#include <QPushButton>

Widget::Widget(const QAudioDevice &deviceInfo, QWidget *parent)
    : QWidget(parent)
    , m_chart(new QChart)
    , m_series(new QLineSeries)
    , spectrogramPlot(new QCustomPlot)
    , updateTimer(new QTimer(this))  // Initialize the timer in the constructor
{
    // Initialize UI components
    setupSpectrogramPlot();
    auto chartView = new QChartView(m_chart);
    chartView->setFixedHeight(300); // Set the height you desire
    QPen pen(Qt::red);
    QBrush brush(Qt::red); // Set the desired color
    m_series->setPen(pen);
    m_series->setBrush(brush); // Set the brush to the series

    m_chart->addSeries(m_series);
    auto axisX = new QValueAxis;
    axisX->setRange(0, XYSeriesIODevice::sampleCount);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");


    auto axisY = new QValueAxis;
    axisY->setRange(-1, 1);
    axisY->setTitleText("Audio level");
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_series->attachAxis(axisX);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisY);
    m_chart->legend()->hide();
    m_chart->setTitle("Data from the microphone (" + deviceInfo.description() + ')');

    chartView->setStyleSheet("background-color: black;");
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);

    m_audioInput = new QAudioInput(deviceInfo, this);

    QAudioFormat formatAudio;
    formatAudio.setSampleRate(8000);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleFormat(QAudioFormat::UInt8);

    m_audioSource = new QAudioSource(deviceInfo, formatAudio);
    m_audioSource->setBufferSize(200);

    m_device = new XYSeriesIODevice(m_series, this);
    m_device->open(QIODevice::WriteOnly);

    m_audioSource->start(m_device);

    // Create audio recorder
    m_audioRecorder = new QMediaRecorder(this);

    m_audioRecorder = new QMediaRecorder(this);
    m_captureSession.setRecorder(m_audioRecorder);
    m_captureSession.setAudioInput(new QAudioInput(this));

    // Set output location for the recorded audio file
    m_audioRecorder->setOutputLocation(QUrl::fromLocalFile("recordedFile.wav"));

    // Connect audioRecorder's stateChanged signal to handleStateChanged slot
    connect(m_audioRecorder, &QMediaRecorder::recorderStateChanged, this, &Widget::handleStateChanged);

    // Add Start Recording button
    QPushButton *startRecordingButton = new QPushButton("Start Recording", this);
    startRecordingButton->setStyleSheet("QPushButton {"
                                        "    background-color: #343434;" // Green background
                                        "    color: white;"              // White text color
                                        "    border: 2px solid #f2f2f2;" // Green border
                                        "    border-radius: 5px;"        // Rounded corners
                                        "    padding: 8px 16px;"          // Padding
                                        "}"
                                        "QPushButton:hover {"
                                        "    background-color: #4a4a4a;" // Darker green on hover
                                        "}");
    connect(startRecordingButton, &QPushButton::clicked, this, &Widget::startRecording);

    // Add Stop Recording button
    QPushButton *stopRecordingButton = new QPushButton("Stop Recording", this);
    stopRecordingButton->setStyleSheet("QPushButton {"
                                       "    background-color: #343434;" // Green background
                                       "    color: white;"              // White text color
                                       "    border: 2px solid #f2f2f2;" // Green border
                                       "    border-radius: 5px;"        // Rounded corners
                                       "    padding: 8px 16px;"          // Padding
                                       "}"
                                       "QPushButton:hover {"
                                       "    background-color: #4a4a4a;" // Darker green on hover
                                       "}");


    connect(stopRecordingButton, &QPushButton::clicked, this, &Widget::stopRecording);

    // Connect the timer to the updateSpectrogram slot
    connect(updateTimer, &QTimer::timeout, this, &Widget::updateSpectrogram);
    updateTimer->start(100);  // Update every 100 milliseconds (adjust as needed)

    // Add buttons to the layout
    mainLayout->addWidget(startRecordingButton);
    mainLayout->addWidget(stopRecordingButton);

    if (mainLayout) {
        mainLayout->addWidget(spectrogramPlot);
    }


}

Widget::~Widget()
{
    // Stop recording before destruction
    m_captureSession.recorder()->stop();
    delete m_audioRecorder;

    m_audioSource->stop();
    m_device->close();
    delete m_audioInput;
    delete m_device;
}

void Widget::updateSpectrogram()
{
    // Assuming you have 5 graphs in the spectrogram
    for (int gi = 0; gi < 2; ++gi)
    {
        QCPGraph *graph = spectrogramPlot->graph(gi);
        QVector<QCPGraphData> timeData(250);

        double now = QDateTime::currentDateTime().toSecsSinceEpoch();

        //QVector<double> audioData = m_audioSource->readAudioData(); // Has issue! we have to change this by audo

        for (int i = 0; i < 250; ++i)
        {
            timeData[i].key = now + 24 * 3600 * i;
            if (i == 0)
                timeData[i].value = (i / 50.0 + 1) * (rand() / (double)RAND_MAX - 0.5);
            else
                timeData[i].value = qFabs(timeData[i - 1].value) * (1 + 0.02 / 4.0 * (4 - gi)) + (i / 50.0 + 1) * (rand() / (double)RAND_MAX - 0.5);
        }

        graph->data()->set(timeData);
    }

    // Replot the spectrogram
    spectrogramPlot->replot();
}


void Widget::setupSpectrogramPlot()
{
    spectrogramPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

    spectrogramPlot->addGraph(); // Graph for frequency
    QColor frequencyColor(0, 0, 255); // Dark Blue
    auto frequencyGraph = spectrogramPlot->graph();
    frequencyGraph->setLineStyle(QCPGraph::lsLine);
    frequencyGraph->setPen(QPen(frequencyColor.lighter(200)));
    frequencyGraph->setBrush(QBrush(frequencyColor));

    spectrogramPlot->addGraph(); // Graph for time
    QColor timeColor(255, 0, 0); // Red
    auto timeGraph = spectrogramPlot->graph(1); // Assuming the graph for time is the second one
    timeGraph->setLineStyle(QCPGraph::lsLine);
    timeGraph->setPen(QPen(timeColor.lighter(200)));
    timeGraph->setBrush(QBrush(timeColor));

    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("h:mm:ss");
    spectrogramPlot->xAxis->setTicker(timeTicker);

    QSharedPointer<QCPAxisTickerText> frequencyTicker(new QCPAxisTickerText);
    frequencyTicker->addTick(10, "Low");
    frequencyTicker->addTick(50, "High");
    spectrogramPlot->yAxis->setTicker(frequencyTicker);

    spectrogramPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    spectrogramPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    spectrogramPlot->xAxis->setLabel("Time");
    spectrogramPlot->yAxis->setLabel("Frequency (kHz)");

    spectrogramPlot->xAxis2->setVisible(true);
    spectrogramPlot->yAxis2->setVisible(true);
    spectrogramPlot->xAxis2->setTicks(true);
    spectrogramPlot->yAxis2->setTicks(true);
    spectrogramPlot->xAxis2->setTickLabels(true);
    spectrogramPlot->yAxis2->setTickLabels(true);

    double startTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    double endTime = startTime + 24 * 3600 * 249;
    spectrogramPlot->xAxis->setRange(startTime, endTime);

    double minY = 0;
    double maxY = 60;
    spectrogramPlot->yAxis->setRange(minY, maxY);

    spectrogramPlot->legend->setVisible(true);
    spectrogramPlot->legend->setBrush(QColor(255, 255, 255, 150));

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(layout());
    if (mainLayout)
    {
        mainLayout->addWidget(spectrogramPlot);
    }
}


void Widget::startRecording()
{
    qDebug() << "Manual recording started";
    m_captureSession.recorder()->record();
}

void Widget::stopRecording()
{
    qDebug() << "Manual recording stopped";
    m_captureSession.recorder()->stop();
}

void Widget::handleStateChanged(QMediaRecorder::RecorderState state)
{
    if (state == QMediaRecorder::RecordingState) {
        // Recording started
        qDebug() << "Recording started";
    } else if (state == QMediaRecorder::StoppedState) {
        // Recording stopped
        qDebug() << "Recording stopped";
    }
}
