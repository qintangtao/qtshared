#include "screencapture.h"
//#include <QCoreApplication>
//#include <QDesktopWidget>
#include <QFile>
#include <QTime>
#include <qtwinextras/qwinfunctions.h>
#include "H264Encoder.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(logScreenCapture, "log.screen.capture")


extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavcodec/avfft.h"
    #include "libavdevice/avdevice.h"
    #include "libswscale/swscale.h"
    #include "libavformat/avformat.h"
    #include "libavformat/avio.h"
    #include "libavutil/common.h"
    #include "libavutil/avstring.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/time.h"
    #include "libavutil/frame.h"
    #include "libswresample/swresample.h"
}

ScreenCapture::ScreenCapture(QObject *parent) : QThread(parent)
{

}

void ScreenCapture::run()
{
    //QSize deskTopSize = QGuiApplication::primaryScreen()->availableSize();
    //QDesktopWidget* desktopWidget = QCoreApplication::desktop();
    //获取设备屏幕大小
    //QRect screenRect = desktopWidget->screenGeometry();
    int   cxScreen = GetSystemMetrics (SM_CXSCREEN) ;  // wide
    int   cyScreen = GetSystemMetrics (SM_CYSCREEN) ;  // high

    int srcw = cxScreen;
    int srch = cyScreen;
    int desw = srcw;
    int desh = srch;
    int fps = 25;
    int bitrate = 2048;
    int maxpFrame = 5;
    int speed = 1;
    int op = 20; //1是无损压缩
    int up = 2; // 0 1 2是动态码率
    int sleepTime = 1000/fps - 4;
    //qDebug()<<"size == "<<w<<h;
    uint8_t *YUV_BUF_420[4];
    int linesizes[4];

    uint8_t *RGB32_BUF[4];
    int rgbLinesize[4];

    qCDebug(logScreenCapture) << "called";


    QFile file("aaaaaaaaaaaaaaaaaaaaa.dat");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        //QSize deskTopSize = QGuiApplication::primaryScreen()->virtualSize();

        qCDebug(logScreenCapture)<<desw<<desh<<fps<<bitrate<<maxpFrame<<speed;

        av_image_alloc(RGB32_BUF,rgbLinesize,srcw,srch,AV_PIX_FMT_RGB32,1);

        av_image_alloc(YUV_BUF_420,linesizes,desw,desh,AV_PIX_FMT_YUV420P,1);


        SwsContext *m_pImageConvertCtx = sws_getContext(srcw,srch,AV_PIX_FMT_RGB32,
                                                desw,desh,AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);

       //free(YUV_BUF_420[0]);
       //YUV_BUF_420[0] = (uint8_t *)malloc(w*h*1.8);
       unsigned char *yuvBuf = (uint8_t *)malloc(desw*desh*1.5);
       int yuvSize = desw*desh*1.5;
       int ySize = desw*desh;
       int uSize = ySize/4;
       unsigned char * m_264Buf = NULL; //
       uint8_t *temp_rgb32_buf = (uint8_t *)malloc(srcw*srch*4);

       H264Encoder *m_pEncoder = new H264Encoder();
       //w h 一秒多少帧、俩个i帧之间最多有几个p帧、码率越大图形质量越好、编码速度1最快、无损压缩、2是很定码率
       m_pEncoder->Init(desw,desh, fps, maxpFrame, bitrate, speed, op, up);
       qDebug()<<"init over";
//       m_pEncoder->Init(1920,1080, 25, 5, 2048, 1, 20, 0);  最优
       // ---------------------------------------------------------------------------------------------------------------------------------------

       //初始化Pusher结构信息
       uchar  sps[100];
       uchar  pps[100];
       long spslen=0;
       long ppslen=0;
       m_pEncoder->GetSPSAndPPS(sps,spslen,pps,ppslen);


       //int m_times = 0;
       HDC m_srcHDC = GetDC(GetDesktopWindow());

       //QScreen *screen = QGuiApplication::primaryScreen();

       HDC hdcDest = CreateCompatibleDC(m_srcHDC);
       HBITMAP hBitmap = CreateCompatibleBitmap(m_srcHDC, srcw, srch);

       // select the bitmap object
       SelectObject(hdcDest, hBitmap);


       while(!this->isInterruptionRequested()){


           QTime time = QTime::currentTime();

           // bitblt over
           BitBlt(hdcDest, 0, 0, srcw, srch, m_srcHDC, 0, 0, SRCCOPY);
           // restore selection

           // clean up
           //DeleteDC(hdcDest);
           GetBitmapBits(hBitmap,srcw*srch*4,temp_rgb32_buf);

           RGB32_BUF[0] = (uint8_t *)temp_rgb32_buf;//
           sws_scale(m_pImageConvertCtx,RGB32_BUF,rgbLinesize,0,srch,
                     YUV_BUF_420,linesizes);

           //送去编码
           int datasize=0;
           bool keyframe=false;
           memcpy(yuvBuf,YUV_BUF_420[0],ySize);
           memcpy(yuvBuf+ySize,YUV_BUF_420[1],uSize);
           memcpy(yuvBuf+ySize+uSize,YUV_BUF_420[2],uSize);

           m_264Buf = m_pEncoder->Encoder(yuvBuf, yuvSize, datasize, keyframe);


           //qDebug()<<"time =="<<time.msecsTo(QTime::currentTime());
           if(!m_264Buf){
               qCWarning(logScreenCapture()) <<"编码失败";
               continue;
           }

           //rtp_h264_video_tx(this,m_264Buf,datasize,m_times);

           file.write((const char *)m_264Buf, datasize);


           //m_times+=3600;

           //msleep(sleepTime);
        }
        file.close();
    } else {
        qCCritical(logScreenCapture) << "open file failed";
    }

    qCDebug(logScreenCapture) << "done";
}
