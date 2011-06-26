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
    int _weHaveWaves;
	int _weHaveRotates;
    int ProcessRotates(IplImage *showImg);

  private:
    IplImage *rotateImage(const IplImage *src, float angleDegrees);


  protected:
    bool isBackUp;
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
