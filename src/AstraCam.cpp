// This file is part of the Orbbec Astra SDK [https://orbbec3d.com]
// Copyright (c) 2015-2017 Orbbec 3D
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Be excellent to each other.
#include "../inc/AstraStream/BodyVisualizer.h"
#include "../inc/SFML/sfPen.h"
#include "FaceRecognition/std_includes.h"
#include "FaceRecognition/FaceGrabber.h"


astra::DepthStream configure_depth(astra::StreamReader& reader)
{
	auto depthStream = reader.stream<astra::DepthStream>();

	//We don't have to set the mode to start the stream, but if you want to here is how:
	astra::ImageStreamMode depthMode;

	depthMode.set_width(640);
	depthMode.set_height(480);
	depthMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
	depthMode.set_fps(30);

	depthStream.set_mode(depthMode);

	return depthStream;
}

void astra_init()
{

}



int main(int argc, char** argv)
{

	//初始化相机库
	astra::initialize();

	const char* licenseString = "<INSERT LICENSE KEY HERE>";
	orbbec_body_tracking_set_license(licenseString);

	//生成输出窗口，估计跟namedWindow差不多的用法
	//sf::RenderWindow window(sf::VideoMode(1280, 960), "Simple Body Viewer");

#ifdef _WIN32
	auto fullscreenStyle = sf::Style::None;
#else
	auto fullscreenStyle = sf::Style::Fullscreen;
#endif
	//获取全屏模式，由sf库查询显卡和显示器支持的全屏模式，返回一个列表，第一个模式是最好的,然后依次递减
	//const sf::VideoMode fullScreenMode = sf::VideoMode::getFullscreenModes()[0];
	////窗口模式大小
	//const sf::VideoMode windowedMode(1280, 960);
	//bool isFullScreen = false;

	//这个库教程上提示了很浅显易懂的比喻，旧式的电影放映机
	/*
	* 旧式放映机需要两样东西，胶卷和放映机
	* 胶卷就是摄像机的一个个流对象(Stream)
	* 放映机就是StreamReader
	*/

	//流组，这个流组由多个不同的，单独的流对象进行组成，
	//就如同老式胶卷除了电影本身可能还会由如音频等流对象组成。

	astra::StreamSet sensor;
	//生成一个流读取器(放映机)
	astra::StreamReader reader = sensor.create_reader();
	//流处理器，处理生成出来的流
	BodyVisualizer listener;
	//从流读取器中初始化一个深度流对象
	auto depthStream = configure_depth(reader);
	//深度流开始录制
	depthStream.start();
	//初始化一个身体流对象(BodyStream)
	auto bodyStream = reader.stream<astra::BodyStream>();

	const char* helpMessage =
		"keyboard shortcut:\n"
		"D      use 640x400 depth resolution\n"
		"F      toggle between fullscreen and windowed mode\n"
		"H      show/hide this message\n"
		"M      enable/disable depth mirroring\n"
		"P      enable/disable drawing texture\n"
		"S      toggle SkeletonProfile(Full or Basic)\n"
		"T      toggle BodyTrackingFeatureFlags(HandPoses, Segmentation or Joints\n"
		"SPACE  show/hide all text\n"
		"Esc    exit";
	listener.set_help_message(helpMessage);
	//开始获取身体流
	bodyStream.start();
	//将放映机reader与(处理者)listener结合在一起, 给Listener的爹绑定一个Reader
	reader.add_listener(listener);
	//获取骨架数据
	astra::SkeletonProfile profile = bodyStream.get_skeleton_profile();

	//这是一个枚举，说明当前我想获取的是手势位置
	// HandPoses includes Joints and Segmentation
	astra::BodyTrackingFeatureFlags features = astra::BodyTrackingFeatureFlags::HandPoses;

	FaceGrabber fg;
	fg.StarGrab();

	listener.bheight_ready_ = true;
	std::cout << "give me a flag.txt" << std::endl;
	while (true)
	{
		astra_update();//让驱动更新当前帧

		fg.GetFrame();

		FILE* fp = NULL;
		fp = fopen("flag.txt", "r");
		if (fp)
		{
			listener.bheight_ready_ = false;
			fclose(fp);
			remove("flag.txt");//清除
		}
		if (listener.bheight_ready_ && !listener.height_vec_.empty()) //有身高
		{
			while (!fg.GetFace())
			{
				fg.GetFrame();
			}
			if (fg.GetFace()) //处理脸部
			{
				fg.CleanDisk();
				fg.ShowROIFace();
				cv::waitKey(1);
				fg.WritePic2Disk();
			}
			//写身高
			fp = fopen("height.txt", "w+");
			if (fp == NULL)
				return -1;
			fprintf(fp, "%lf\n", listener.GetHeight());
			fclose(fp);
		}

	}

	astra::terminate();

	return 0;
}
