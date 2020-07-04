#include "KinectAzure.h"


namespace kinect {
    
	int getDeviceCount() {
		return k4a_device_get_installed_count();
	}

    std:: string getSerialNumber(k4a_device_t device) {
        char* serial_number = NULL;
        size_t serial_number_length = 0;

        if (K4A_BUFFER_RESULT_TOO_SMALL != k4a_device_get_serialnum(device, NULL, &serial_number_length))
        {
            CI_LOG_E("Failed to get serial number length");
            return "Error";
        }

        serial_number = new char[serial_number_length];
        if (serial_number == NULL)
        {
            CI_LOG_E("Failed to allocate memory for serial number");
            return "Error";
        }

        if (K4A_BUFFER_RESULT_SUCCEEDED != k4a_device_get_serialnum(device, serial_number, &serial_number_length))
        {
            CI_LOG_E("Failed to get serial number");
            free(serial_number);
            serial_number = NULL;
            return "Error";
        }

        return std::string(serial_number);
    }

    void listAllDevices() {
        int device_count = getDeviceCount();
        char* serial_number = NULL;
        size_t serial_number_length = 0;
        k4a_device_t device = NULL;

        for (int deviceIndex = 0; deviceIndex < device_count; deviceIndex++)
        {
            if (K4A_RESULT_SUCCEEDED != k4a_device_open(deviceIndex, &device))
            {
                CI_LOG_D("Failed to open device : " << deviceIndex);
                continue;
            }

            CI_LOG_D("Serial Number : " << deviceIndex << ": " << getSerialNumber(device));

            k4a_device_close(device);
        }
    }


    static void create_xy_table(const k4a_calibration_t* calibration, k4a_image_t xy_table)
    {
        k4a_float2_t* table_data = (k4a_float2_t*)(void*)k4a_image_get_buffer(xy_table);

        int width = calibration->depth_camera_calibration.resolution_width;
        int height = calibration->depth_camera_calibration.resolution_height;

        k4a_float2_t p;
        k4a_float3_t ray;
        int valid;
        int count = 0;
        int nonValidCount = 0;

        for (int y = 0, idx = 0; y < height; y++)
        {
            p.xy.y = (float)y;
            for (int x = 0; x < width; x++, idx++)
            {
                p.xy.x = (float)x;

                k4a_calibration_2d_to_3d(
                    calibration, &p, 1.f, K4A_CALIBRATION_TYPE_DEPTH, K4A_CALIBRATION_TYPE_DEPTH, &ray, &valid);

                if (valid)
                {
                    table_data[idx].xy.x = ray.xyz.x;
                    table_data[idx].xy.y = ray.xyz.y;
                }
                else
                {
                    table_data[idx].xy.x = 9999.0f;
                    table_data[idx].xy.y = 9999.0f;
                    nonValidCount++;
                }

            }
        }

    }

    static void generate_point_cloud(const k4a_image_t depth_image, const k4a_image_t xy_table, k4a_image_t point_cloud, int* point_count) {
        int width = k4a_image_get_width_pixels(depth_image);
        int height = k4a_image_get_height_pixels(depth_image);

        uint16_t* depth_data = (uint16_t*)(void*)k4a_image_get_buffer(depth_image);
        k4a_float2_t* xy_table_data = (k4a_float2_t*)(void*)k4a_image_get_buffer(xy_table);
        k4a_float3_t* point_cloud_data = (k4a_float3_t*)(void*)k4a_image_get_buffer(point_cloud);

        *point_count = 0;
        for (int i = 0; i < width * height; i++)
        {
            if (depth_data[i] != 0 && !isnan(xy_table_data[i].xy.x) && !isnan(xy_table_data[i].xy.y))
            {
                point_cloud_data[i].xyz.x = xy_table_data[i].xy.x * (float)depth_data[i];
                point_cloud_data[i].xyz.y = xy_table_data[i].xy.y * (float)depth_data[i];
                point_cloud_data[i].xyz.z = (float)depth_data[i];
            }
            else
            {
                point_cloud_data[i].xyz.x = -9999.0f;
                point_cloud_data[i].xyz.y = -9999.0f;
                point_cloud_data[i].xyz.z = -9999.0f;
            }

            (*point_count)++;
        }

    }

    template <typename T>
    void updateTexture(gl::TextureRef& tex, const T& src, const gl::Texture2d::Format& format = gl::Texture2d::Format())
    {
        if (src.getWidth() == 0) return;

        if (!tex
            || tex->getWidth() != src.getWidth()
            || tex->getHeight() != src.getHeight())
        {
            try {
                tex = gl::Texture2d::create(src, format);
            }
            catch (...) {
                CI_LOG_D("Error creating texture");
            }

        }
        else
        {
            tex->update(src);
        }
    }


	KinectAzure::KinectAzure(Option mOption) {
		option = mOption;

        k4a_result_t result = k4a_device_open(option.deviceId, &device_handle);
        if (result != K4A_RESULT_SUCCEEDED)
            return;

        k4a_device_configuration_t conf = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        if (option.enableColor)
        {
            conf.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
            if (option.use720p) {
                conf.color_resolution = K4A_COLOR_RESOLUTION_720P;
            }
            else {
                conf.color_resolution = K4A_COLOR_RESOLUTION_1080P;
            }
        }

        if (option.enableDepth || option.enablePointCloud)
        {
            if (option.depthMode > 0) {
                conf.depth_mode = (k4a_depth_mode_t)option.depthMode;
            }
            else {
                conf.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
            }

        }

        conf.camera_fps = K4A_FRAMES_PER_SECOND_30;
        conf.depth_delay_off_color_usec = 1;
        conf.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;

        if (option.enableDepth || option.enablePointCloud) {
            if (K4A_RESULT_SUCCEEDED != k4a_device_get_calibration(device_handle, conf.depth_mode, conf.color_resolution, &calibration)) {
                return;
            }
            transformation = k4a_transformation_create(&calibration);

            // point cloud
            k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM,
                calibration.depth_camera_calibration.resolution_width,
                calibration.depth_camera_calibration.resolution_height,
                calibration.depth_camera_calibration.resolution_width * (int)sizeof(k4a_float2_t),
                &xy_table);

            create_xy_table(&calibration, xy_table);

            k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM,
                calibration.depth_camera_calibration.resolution_width,
                calibration.depth_camera_calibration.resolution_height,
                calibration.depth_camera_calibration.resolution_width * (int)sizeof(k4a_float3_t),
                &point_cloud);
        }

        k4a_result_t startResult = k4a_device_start_cameras(device_handle, &conf);
        if (startResult != K4A_RESULT_SUCCEEDED) {
            CI_LOG_D("Error starting device : " << this->option.deviceId);
            return;
        }

        // color control
        if (option.exposure > 0) {
            //CI_LOG_D("Reset color control to manual : " << option.exposure);
            k4a_device_set_color_control(device_handle, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, option.exposure);
        }

        if (option.whiteBalance > 0) {
            //CI_LOG_D("Reset white balance to manual : " << option.whiteBalance);
            k4a_device_set_color_control(device_handle, K4A_COLOR_CONTROL_WHITEBALANCE, K4A_COLOR_CONTROL_MODE_MANUAL, option.whiteBalance);
        }

        serialNumber = getSerialNumber(device_handle);

        // separate thread for camera feed
        using namespace std;
        mThread = shared_ptr<thread>(new thread(bind(&KinectAzure::updateCamera, this)));
        mThread->detach();


        // update loop
        App::get()->getSignalUpdate().connect(std::bind(&KinectAzure::update, this));

        CI_LOG_D("Kinect started : " << this->option.deviceId << ": " << serialNumber);
	}

    void KinectAzure::updateCamera() {
        while (isRunning) {
            const int32_t TIMEOUT_IN_MS = 1000;

            k4a_capture_t capture_handle;
            k4a_wait_result_t waiResult = k4a_device_get_capture(device_handle, &capture_handle, TIMEOUT_IN_MS);

            if (waiResult == K4A_WAIT_RESULT_SUCCEEDED) {
                if (option.enableColor)
                {
                    auto image = k4a_capture_get_color_image(capture_handle);
                    if (image != 0)
                    {
                        if (colorSize.x == 0 || colorSize.y == 0)
                        {
                            colorSize.x = k4a_image_get_width_pixels(image);
                            colorSize.y = k4a_image_get_height_pixels(image);
                            colorSize.z = k4a_image_get_stride_bytes(image);
                        }
                        auto ptr = k4a_image_get_buffer(image);

                        colorSurface = Surface8u(ptr, colorSize.x, colorSize.y, colorSize.z, SurfaceChannelOrder::BGRX);

                        k4a_image_release(image);
                        isColorDirty = true;
                    }
                }

                if (option.enableDepth || option.enablePointCloud)
                {
                    auto image = k4a_capture_get_depth_image(capture_handle);
                    if (image != 0)
                    {
                        //isPointsReady = false;
                        generate_point_cloud(image, xy_table, point_cloud, &point_count);

                        int width = k4a_image_get_width_pixels(point_cloud);
                        int height = k4a_image_get_height_pixels(point_cloud);
                        numberOfPoints = width * height;


                        point_cloud_data = (k4a_float3_t*)(void*)k4a_image_get_buffer(point_cloud);
                        isPointsReady = true;
                        isPointsDirty = true;

                        if (option.enableDepth) {
                            // transform depth image to camera image space
                            if (depthSize.x == 0 || depthSize.y == 0)
                            {
                                depthSize.x = k4a_image_get_width_pixels(image);
                                depthSize.y = k4a_image_get_height_pixels(image);
                                depthSize.z = k4a_image_get_stride_bytes(image);
                            }

                            int color_image_width_pixels = 1920;
                            int color_image_height_pixels = 1080;
                            if (this->option.use720p) {
                                color_image_width_pixels = 1280;
                                color_image_height_pixels = 720;
                            }

                            k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16,
                                color_image_width_pixels,
                                color_image_height_pixels,
                                color_image_width_pixels * (int)sizeof(uint16_t),
                                &transformed_depth_image);

                            // transform depth image space to color camera space
                            k4a_transformation_depth_image_to_color_camera(transformation, image, transformed_depth_image);

                            auto ptrTransformed = (uint16_t*)k4a_image_get_buffer(transformed_depth_image);

                            depthSize.x = k4a_image_get_width_pixels(transformed_depth_image);
                            depthSize.y = k4a_image_get_height_pixels(transformed_depth_image);
                            depthSize.z = k4a_image_get_stride_bytes(transformed_depth_image);

                            if (ptrTransformed != nullptr)
                            {
                                depthTransformedChannel = Channel16u(depthSize.x, depthSize.y, depthSize.z, 1, ptrTransformed);
                                isDepthDirty = true;
                            }
                        }

                        // clear
                        k4a_image_release(image);

                    }

                }

            }

            k4a_capture_release(capture_handle);

        }

    }

    void KinectAzure::update() {

        if (option.enableColor && isColorDirty && colorSize.x != 0) {

            if (updateGLTexture) {
                auto format = gl::Texture::Format()
                    .immutableStorage()
                    .loadTopDown();

                updateTexture(texColor, colorSurface, format);
            }
            isColorDirty = false;
        }

        if (option.enableDepth && isDepthDirty && depthTransformedChannel.getWidth() != 0) {
            if (updateGLTexture) {
                auto format = gl::Texture::Format()
                    .dataType(GL_FLOAT)
                    .immutableStorage()
                    .loadTopDown();
                updateTexture(texDepth, depthTransformedChannel, format);
            }
            k4a_image_release(transformed_depth_image);
            isDepthDirty = false;
        }
    }


    // check if color texture is created
    bool KinectAzure::isColorReady() { return !!texColor; }

    // check if depth texture is created
    bool KinectAzure::isDepthReady() { return !!texDepth; }

    // check if both color & depth are ready
    bool KinectAzure::isReady() {
        return isColorReady() && isDepthReady();
    }


	KinectAzure::~KinectAzure() {
		isRunning = false;
		if (device_handle)
		{
			k4a_device_stop_cameras(device_handle);
			k4a_device_close(device_handle);
		}
	}

    KinectAzureRef create(Option option) { return std::make_shared<KinectAzure>(option); }
	
}