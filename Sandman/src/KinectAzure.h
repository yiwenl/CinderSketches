#pragma once

#include "k4a/k4a.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

namespace kinect {
	typedef std::shared_ptr<struct KinectAzure> KinectAzureRef;
    

	// public methods
	int			getDeviceCount();
	std::string getSerialNumber(k4a_device_t device);
	void		listAllDevices();

	// point cloud utils
	static void		create_xy_table(const k4a_calibration_t* calibration, k4a_image_t xy_table);
	static void		generate_point_cloud(const k4a_image_t depth_image, const k4a_image_t xy_table, k4a_image_t point_cloud, int* point_count);


    // texture utils
    template <typename T>
    void updateTexture(gl::TextureRef& tex, const T& src, const gl::Texture2d::Format& format);


	// Kinect settings
	struct Option {
		int deviceId = 0;
		bool enableColor = false;
		bool enableDepth = true;
		bool enablePointCloud = false;
		bool use720p = false;
		int depthMode = 0;
		int exposure = 0;
		int whiteBalance = 0;
	};

	struct KinectAzure
	{
        Option          option;
        k4a_device_t    device_handle = 0;
        std::string     serialNumber;
        ivec3           colorSize, depthSize;

        k4a_calibration_t       calibration;
        k4a_transformation_t    transformation;
        k4a_image_t transformed_depth_image = NULL;
        
        // thread for camera feed update
        std::shared_ptr<std::thread>  mThread;

        // cinder surface & textures
        Surface8u           colorSurface;
        Channel16u          depthTransformedChannel;
        gl::Texture2dRef    texColor;
        gl::Texture2dRef    texDepth;
        bool updateGLTexture = true;


        // status
        bool isColorDirty   = false;
        bool isDepthDirty   = false;
        bool isPointsReady  = false;
        bool isPointsDirty  = true;

        // for threading to stop
        bool isRunning      = true;

        // point cloud
        k4a_image_t         xy_table = NULL;
        k4a_image_t         point_cloud = NULL;
        k4a_float3_t*       point_cloud_data;
        int point_count     = 0;
        int numberOfPoints  = 0;


        // methods

		KinectAzure(kinect::Option);
		~KinectAzure();

		void updateCamera();
		void update();

        bool isColorReady();
        bool isDepthReady();
        bool isReady();
	};

    KinectAzureRef create(Option option);
}


