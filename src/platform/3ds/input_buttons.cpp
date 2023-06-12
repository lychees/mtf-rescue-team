/*
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

// Headers
#include "input_buttons.h"
#include "keys.h"

Input::ButtonMappingArray Input::GetDefaultButtonMappings() {
	return {
		{UP, Keys::JOY_DPAD_UP},
		{DOWN, Keys::JOY_DPAD_DOWN},
		{LEFT, Keys::JOY_DPAD_LEFT},
		{RIGHT, Keys::JOY_DPAD_RIGHT},
		{DECISION, Keys::JOY_A},
		{CANCEL, Keys::JOY_B},
		{CANCEL, Keys::JOY_X},
		{SHIFT, Keys::JOY_Y},
		{TOGGLE_FPS, Keys::JOY_SHOULDER_LEFT},
		{FAST_FORWARD, Keys::JOY_SHOULDER_RIGHT},
		{SETTINGS_MENU, Keys::JOY_START},
		{RESET, Keys::JOY_BACK},
		// Touchscreen
		{N0, Keys::N0},
		{N1, Keys::N1},
		{N2, Keys::N2},
		{N3, Keys::N3},
		{N4, Keys::N4},
		{N5, Keys::N5},
		{N6, Keys::N6},
		{N7, Keys::N7},
		{N8, Keys::N8},
		{N9, Keys::N9},
		{PLUS, Keys::KP_ADD},
		{MINUS, Keys::KP_SUBTRACT},
		{MULTIPLY, Keys::KP_MULTIPLY},
		{DIVIDE, Keys::KP_DIVIDE},
		{PERIOD, Keys::KP_PERIOD},

#if defined(USE_JOYSTICK_AXIS) && defined(SUPPORT_JOYSTICK_AXIS)
		{LEFT, Keys::JOY_STICK_PRIMARY_LEFT},
		{RIGHT, Keys::JOY_STICK_PRIMARY_RIGHT},
		{DOWN, Keys::JOY_STICK_PRIMARY_DOWN},
		{UP, Keys::JOY_STICK_PRIMARY_UP},
#endif
	};
}

Input::DirectionMappingArray Input::GetDefaultDirectionMappings() {
	return {
		{ Direction::DOWN, DOWN },
		{ Direction::LEFT, LEFT },
		{ Direction::RIGHT, RIGHT },
		{ Direction::UP, UP },
	};
}

Input::KeyNamesArray Input::GetInputKeyNames() {
	return {
		{Keys::JOY_DPAD_UP, "D-Pad Up"},
		{Keys::JOY_DPAD_DOWN, "D-Pad Down"},
		{Keys::JOY_DPAD_LEFT, "D-Pad Left"},
		{Keys::JOY_DPAD_RIGHT, "D-Pad Up"},
		{Keys::JOY_STICK_PRIMARY_UP, "Circle Pad Up"},
		{Keys::JOY_STICK_PRIMARY_DOWN, "Circle Pad Down"},
		{Keys::JOY_STICK_PRIMARY_LEFT, "Circle Pad Left"},
		{Keys::JOY_STICK_PRIMARY_RIGHT, "Circle Pad Up"},
		{Keys::JOY_A, "A"},
		{Keys::JOY_B, "B"},
		{Keys::JOY_X, "X"},
		{Keys::JOY_Y, "Y"},
		{Keys::JOY_SHOULDER_LEFT, "L"},
		{Keys::JOY_SHOULDER_RIGHT, "R"},
		{Keys::JOY_BACK, "Select"},
		{Keys::JOY_START, "Start"},
		// Touchscreen
		{Keys::N0, "Touch 0"},
		{Keys::N1, "Touch 1"},
		{Keys::N2, "Touch 2"},
		{Keys::N3, "Touch 3"},
		{Keys::N4, "Touch 4"},
		{Keys::N5, "Touch 5"},
		{Keys::N6, "Touch 6"},
		{Keys::N7, "Touch 7"},
		{Keys::N8, "Touch 8"},
		{Keys::N9, "Touch 9"},
		{Keys::KP_ADD, "Touch +"},
		{Keys::KP_SUBTRACT, "Touch -"},
		{Keys::KP_MULTIPLY, "Touch *"},
		{Keys::KP_DIVIDE, "Touch /"},
		{Keys::KP_PERIOD, "Touch ."}
	};
}
