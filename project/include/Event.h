#pragma once

#include <glm/vec2.hpp>

struct Event {
	enum Type {
		KEY_PRESSED,
		KEY_RELEASED,
		MOUSE_MOVED,
		MOUSE_PRESSED,
		MOUSE_RELEASED
	} type;

	int key;
	int modifiers;

	int mouseButton;
	glm::vec2 mousePos;
	glm::vec2 mouseDelta;
};