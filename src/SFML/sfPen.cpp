#include "sfPen.h"

void sfLine::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(vertices_, 4, sf::Quads, states);
}
