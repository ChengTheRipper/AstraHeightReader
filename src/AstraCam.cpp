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

//我是吴彦祖

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

int main(int argc, char** argv)
{
	//初始化相机库
	astra::initialize();
	//输入序列号文件的地址，读入序列号，然后进行识别
	if (argc == 2)
	{
		FILE* fp = fopen(argv[1], "rb");
		char licenseString[1024] = { 0 };
		fread(licenseString, 1, 1024, fp);
		orbbec_body_tracking_set_license(licenseString);

		fclose(fp);
	}
	else
	{
		const char* licenseString = "<INSERT LICENSE KEY HERE>";
		orbbec_body_tracking_set_license(licenseString);
	}
	//生成输出窗口，估计跟namedWindow差不多的用法
	sf::RenderWindow window(sf::VideoMode(1280, 960), "Simple Body Viewer");

#ifdef _WIN32
	auto fullscreenStyle = sf::Style::None;
#else
	auto fullscreenStyle = sf::Style::Fullscreen;
#endif
	//获取全屏模式，由sf库查询显卡和显示器支持的全屏模式，返回一个列表，第一个模式是最好的,然后依次递减
	const sf::VideoMode fullScreenMode = sf::VideoMode::getFullscreenModes()[0];
	//窗口模式大小
	const sf::VideoMode windowedMode(1280, 960);
	bool isFullScreen = false;

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

	while (window.isOpen())
	{
		astra_update();//让驱动更新当前帧
		//定义一个系统时间 
		sf::Event event;
		//pollEvent包含一个事件队列(？)，会把事件队列最顶端的事件弹出，
		//把如果发生了任何事件，就会返回真，并且把事件通过引用赋给event;
		while (window.pollEvent(event))
		{
			switch (event.type)//事件类型
			{
			case sf::Event::Closed://关闭
				window.close();
				break;
			case sf::Event::KeyPressed://按键按下了
			{
				if (event.key.code == sf::Keyboard::C && event.key.control)
				{
					window.close();
				}
				switch (event.key.code)
				{
				case sf::Keyboard::D:
				{
					auto oldMode = depthStream.mode();
					astra::ImageStreamMode depthMode;

					depthMode.set_width(640);
					depthMode.set_height(400);
					depthMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
					depthMode.set_fps(30);

					depthStream.set_mode(depthMode);
					auto newMode = depthStream.mode();
					printf("Changed depth mode: %dx%d @ %d -> %dx%d @ %d\n",
						oldMode.width(), oldMode.height(), oldMode.fps(),
						newMode.width(), newMode.height(), newMode.fps());
					break;
				}
				case sf::Keyboard::Escape: //esc键被按下
					window.close();
					break;
				case sf::Keyboard::F:
					if (isFullScreen)
					{
						window.create(windowedMode, "Simple Body Viewer", sf::Style::Default);
					}
					else
					{
						window.create(fullScreenMode, "Simple Body Viewer", fullscreenStyle);
					}
					isFullScreen = !isFullScreen;
					break;
				case sf::Keyboard::H:
					listener.toggle_help();
					break;
				case sf::Keyboard::M:
					depthStream.enable_mirroring(!depthStream.mirroring_enabled());
					break;
				case sf::Keyboard::P:
					listener.toggle_paused();
					break;
				case sf::Keyboard::S:
					if (profile == astra::SkeletonProfile::Full)
					{
						profile = astra::SkeletonProfile::Basic;
						printf("Skeleton Profile: basic\n");
					}
					else
					{
						profile = astra::SkeletonProfile::Full;
						printf("Skeleton Profile: full\n");
					}
					bodyStream.set_skeleton_profile(profile);
					break;
				case sf::Keyboard::T:
					if (features == astra::BodyTrackingFeatureFlags::Segmentation)
					{
						// Joints includes Segmentation
						features = astra::BodyTrackingFeatureFlags::Joints;
						printf("Default Body Features: Seg+Body\n");
					}
					else if (features == astra::BodyTrackingFeatureFlags::Joints)
					{
						// HandPoses includes Joints and Segmentation
						features = astra::BodyTrackingFeatureFlags::HandPoses;
						printf("Default Body Features: Seg+Body+Hand\n");
					}
					else
					{
						// HandPoses includes Joints and Segmentation
						features = astra::BodyTrackingFeatureFlags::Segmentation;
						printf("Default Body Features: Seg\n");
					}
					bodyStream.set_default_body_features(features);
					break;
				case sf::Keyboard::Space:
					listener.toggle_overlay();//画出掩膜
					break;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		listener.draw_to(window);
		window.display();
	}

	astra::terminate();

	return 0;
}
