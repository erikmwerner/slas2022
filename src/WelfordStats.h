
/*!
* SLAS 2019 IoT
*/

#ifndef __WELFORDSTATS__H__
#define __WELFORDSTATS__H__

class WelfordStats
{
	double _mean = 0;
	double _var = 0;
	double _min = 0; //< replace with max value of double
	double _max = 0; //< replace with min value of double
	int _count = 0;

public:
	double mean() {return _mean;}
	double variance() {return (_count > 1) ? _var / (_count - 1) : 0;}
	// double stdDev() const {return (_count > 1) ? sqrt(_var / (_count - 1)) : 0;}
	//public double stdErr() const {return (_count > 0) ? stdDev()/sqrt(_count ) : 0;}
	double minimum() {return _min;}
	double maximum() {return _max;}
	double count() {return _count;}
	
	WelfordStats()
	{
		reset();
	}

	void reset()
	{
		double _mean = 0;
		double _var = 0;
		double _min = 0;
		double _max = 0;
		int _count = 0;
	}

	void update(const double value)
	{
		++_count;
		if(_count == 1)
		{
			_mean = value;
			_var = 0;
		}
		else
		{
			double delta = value - _mean;
			_mean += delta / (double)_count;
			_var += delta * (value - _mean);
		}
		
		if(value < _min)
		{
			_min = value;
		}
		if(value > _max)
		{
			_max = value;
		}
	}
};


#endif //End __WELFORDSTATS__H__ include guard