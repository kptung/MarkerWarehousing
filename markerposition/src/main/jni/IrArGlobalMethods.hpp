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

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define IR_LIB_VERSION 1.0.0
#define JNI_DBG 0
#define LOG_TAG "IrMarkerLib"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif


#ifdef _WIN32
#include "win32\dirent.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

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

template <typename T>
std::string ToString(const T& value)
{
	std::ostringstream stream;
	stream << value;
	return stream.str();
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

#endif
