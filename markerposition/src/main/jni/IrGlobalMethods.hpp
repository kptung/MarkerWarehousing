#ifndef __IrGlobalMethods_hpp__
#define __IrGlobalMethods_hpp__

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip> 

#ifndef SEP
#define SEP (std::string("/"))
#endif
#ifndef TMP
#define TMP (std::string("tmp"))
#endif
#ifndef JPG
#define JPG (std::string(".jpg"))
#endif
#ifndef _RAD2DEG_
#define _RAD2DEG_ (57.2957795786)
#endif

namespace Ir
{
	template <class T>
	void sortQuadCorners( std::vector<T> &corners )
	{
		// Get mass center
		T center(0, 0);
		size_t i, corners_size = corners.size();
		for (i = 0; i < corners_size; i++)
		   center += corners[i];

		std::vector<T> top, bot;
		for (i = 0; i < corners_size; i++)
		{
			if (corners_size*corners[i].y < center.y)
				top.push_back(corners[i]);
			else
				bot.push_back(corners[i]);
		}

		if ( top.size() == bot.size() )
		{
			corners.clear();

			T tl = top[0].x > top[1].x ? top[1] : top[0];
			T tr = top[0].x > top[1].x ? top[0] : top[1];
			T bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
			T br = bot[0].x > bot[1].x ? bot[0] : bot[1];

			corners.push_back(tl);
			corners.push_back(tr);
			corners.push_back(br);
			corners.push_back(bl);

			return;
		}

		std::vector<T> left, right;
		for (i = 0; i < corners_size; i++)
		{
			if (corners_size*corners[i].x < center.x)
				left.push_back(corners[i]);
			else
				right.push_back(corners[i]);
		}

		if (left.size() == right.size())
		{
			corners.clear();

			T tl = left[0].y > left[1].y ? left[1] : left[0];
			T bl = left[0].y > left[1].y ? left[0] : left[1];
			T tr = right[0].y > right[1].y ? right[1] : right[0];
			T br = right[0].y > right[1].y ? right[0] : right[1];

			corners.push_back(tl);
			corners.push_back(tr);
			corners.push_back(br);
			corners.push_back(bl);

			return;
		}
	}




}

template <typename T>
std::string ToString(const T& value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}


template <typename T>
class make_vector
{
public:
	typedef make_vector<T> my_type;
	my_type& operator<< (const T& val)
	{
		data_.push_back(val);
		return *this;
	}
	operator std::vector<T>() const
	{
		return data_;
	}

private:
	std::vector<T> data_;
};


namespace cv
{
    inline void showAndSave(std::string name, const cv::Mat& m)
    {
        cv::imshow(name, m);
        cv::imwrite(name + ".png", m);
    }

	inline void rotateAnImage(cv::Mat& src, double angle, cv::Mat& dst)
	{
		int len = (std::max)(src.cols, src.rows);
		cv::Point2f pt((float)len/2.0f, (float)len/2.0f);
		cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
		cv::warpAffine(src, dst, r, cv::Size(len, len));
	}

	static cv::RNG rng( 0xFFFFFFFF );
	inline cv::Scalar randomColor( void )
	{
	  int icolor = (unsigned) rng;
	  return cv::Scalar( icolor&255, (icolor>>8)&255, (icolor>>16)&255 );
	}
}




#ifdef _WIN32
#include "win32\dirent.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif


inline bool read_line_by_line(const std::string &filename, std::vector<std::string> &lines)
{
	lines.clear();

	std::fstream fin;

	fin.open(filename.c_str(), std::ios::in);

	if( !fin )
	{
		std::cerr << "Can't open file: " << filename << std::endl;
		return false;
	}

	std::string str;

	while( getline(fin,str) )
	{
		lines.push_back( std::string(str) );
	}

	fin.close();

	return true;
}


/* Returns a list of files in a directory (except the ones that begin with a dot) */
inline void get_files_in_directory(const std::string &directory, std::vector<std::string> &out)
{
    DIR *dir;
    struct dirent *ent;
    struct stat st;

    dir = opendir( directory.c_str() );

	if ( dir == NULL )
	{
		std::cerr << "Can't find folder: " << directory << std::endl;
		return;
	}

    while ( (ent = readdir(dir)) != NULL )
	{
        const std::string file_name = ent->d_name;
        const std::string full_file_name = directory + "/" + file_name;

        if (file_name[0] == '.')
            continue;

        if (stat(full_file_name.c_str(), &st) == -1)
            continue;

        const bool is_directory = (st.st_mode & S_IFDIR) != 0;

        if (is_directory)
            continue;

		out.push_back(file_name);
    }

    closedir(dir);
}

/// intersection of polygons

inline bool segementIntersection(
	const cv::Point2f& p0,
	const cv::Point2f& p1,
	const cv::Point2f& p2,
	const cv::Point2f& p3,
	cv::Point2f& p
)
{
	float s10_x = p1.x - p0.x;
	float s10_y = p1.y - p0.y;
	float s32_x = p3.x - p2.x;
	float s32_y = p3.y - p2.y;
	float denom = s10_x * s32_y - s32_x * s10_y;

	if (std::fabs(denom) < 1.0e-6)
	{
		return false;
	}

	bool denom_positive = denom > 0.0f;

	float s02_x = p0.x - p2.x;
	float s02_y = p0.y - p2.y;
	float s_numer = s10_x * s02_y - s10_y * s02_x;

	if ((s_numer < 0.f) == denom_positive)
	{
		return false;
	}

	float t_numer = s32_x * s02_y - s32_y * s02_x;
	if ((t_numer < 0.0f) == denom_positive)
	{
		return false;
	}

	if ((s_numer > denom) == denom_positive || (t_numer > denom) == denom_positive)
	{
		return false;
	}

	float t = t_numer / denom;

	p.x = p0.x + (t * s10_x);
	p.y = p0.y + (t * s10_y);

	return true;
}


inline bool pointInPolygon(
	const cv::Point2f& p,
	const std::vector<cv::Point2f>& points
)
{
	int i, j;
	int n = (int)points.size();
	bool c = false;
	for (i = 0, j = n - 1; i < n; j = i++)
	{
		if (((points[i].y >= p.y) != (points[j].y >= p.y)) &&
			(p.x <= (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
			)

			c = !c;
	}

	return c;
}


inline void intersectPolygon(
	const std::vector<cv::Point2f>& poly0,
	const std::vector<cv::Point2f>& poly1,
	std::vector<cv::Point2f>& inter)
{
	inter.clear();

	int n0 = (int)poly0.size();
	int n1 = (int)poly1.size();

	for (int i = 0; i < n0; i++)
	{
		if (pointInPolygon(poly0[i], poly1))
		{
			inter.push_back(poly0[i]);
		}
	}

	for (int j = 0; j < n1; j++)
	{
		if (pointInPolygon(poly1[j], poly0))
		{
			inter.push_back(poly1[j]);
		}
	}

	for (int i = 0; i < n0; i++)
	{
		for (int j = 0; j < n1; j++)
		{
			cv::Point2f pinter;
			if (segementIntersection(poly0[i], poly0[(i + 1) % n0], poly1[j], poly1[(j + 1) % n1], pinter))
			{
				inter.push_back(pinter);
			}
		}
	}

	if (!inter.empty())
	{
		cv::Point2f center(0.0f, 0.0f);
		int n = (int)inter.size();
		for (int k = 0; k < n; k++)
		{
			center += inter[k];
		}
		center *= 1.0f / (float)n;

		std::vector<std::pair<cv::Point2f, float> > pc(n);
		for (int k = 0; k < n; k++)
		{
			float angle = atan2f((float)(inter[k].y - center.y), (float)(inter[k].x - center.x));
			pc[k] = std::make_pair(inter[k], angle);
		}

		std::sort(pc.begin(), pc.end(), [](auto &left, auto &right) { return left.second < right.second; });

		for (int k = 0; k < n; k++)
		{
			inter[k] = pc[k].first;
		}
	}
}


#endif
