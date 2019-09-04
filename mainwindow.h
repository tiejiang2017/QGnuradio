#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<gnuradio-runtime/include/gnuradio/top_block.h>
#include<gnuradio/audio/sink.h>
#include<gnuradio/blocks/wavfile_source.h>
#include<wavfile_source_impl.h>
#include<beklBlk/beklUdpSource.h>
#include<beklBlk/beklAmpAnalysis.h>
#include<beklAntennaPrehandle_impl.h>
#include<gnuradio/blocks/file_sink.h>
#include<gnuradio/blocks/file_source.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow,gr::top_block
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
//    gr::audio::sink::sptr audioSink;
    gr::audio::sink::sptr audioSink;
    gr::blocks::wavfile_source::sptr wavSource;
    boost::shared_ptr<gr::beklBlock::beklUdpSource> udpSptr;
    boost::shared_ptr<gr::blocks::file_sink> fileSinkSptr;
    boost::shared_ptr<gr::blocks::file_source> fileSourceSptr;
    boost::shared_ptr<gr::beklBlk::beklAmpAnalysis>  ampAnalyPtr;
    boost::shared_ptr<gr::beklBlock::beklAntennaPrehandle>  antennaPrehandlePtr;
//    boost::shared_ptr<gr::blocks::wavfile_source_impl> wavSource;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
