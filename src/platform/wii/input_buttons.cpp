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
#include "platform/sdl/axis.h"

Input::ButtonMappingArray Input::GetDefaultButtonMappings() {
	return {
		// Wiimote
		{CANCEL, Keys::JOY_OTHER_0}, // A, shared with Classic Controller (CC)
		{DECISION, Keys::JOY_OTHER_1}, // B, shared with CC
		{CANCEL, Keys::JOY_OTHER_2}, // 1
		{DECISION, Keys::JOY_OTHER_3}, // 2
		{RESET, Keys::JOY_OTHER_4}, // -, shared with CC
		{FAST_FORWARD, Keys::JOY_OTHER_5}, // +, shared with CC
		{SETTINGS_MENU, Keys::JOY_OTHER_6}, // Home, shared with CC

		// Nunchuck
		{DECISION, Keys::JOY_OTHER_7}, // Z
		{TOGGLE_FPS, Keys::JOY_OTHER_8}, // C

		// Classic Controller
		{SHIFT, Keys::JOY_OTHER_9}, // X
		{CANCEL, Keys::JOY_OTHER_10}, // Y
		{N0, Keys::JOY_OTHER_11}, // L
		{FAST_FORWARD_PLUS, Keys::JOY_OTHER_12}, // R
		{N5, Keys::JOY_OTHER_13}, // ZL
		{TOGGLE_FPS, Keys::JOY_OTHER_14}, // ZR

		{UP, Keys::JOY_STICK_PRIMARY_UP},
		{DOWN, Keys::JOY_STICK_PRIMARY_DOWN},
		{LEFT, Keys::JOY_STICK_PRIMARY_LEFT},
		{RIGHT, Keys::JOY_STICK_PRIMARY_RIGHT},

		{N1, Keys::JOY_STICK_SECONDARY_DOWN_LEFT},
		{N2, Keys::JOY_STICK_SECONDARY_DOWN},
		{N3, Keys::JOY_STICK_SECONDARY_DOWN_RIGHT},
		{N4, Keys::JOY_STICK_SECONDARY_LEFT},
		{N6, Keys::JOY_STICK_SECONDARY_RIGHT},
		{N7, Keys::JOY_STICK_SECONDARY_UP_LEFT},
		{N8, Keys::JOY_STICK_SECONDARY_UP},
		{N9, Keys::JOY_STICK_SECONDARY_UP_RIGHT},

		// D-Pad on Wiimote & Classic Controller
		{UP, Keys::JOY_DPAD_UP},
		{DOWN, Keys::JOY_DPAD_DOWN},
		{LEFT, Keys::JOY_DPAD_LEFT},
		{RIGHT, Keys::JOY_DPAD_RIGHT},
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
	return {};
}

SdlAxis Input::GetSdlAxis() {
	// Classic Controller L/R Trigger axis do not report proper values
	// Handled above as Button 11/12
	return {
		0, 1, 2, 3, -1, -1, false, false
	};
}
