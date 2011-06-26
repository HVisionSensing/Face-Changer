#include "ViewPointSimulation.h"



// TODO: need to be fixed
double gaussian(double sigma) {
    /*
  double mean = 0.2f;
  CvRNG rng = cvRNG(cv::getTickCount());
  IplImage *img = cvCreateImage(cvSize(1, 1), IPL_DEPTH_8U, 3);
  cvRandArr(&rng, img, CV_RAND_NORMAL, cvScalar(mean), cvScalar(sigma));
  return (double)img->imageData[0];*/
  srand(time(NULL));
  return (double)(rand()%1000+1)/1000*sigma;
}


ViewPointSimulation::ViewPointSimulation()
{
	Reset();
};

int ViewPointSimulation::SetParameter(const int origin_x, const int origin_y, const double sigma, const int PixelHeight, const int PixelWidth, const int HalfIntervalDist, const int RowNum, const int ColNum)
{
	origin_pt.x = origin_x;
	origin_pt.y = origin_y;
	m_sigma = sigma;
	m_PixelHeight = PixelHeight;
	m_PixelWidth = PixelWidth;
	m_PixelHalfBlack = HalfIntervalDist;
	m_RowNum = RowNum;
	m_ColNum = ColNum;
	m_start = 0;

	return 0;
}

int ViewPointSimulation::Simulation(const CvPoint in_pt, CvPoint &out_pt)
{
	return Simulation(in_pt.x, in_pt.y, out_pt.x, out_pt.y);
}

int ViewPointSimulation::Simulation(const int in_x, const int in_y,int &out_x, int &out_y)
{
	int64 now_time = cv::getTickCount();
	int screen_width = (m_PixelWidth + 2 * m_PixelHalfBlack) * m_ColNum;
	int screen_height = (m_PixelHeight + 2 * m_PixelHalfBlack) * m_RowNum;


	if (m_PixelHeight <= 0 ||
		m_PixelWidth <= 0 ||
		m_RowNum <= 0 ||
		m_ColNum <= 0) {
		return -1;
	}

	// Initialize
	if (m_start == 0) {
		last_pt.x = in_x;
		last_pt.y = in_y;
		bias_pt.x = (int) ((gaussian(0.25f) + 0.7) * in_x);
		bias_pt.y = (int) ((gaussian(0.25f) + 0.7) * in_y);
		m_start = 1;
	}

	// Generating simulation point
	if (now_time - last_rng_time > cv::getTickFrequency() / 15) {		// 15 fps
		last_rng_time = now_time;

		if ((in_x > 0 && last_pt.x > 0) ||
			(in_x < 0 && last_pt.x < 0)) {
			bias_pt.x += (int) ((gaussian(0.25f) + 0.7) * (in_x - last_pt.x));
		}
		else {
			bias_pt.x = (int) ((gaussian(0.25f) + 0.7) * in_x);
		}

		if ((in_y > 0 && last_pt.y > 0) ||
			(in_y < 0 && last_pt.y < 0)) {
			bias_pt.y += (int) ((gaussian(0.25f) + 0.7) * (in_y - last_pt.y));
		}
		else {
			bias_pt.y = (int) ((gaussian(0.25f) + 0.7) * in_y);
		}
        int t_x = (int) (bias_pt.x + gaussian(m_sigma));
        int t_y = (int) (bias_pt.y + gaussian(m_sigma));
        if(t_x - last_sim_pt.x != 0){
            speed[0] += (t_x - last_sim_pt.x);
            interval = 4;
        }
        if(t_y - last_sim_pt.y != 0){
            speed[1] += (t_y - last_sim_pt.y);
            interval = 4;
        }
		if(interval){
            if(speed[0]>0){
                t_x += speed[0]/interval ? speed[0]/interval : 1;
                speed[0] -= speed[0]/interval ? speed[0]/interval : 1;
            }
            else if(speed[0]<0){
                t_x += speed[0]/interval ? speed[0]/interval : -1;
                speed[0] -= speed[0]/interval ? speed[0]/interval : -1;
            }
            if(speed[1]>0){
                t_y += speed[1]/interval ? speed[1]/interval : 1;
                speed[1] -= speed[1]/interval ? speed[1]/interval : 1;
            }
            else if(speed[1]<0){
                t_y += speed[1]/interval ? speed[1]/interval : -1;
                speed[1] -= speed[0]/interval ? speed[1]/interval : -1;
            }
            interval -= 1;
        }
        if(t_y >= screen_height - origin_pt.y)t_y = screen_height - origin_pt.y - 1;
        else if(t_y < -origin_pt.y)t_y = -origin_pt.y;
        if(t_x >= screen_width - origin_pt.x)t_x = screen_width - origin_pt.x - 1;
        else if(t_x < -origin_pt.x)t_x = -origin_pt.x;
        
        last_sim_pt.x = out_x = t_x;
		last_sim_pt.y = out_y = t_y;
        
		last_pt.x = in_x;
		last_pt.y = in_y;
	}
	else {
        //speed[0] = t_x - last_sim_pt.x;
        //speed[1] = t_y - last_sim_pt.y;
		int t_x = last_sim_pt.x;
        int t_y = last_sim_pt.y;
        if(interval){
            if(speed[0]>0){
                t_x += speed[0]/interval ? speed[0]/interval : 1;
                speed[0] -= speed[0]/interval ? speed[0]/interval : 1;
            }
            else if(speed[0]<0){
                t_x += speed[0]/interval ? speed[0]/interval : -1;
                speed[0] -= speed[0]/interval ? speed[0]/interval : -1;
            }
            if(speed[1]>0){
                t_y += speed[1]/interval ? speed[1]/interval : 1;
                speed[1] -= speed[1]/interval ? speed[1]/interval : 1;
            }
            else if(speed[1]<0){
                t_y += speed[1]/interval ? speed[1]/interval : -1;
                speed[1] -= speed[0]/interval ? speed[1]/interval : -1;
            }
            interval-=1;
        }
		out_x = t_x;
		out_y = t_y;
	}
    if(out_y >= screen_height - origin_pt.y)last_sim_pt.y = out_y = screen_height - origin_pt.y - 1;
    else if(out_y < -origin_pt.y)last_sim_pt.y = out_y = -origin_pt.y;
    if(out_x >= screen_width - origin_pt.x)last_sim_pt.x = out_x = screen_width - origin_pt.x - 1;
    else if(out_x < -origin_pt.x)last_sim_pt.x = out_x = -origin_pt.x;

	/*if (out_x < -origin_pt.x || out_x >= screen_width - origin_pt.x ||
		out_y < -origin_pt.y || out_y >= screen_height - origin_pt.y) {
		return -1;
	}*/


	return (out_y + origin_pt.y) / (m_PixelHeight + 2 * m_PixelHalfBlack) * m_ColNum + (out_x + origin_pt.x) / (m_PixelWidth + 2 * m_PixelHalfBlack);
}

void ViewPointSimulation::Reset()
{
    speed[0] = 0;
    speed[1] = 0;
	m_start = 0;
	m_sigma = 1.0f;
	last_rng_time = 0;
	last_pt.x = 0;
	last_pt.y = 0;
	last_sim_pt.x = 0;
	last_sim_pt.y = 0;
	m_PixelHeight = 0;
	m_PixelWidth = 0;
	m_PixelHalfBlack = 0;
	m_RowNum = 0;
	m_ColNum = 0;
	origin_pt.x = 0;
	origin_pt.y = 0;
}
