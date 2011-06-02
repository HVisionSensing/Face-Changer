#define _USE_MATH_DEFINES

#include <math.h>

#include "opencvheader.h"
#include "UnveilinWaves.h"


UnveilinWaves::UnveilinWaves()
{
	m_height = 0;
	m_width = 0;
	m_img[0] = NULL;
	m_img[1] = NULL;
//chunku 0820 12:12
	_activeBuffer = 0;
	_scale=1;
	//chunku=================

	isLoaded = false;
}

UnveilinWaves::UnveilinWaves(const IplImage *img1, const IplImage *img2)
{
	m_img[0] = NULL;
	m_img[1] = NULL;
	//chunku 0820 12:12
	_activeBuffer = 0;
	_scale=1;
	//chunku=================
	if (SetImage(img1, img2)) {
		m_height = 0;
		m_width = 0;

		isLoaded = false;
	}

}

UnveilinWaves::~UnveilinWaves()
{
	Reset();
}

void UnveilinWaves::Reset()
{
	if (!isLoaded) {
		return;
	}

	if (m_img[0] != NULL) {
		cvReleaseImage(&m_img[0]);
	}
	if (m_img[1] != NULL) {
		cvReleaseImage(&m_img[1]);
	}
	m_height = 0;
	m_width = 0;
	m_img[0] = NULL;
	m_img[1] = NULL;
	//chunku 0820 12:12
	_activeBuffer = 0;
	_scale=1;
	//chunku=================
	isLoaded = false;
}

int UnveilinWaves::SetImage(const IplImage *img1, const IplImage *img2)
{
	if (img1 == NULL || img2 == NULL) {
		return 1;
	}

	if (isLoaded) {
		Reset();
	}

	m_height = img1->height;
	m_width = img1->width;
	m_img[0] = cvCloneImage(img1);
	m_img[1] = cvCreateImage( cvSize(m_width, m_height), IPL_DEPTH_8U, 3);
	cvResize(img2, m_img[1], CV_INTER_CUBIC);
	/*  chunku 0820 12:11 */
	
	_waveWidth = m_width >> _scale;
	_waveHeight = m_height >> _scale;
	_waves = new short** [_waveWidth];
	for(int i=0; i<_waveWidth; i++){
		_waves[i] = new short*[_waveHeight];
		for(int j=0; j<_waveHeight; j++){
			_waves[i][j] = new short[2];
			_waves[i][j][0]= 0;
			_waves[i][j][1]= 0;
		}
	}
	/*-----三維不能直接 new----------*/
	isLoaded = true;

	return 0;
}

void UnveilinWaves::PutDrop(int x, int y, short height){
	_weHaveWaves = 1;
	int radius = 10;
	double dist;

	for (int i = -radius; i <= radius; i++){
                for (int j = -radius; j <= radius; j++){
                    if (((x + i >= 0) && (x + i < _waveWidth - 1)) && ((y + j >= 0) && (y + j < _waveHeight - 1))){
                        dist = sqrt((double)((i*i)+ (j*j)));
                        if (dist < radius)
                            _waves[x + i][y + j][_activeBuffer] = (short)(cos(dist * M_PI / radius) * height);
                    }
                }
     }

}

int UnveilinWaves::ProcessWaves(IplImage *showImg)
{
	int newBuffer = (_activeBuffer == 0) ? 1 : 0;
	int wavesFound = 0;
	int xOffset, yOffset, x, y;
	uchar *src2, *dst;


	if (!isLoaded) {
		return 1;
	}
	if (showImg == NULL || showImg->height != m_height || showImg->width != m_width) {
		return 1;
	}

//			printf("ProcessWaves!!!\n");
            for (x = 1; x < _waveWidth - 1; x++)
            {
                for (y = 1; y < _waveHeight - 1; y++)
                {
                    _waves[x][y][newBuffer] = (short)(
                                            ((_waves[x - 1][y - 1][_activeBuffer] +
                                            _waves[x][y - 1][_activeBuffer] +
                                            _waves[x + 1][y - 1][_activeBuffer] +
                                            _waves[x - 1][y][_activeBuffer] +
                                            _waves[x + 1][y][_activeBuffer] +
                                            _waves[x - 1][y + 1][_activeBuffer] +
                                            _waves[x][y + 1][_activeBuffer] +
                                            _waves[x + 1][y + 1][_activeBuffer]) >> 2) - _waves[x][y][newBuffer]);  //如果在這邊把(x,y)的newBuffer改成_activeBuffer呢??會壞掉喔!!

                    //damping   看起來是說  如果還再波的話 要衰減他
                    if (_waves[x][y][newBuffer] != 0)
                    {
                        _waves[x][y][newBuffer] -= (short)(_waves[x][y][newBuffer] >> 4);   //因為一直疊加 會使得_waves一直變大  所以應該要有能量的衰減 這就是衰減率
                        wavesFound = 1;

                    }

                }
            }
			
			for (x = 0; x < m_width; x++)
            {
                for (y = 0; y < m_height; y++)
                {
                    int waveX = (int) x >> _scale;
                    int waveY = (int) y >> _scale;

                    //check bounds
                    if (waveX <= 0) waveX = 1;
                    if (waveY <= 0) waveY = 1;
                    if (waveX >= _waveWidth - 1) waveX = _waveWidth - 2;
                    if (waveY >= _waveHeight - 1) waveY = _waveHeight - 2;

                    //this gives us the effect of water breaking the light
                    xOffset = (_waves[waveX - 1][waveY][_activeBuffer] - _waves[waveX + 1][waveY][_activeBuffer]) >> 3;
                    yOffset = (_waves[waveX][waveY - 1][_activeBuffer] - _waves[waveX][waveY + 1][_activeBuffer]) >> 3;

                    if ((xOffset != 0) || (yOffset != 0))
                    {
                        //check bounds
                        if (x + xOffset >= m_width - 1) xOffset = m_width - x - 1;
                        if (y + yOffset >= m_height - 1) yOffset = m_height - y - 1;
                        if (x + xOffset < 0) xOffset = -x;
                        if (y + yOffset < 0) yOffset = -y;

                        //generate alpha
                        /*
                        alpha = (byte)(200 - xOffset);
                        if (alpha < 0) alpha = 0;
                        if (alpha > 255) alpha = 254;
                        */
                        //set colors
						/*
                        tmpBytes[3 * (x + y * _bmpWidth)] = _bmpBytes2[3 * (x + xOffset + (y + yOffset) * _bmpWidth)];
                        tmpBytes[3 * (x + y * _bmpWidth) + 1] = _bmpBytes2[3 * (x + xOffset + (y + yOffset) * _bmpWidth) + 1];
                        tmpBytes[3 * (x + y * _bmpWidth) + 2] = _bmpBytes2[3 * (x + xOffset + (y + yOffset) * _bmpWidth) + 2];
                        */
						//tmpBytes[4 * (x + y * _bmpWidth) + 3] = alpha;

						//寫入dst
						//src1 = &CV_IMAGE_ELEM(m_img[0], uchar,  y+yOffset, (x+xOffset)* 3);
						src2 = &CV_IMAGE_ELEM(m_img[1], uchar, y + yOffset, (x+xOffset) * 3);
						dst = &CV_IMAGE_ELEM(showImg, uchar, y, x * 3);

						dst[0] = src2[0];
						dst[1] = src2[1];
						dst[2] = src2[2];
                    }

                }
            }
			_weHaveWaves = wavesFound;
			_activeBuffer = newBuffer;
//			printf("_weHaveWaves = %d\n", _weHaveWaves);
//			printf("_iplimage origin=%d\n", showImg->origin);

	return 0;
}