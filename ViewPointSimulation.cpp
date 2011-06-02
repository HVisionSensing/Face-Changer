#include "ViewPointSimulation.h"


// TODO: need to be fixed
double gaussian(double sigma) {
  double mean = 0.2f;
  CvRNG rng = cvRNG(cv::getTickCount());
  IplImage *img = cvCreateImage(cvSize(1, 1), IPL_DEPTH_8U, 3);
  cvRandArr(&rng, img, CV_RAND_NORMAL, cvScalar(mean), cvScalar(sigma));
  return (double)img->imageData[0];
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

		last_sim_pt.x = out_x = (int) (bias_pt.x + gaussian(m_sigma));
		last_sim_pt.y = out_y = (int) (bias_pt.y + gaussian(m_sigma));
		last_pt.x = in_x;
		last_pt.y = in_y;
	}
	else {
		out_x = last_sim_pt.x;
		out_y = last_sim_pt.y;
	}


	if (out_x < -origin_pt.x || out_x >= screen_width - origin_pt.x ||
		out_y < -origin_pt.y || out_y >= screen_height - origin_pt.y) {
		return -1;
	}


	return (out_y + origin_pt.y) / (m_PixelHeight + 2 * m_PixelHalfBlack) * m_ColNum + (out_x + origin_pt.x) / (m_PixelWidth + 2 * m_PixelHalfBlack);
}

void ViewPointSimulation::Reset()
{
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
