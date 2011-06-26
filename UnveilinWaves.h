#ifndef _UNVEILINWAVES_H_
#define _UNVEILINWAVES_H_


class UnveilinWaves
{
  public:
    UnveilinWaves();
    UnveilinWaves(const IplImage *img1, const IplImage *img2);
    ~UnveilinWaves();
    void Reset();
    int SetImage(const IplImage *img1, const IplImage *img2);
    void PutDrop(int x, int y, short height);
    int ProcessWaves(IplImage *showImg);
    int _weHaveWaves; //deprecated
	int _weHaveRotates; //deprecated
    int _weHaveEffects;
    int ProcessHoughLines(IplImage *showImg);
    int ProcessRotates(IplImage *showImg);
    int ProcessNegate(IplImage *showImg);
    int ProcessSmooth(IplImage *showImg, int smoothType=CV_GAUSSIAN);
    int ProcessDilate(IplImage *showImg);
    int ProcessErode(IplImage *showImg);
    int ProcessMorph(IplImage *showImg, int morphType=CV_MOP_TOPHAT);

  private:
    IplImage *rotateImage(const IplImage *src, float angleDegrees);
    int adjustImage(IplImage* src, IplImage* dst, double low, double high, double bottom, double top, double gamma );
    bool isPreProcessed;


  protected:
    bool isBackUp;
    int times;
	double m_angle;
    IplImage *ori;
    bool isLoaded;
    int m_height;
    int m_width;
    IplImage *m_img[2];
    short*** _waves;
    int _waveWidth;
    int _waveHeight;
    int _activeBuffer;
    int _scale;
	int _angle;
    uchar* tmpeBytes;
};

#endif /* _UNVEILINWAVES_H_ */
