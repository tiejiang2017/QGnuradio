#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<gnuradio/audio/sink.h>
#include<gnuradio/audio/source.h>
#include<gnuradio-runtime/include/gnuradio/hier_block2.h>
#include<beklBlk/beklUdpSourece.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),gr::top_block("topblock"),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("gnuradio");
    wavSource = gr::blocks::wavfile_source::make("/home/bekl/gnuRadio/proj/1.wav", true);
    audioSink = gr::audio::sink::make(44100, "", true);




    gr::basic_block_sptr ws = boost::dynamic_pointer_cast<basic_block, gr::blocks::wavfile_source>(wavSource);
    gr::basic_block_sptr as = boost::dynamic_pointer_cast<basic_block, gr::audio::sink>(audioSink);



    //udpSptr = gr::beklBlock::beklUdpSource::make(1,"192.168.6.88",28678,40960000,false);
    udpSptr = gr::beklBlock::beklUdpSource::make(1,"127.0.0.1",28678,40960000,false);
    ampAnalyPtr = gr::beklBlk::beklAmpAnalysis::make(1);
    fileSinkSptr = gr::blocks::file_sink::make(20480000,"udp.dat",false);
    fileSourceSptr = gr::blocks::file_source::make(1,"/home/bekl/gnuRadio/proj/udpSave.dat");
    antennaPrehandlePtr = gr::beklBlock::beklAntennaPrehandle::make("/home/bekl/gnu_handle/package.ini","/home/bekl/gnu_handle/magdec.ini");
    //gr::hier_block2::connect(udpSptr,0,ampAnalyPtr,0);
   // gr::hier_block2::connect(ampAnalyPtr,0,fileSinkSptr,0);
    gr::hier_block2::connect(fileSourceSptr,0,antennaPrehandlePtr,0);
    gr::hier_block2::connect(antennaPrehandlePtr,0,fileSinkSptr,0);
//    gr::hier_block2_sptr wsHier =  boost::dynamic_pointer_cast<gr::hier_block2, gr::basic_block>(ws);

//     boost::shared_ptr<gr::blocks::wavfile_source> a =  boost::dynamic_pointer_cast<gr::blocks::wavfile_source, gr::basic_block>(ws);
//    gr::hier_block2::connect(ws,0,as,0);
//    gr::hier_block2::connect(udpSptr,0,fileSinkSptr,0);
    //gr::hier_block2::connect(udpSptr,0,ampAnalyPtr,0);
   // gr::hier_block2::connect(ampAnalyPtr,0,fileSinkSptr,0);
   // gr::hier_block2::connect(wavSource,0,audioSink,0);
//    gr::hier_block2::connect(boost::dynamic_pointer_cast<basic_block, gr::blocks::wavfile_source>(wavSource),0,\
                           boost::dynamic_pointer_cast<basic_block, gr::audio::sink>(audioSink),0);
    this->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
