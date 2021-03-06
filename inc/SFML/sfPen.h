﻿#pragma once
/*
* sfPen这个文件包含多个画笔类
* 比如我们一开始写的sfLine类用来画直线
*/
#include <SFML/Graphics.hpp>

class sfLine : public sf::Drawable
{
public:
	sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color color, float thickness)
		: color_(color)
	{
		const sf::Vector2f direction = point2 - point1;
		const sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		const sf::Vector2f normal(-unitDirection.y, unitDirection.x);

		const sf::Vector2f offset = (thickness / 2.f) * normal;

		vertices_[0].position = point1 + offset;
		vertices_[1].position = point2 + offset;
		vertices_[2].position = point2 - offset;
		vertices_[3].position = point1 - offset;

		for (int i = 0; i < 4; ++i)
			vertices_[i].color = color;
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	sf::Vertex vertices_[4];
	sf::Color color_;
};
