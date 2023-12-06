# FinRes
<h1>Development of a Cross-Platform Audio Recording and Spectrogram Visualization Tool</h1> 

<h3>Create a cross-platform software application that records audio through the system microphone and visualizes the log mel spectrogram of the recorded audio in real-time.</h3>

``` widget.h ```

It includes several header files for Qt classes such as QMediaRecorder, QPushButton, QCustomPlot, etc. 

The class is defined with one constructor that takes two parameters: a reference to a QAudioDevice object called "deviceInfo" and a pointer to a QWidget object called "parent". There is also a destructor defined.

The private section of the class contains several member variables, including a pointer to an XYSeriesIODevice object called "m_device", a pointer to a QChart object called "m_chart", a pointer to a QLineSeries object called "m_series", a pointer to a QAudioInput object called "m_audioInput", a pointer to a QAudioSource object called "m_audioSource", a pointer to a QMediaRecorder object called "m_audioRecorder", a QMediaCaptureSession object called "m_captureSession" and a pointer to a QCustomPlot object called "spectrogramPlot". Additionally, there is a QTimer object called "updateTimer" defined, which is used for updating the spectrogram.

There are several private slots defined for handling events related to the QMediaRecorder object, starting and stopping the recording, setting up the spectrogram plot, and updating the spectrogram.


``` widget.cpp ```

This code includes a QChart and a QLineSeries, which are displayed on a QVBoxLayout. It takes the QAudioDevice constructor, initializes the m_chart with the QChartView, and sets the color scheme. 

Then, it initializes an audio input named m_audioInput and an audio source named m_audioSource, with a set sample rate of 8000 Hz, a channel count of 1, and a sample format of unsigned 8-bit integer. A device is created named m_device as an XYSeriesIODevice, which operates in write-only mode, and the audio source is started with this device as its target. SpectrogramPlot is also set up through setupSpectrogramPlot(). 

An audio recorder is initialized as m_audioRecorder with its recording state connected to the handleStateChanged slot. A button called startRecordingButton is added to the widget interface which calls the startRecording function when clicked. The audio recorder's output file location is set to be a local file named "recordedFile.wav". 

In short, this code sets up a microphone input, plots a graph representing audio data, starts an audio recording when a button is clicked, and saves the recording to a local file.


``` If you use Mac, Please follow these steps to run and build the app ```

1) Install Xcode
2) Install Qt 6.6.x
3) Open CMakeList.txt via QtCreator and set by macOS Desktop Kit Qt 6.6.1 for macOS.
4) Run CMake and Build!
