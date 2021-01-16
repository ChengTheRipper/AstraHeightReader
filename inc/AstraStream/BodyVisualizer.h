#pragma once
#include "../../inc/STD/std_includes.h"
#include "../SFML/sfPen.h"

#include <astra/astra.hpp>


//这个类处理对流进行处理并画出
//继承自FrameListener类的画身体工具
class BodyVisualizer : public astra::FrameListener
{
public:
	BodyVisualizer();

	//本身是一个调色板，这玩意根据给定的bodyID返回一种标准颜色
	static sf::Color get_body_color(std::uint8_t bodyId);


	void init_depth_texture(int width, int height);


	void init_overlay_texture(int width, int height);

	void check_fps();

	void processDepth(astra::Frame& frame);

	void processBodies(astra::Frame& frame);


	void update_body(astra::Body body, const float jointScale);


	void update_bone(const astra::JointList& joints,
		const float jointScale, astra::JointType j1,
		astra::JointType j2);


	void update_overlay(const astra::BodyMask& bodyMask,
		const astra::FloorMask& floorMask);


	void clear_overlay();


	virtual void on_frame_ready(astra::StreamReader& reader,
		astra::Frame& frame) override;


	void draw_bodies(sf::RenderWindow& window);


	void draw_text(sf::RenderWindow& window,
		sf::Text& text,
		sf::Color color,
		const int x,
		const int y) const;


	void draw_help_message(sf::RenderWindow& window) const;


	void draw_to(sf::RenderWindow& window);


	void toggle_paused();


	bool is_paused() const;


	void toggle_overlay();


	bool overlay_enabled() const;


	void toggle_help();


	void set_help_message(const char* msg);

private:
	long double frameDuration_{ 0 };
	std::clock_t lastTimepoint_{ 0 };
	sf::Texture texture_;
	sf::Sprite sprite_;
	sf::Font font_;

	using BufferPtr = std::unique_ptr < uint8_t[] >;
	BufferPtr displayBuffer_{ nullptr };

	std::vector<astra::Vector2f> jointPositions_;

	int depthWidth_{ 0 };
	int depthHeight_{ 0 };
	int overlayWidth_{ 0 };
	int overlayHeight_{ 0 };

	std::vector<sfLine> boneLines_;
	std::vector<sfLine> boneShadows_;
	std::vector<sf::CircleShape> circles_;
	std::vector<sf::CircleShape> circleShadows_;

	float lineThickness_{ 0.5f }; // pixels
	float jointRadius_{ 1.0f };   // pixels
	float shadowRadius_{ 0.5f };  // pixels

	BufferPtr overlayBuffer_{ nullptr };
	sf::Texture overlayTexture_;
	sf::Sprite overlaySprite_;

	bool isPaused_{ false };
	bool isMouseOverlayEnabled_{ true };
	bool isFullHelpEnabled_{ false };
	const char* helpMessage_{ nullptr };
};
