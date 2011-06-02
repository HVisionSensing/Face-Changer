#ifndef _VIEWPOINTSIMULATION_H_
#define _VIEWPOINTSIMULATION_H_


#include "opencvheader.h"


class ViewPointSimulation
{
private:
	int m_start;
  cv::RNG rng;			// Random number generator 
	double m_sigma;
	int64 last_rng_time;	// Last generate random number time
	CvPoint origin_pt;		// The origin point
	CvPoint last_pt;		// Last real point
	CvPoint last_sim_pt;	// Last simulation point
	CvPoint bias_pt;
	int m_PixelHeight;
	int m_PixelWidth;
	int m_PixelHalfBlack;
	int m_RowNum;
	int m_ColNum;

public:
	ViewPointSimulation();
	~ViewPointSimulation() {};

	int SetParameter(const int origin_x, const int origin_y, const double sigma, const int PixelHeight, const int PixelWidth, const int HalfIntervalDist, const int RowNum, const int ColNum);
	int Simulation(const CvPoint in_pt, CvPoint &out_pt);
	int Simulation(const int in_x, const int in_y, int &out_x, int &out_y);
	void Reset();
};

#endif /* _VIEWPOINTSIMULATION_H_ */
