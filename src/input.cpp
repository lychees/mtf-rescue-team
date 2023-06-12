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
#include "input.h"
#include "input_source.h"
#include "output.h"
#include "player.h"
#include "system.h"
#include "baseui.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <utility>
#include <cassert>

namespace Input {
	/**
	 * Start repeat time (in frames) a key has
	 * to be maintained pressed before being
	 * repeated for fist time.
	 */
	constexpr int start_repeat_time = 23;

	/**
	 * Repeat time (in frames) a key has to be
	 * maintained pressed after the start repeat time
	 * has passed for being repeated again.
	 */
	constexpr int repeat_time = 4;

	std::array<int, BUTTON_COUNT> press_time;
	std::bitset<BUTTON_COUNT> triggered, repeated, released;
	Input::KeyStatus raw_triggered, raw_pressed, raw_released;
	std::string textInput;
	std::string textInputDisabled;
	int dir4;
	int dir8;
	std::unique_ptr<Source> source;

	bool gameFocused = true;
	Input::KeyStatus raw_disabled;

	bool wait_input = false;
}

bool Input::isGameFocused() { return gameFocused; }
void Input::setGameFocus(bool game) {
	gameFocused = game;
	ResetKeys(); // prevent external inputs from being readable by game on the frame focus is switched (and vice versa)
}

bool Input::IsWaitingInput() { return wait_input; }
void Input::WaitInput(bool v) { wait_input = v; }

void Input::Init(
	ButtonMappingArray buttons,
	DirectionMappingArray directions,
	const std::string& replay_from_path,
	const std::string& record_to_path
) {
	std::fill(press_time.begin(), press_time.end(), 0);
	triggered.reset();
	repeated.reset();
	released.reset();
	raw_triggered.reset();
	raw_pressed.reset();
	raw_released.reset();

	source = Source::Create(std::move(buttons), std::move(directions), replay_from_path);
	source->InitRecording(record_to_path);

	ResetMask();
}

static void UpdateButton(int i, bool pressed) {
	using namespace Input;

	if (pressed) {
		released[i] = false;
		press_time[i] += 1;
	} else {
		released[i] = press_time[i] > 0;
		press_time[i] = 0;
	}

	if (press_time[i] > 0) {
		triggered[i] = press_time[i] == 1;
		repeated[i] = press_time[i] == 1 || (press_time[i] >= start_repeat_time && press_time[i] % repeat_time == 0);
	} else {
		triggered[i] = false;
		repeated[i] = false;
	}
}

void Input::Update() {
	wait_input = false; // clear each frame

	source->Update();
	auto& pressed_buttons = source->GetPressedButtons();

	// Check button states
	for (unsigned i = 0; i < BUTTON_COUNT; ++i) {
		bool pressed = pressed_buttons[i];
		UpdateButton(i, pressed);
	}

	textInput = source->GetTextInput();

	auto& directions = source->GetDirectionMappings();

	// Press time for directional buttons, the less they have been pressed, the higher their priority will be
	int dirpress[Direction::NUM_DIRECTIONS] = {};

	// Get max pressed time for each directional button
	for (auto& dm: directions) {
		if (dirpress[dm.first] < press_time[dm.second]) {
			dirpress[dm.first] = press_time[dm.second];
		};
	}

	// Calculate diagonal directions pressed time by dir4 combinations
	dirpress[Direction::DOWNLEFT] += (dirpress[Direction::DOWN] > 0 && dirpress[Direction::LEFT] > 0) ? dirpress[Direction::DOWN] + dirpress[Direction::LEFT] : 0;
	dirpress[Direction::DOWNRIGHT] += (dirpress[Direction::DOWN] > 0 && dirpress[Direction::RIGHT] > 0) ? dirpress[Direction::DOWN] + dirpress[Direction::RIGHT] : 0;
	dirpress[Direction::UPLEFT] += (dirpress[Direction::UP] > 0 && dirpress[Direction::LEFT] > 0) ? dirpress[Direction::UP] + dirpress[Direction::LEFT] : 0;
	dirpress[Direction::UPRIGHT] += (dirpress[Direction::UP] > 0 && dirpress[Direction::RIGHT] > 0) ? dirpress[Direction::UP] + dirpress[Direction::RIGHT] : 0;

	dir4 = Direction::NONE;
	dir8 = Direction::NONE;

	// Check if no opposed keys are being pressed at the same time
	if (!(dirpress[Direction::DOWN] > 0 && dirpress[Direction::UP] > 0) && !(dirpress[Direction::LEFT] > 0 && dirpress[Direction::RIGHT] > 0)) {

		// Get dir4 by the with lowest press time (besides 0 frames)
		int min_press_time = 0;
		for (int i = 2; i <= 8; i += 2) {
			if (dirpress[i] > 0) {
				if (min_press_time == 0 || dirpress[i] < min_press_time) {
					dir4 = i;
					min_press_time = dirpress[i];
				}
			}
		}

		// Dir8 will be at least equal to Dir4
		dir8 = dir4;

		// Check diagonal directions (There is a priority order)
		if		(dirpress[Direction::UPRIGHT] > 0)	dir8 = Direction::UPRIGHT;
		else if (dirpress[Direction::UPLEFT] > 0)	dir8 = Direction::UPLEFT;
		else if (dirpress[Direction::DOWNRIGHT] > 0)	dir8 = Direction::DOWNRIGHT;
		else if (dirpress[Direction::DOWNLEFT] > 0)	dir8 = Direction::DOWNLEFT;
	}

	// Determine pressed & released keys from raw keystate
	const auto& raw_pressed_now = source->GetPressedKeys();
	for (unsigned i = 0; i < Input::Keys::KEYS_COUNT; ++i) {
		raw_triggered[i] = raw_pressed_now[i] && !raw_pressed[i];
		raw_released[i] = !raw_pressed_now[i] && raw_pressed[i];
	}
	raw_pressed = raw_pressed_now;
}

void Input::UpdateSystem() {
	source->UpdateSystem();
	auto& pressed_buttons = source->GetPressedButtons();

	// Check button states
	for (unsigned i = 0; i < BUTTON_COUNT; ++i) {
		if (IsSystemButton(static_cast<InputButton>(i))) {
			bool pressed = pressed_buttons[i];
			UpdateButton(i, pressed);
		}
	}
}

void Input::ResetKeys() {
	triggered.reset();
	repeated.reset();
	released.reset();
	for (unsigned i = 0; i < BUTTON_COUNT; i++) {
		press_time[i] = 0;
	}
	dir4 = Direction::NONE;
	dir8 = Direction::NONE;

	textInput = ""; // reset chat input

	// TODO: we want Input to be agnostic to where the button
	// presses are coming from, and if there's a UI at all.
	// Move this into the callers?
	if (DisplayUi) {
		DisplayUi->GetKeyStates().reset();
	}
}

void Input::ResetTriggerKeys() {
	triggered.reset();
}

bool Input::IsPressed(InputButton button) {
	assert(!IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(true);
	return press_time[button] > 0;
}

bool Input::IsTriggered(InputButton button) {
	assert(!IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(true);
	return triggered[button];
}

bool Input::IsRepeated(InputButton button) {
	assert(!IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(true);
	return repeated[button];
}

bool Input::IsReleased(InputButton button) {
	assert(!IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(false);
	return released[button];
}

bool Input::IsSystemPressed(InputButton button) {
	assert(IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	return press_time[button] > 0;
}

bool Input::IsSystemTriggered(InputButton button) {
	assert(IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	return triggered[button];
}

bool Input::IsSystemRepeated(InputButton button) {
	assert(IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	return repeated[button];
}

bool Input::IsSystemReleased(InputButton button) {
	assert(IsSystemButton(button));
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	return released[button];
}

bool Input::IsAnyPressed() {
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(true);
	return std::find_if(press_time.begin(), press_time.end(),
						[](int t) {return t > 0;}) != press_time.end();
}

bool Input::IsAnyTriggered() {
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(true);
	return triggered.any();
}

bool Input::IsAnyRepeated() {
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(true);
	return repeated.any();
}

bool Input::IsAnyReleased() {
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	WaitInput(false);
	return released.any();
}

std::vector<Input::InputButton> Input::GetAllPressed() {
	if(!isGameFocused()) return std::vector<InputButton>(); // disable game input if external (chat) content is focused
	WaitInput(true);
	std::vector<InputButton> vector;
	for (unsigned i = 0; i < BUTTON_COUNT; i++) {
		if (press_time[i] > 0)
			vector.push_back((InputButton)i);
	}
	return vector;
}

std::vector<Input::InputButton> Input::GetAllTriggered() {
	if(!isGameFocused()) return std::vector<InputButton>(); // disable game input if external (chat) content is focused
	WaitInput(true);
	std::vector<InputButton> vector;
	for (unsigned i = 0; i < BUTTON_COUNT; i++) {
		if (triggered[i])
			vector.push_back((InputButton)i);
	}
	return vector;
}

std::vector<Input::InputButton> Input::GetAllRepeated() {
	if(!isGameFocused()) return std::vector<InputButton>(); // disable game input if external (chat) content is focused
	WaitInput(true);
	std::vector<InputButton> vector;
	for (unsigned i = 0; i < BUTTON_COUNT; i++) {
		if (repeated[i])
			vector.push_back((InputButton)i);
	}
	return vector;
}

std::vector<Input::InputButton> Input::GetAllReleased() {
	if(!isGameFocused()) return std::vector<InputButton>(); // disable game input if external (chat) content is focused
	WaitInput(false);
	std::vector<InputButton> vector;
	for (unsigned i = 0; i < BUTTON_COUNT; i++) {
		if (released[i])
			vector.push_back((InputButton)i);
	}
	return vector;
}

bool Input::IsRawKeyPressed(Input::Keys::InputKey key) {
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	return raw_pressed[key];
}

bool Input::IsRawKeyTriggered(Input::Keys::InputKey key) {
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	return raw_triggered[key];
}

bool Input::IsRawKeyReleased(Input::Keys::InputKey key) {
	if(!isGameFocused()) return false; // disable game input if external (chat) content is focused
	return raw_released[key];
}

const Input::KeyStatus& Input::GetAllRawPressed() {
	if(!isGameFocused()) return raw_disabled; // disable game input if external (chat) content is focused
	return raw_pressed;
}

const Input::KeyStatus& Input::GetAllRawTriggered() {
	if(!isGameFocused()) return raw_disabled; // disable game input if external (chat) content is focused
	return raw_triggered;
}

const Input::KeyStatus& Input::GetAllRawReleased() {
	if(!isGameFocused()) return raw_disabled; // disable game input if external (chat) content is focused
	return raw_released;
}

int Input::getDir4() {
	if(!isGameFocused()) return Direction::NONE; // disable game input if external (chat) content is focused
	return dir4;
}

int Input::getDir8() {
	if(!isGameFocused()) return Direction::NONE; // disable game input if external (chat) content is focused
	return dir8;
}

bool Input::IsExternalPressed(InputButton button) {
	assert(!IsSystemButton(button));
	if(isGameFocused()) return false; // disable external (chat) input if game is focused
	WaitInput(true);
	return press_time[button] > 0;
}

bool Input::IsExternalTriggered(InputButton button) {
	assert(!IsSystemButton(button));
	if(isGameFocused()) return false; // disable external (chat) input if game is focused
	WaitInput(true);
	return triggered[button];
}

bool Input::IsExternalRepeated(InputButton button) {
	assert(!IsSystemButton(button));
	if(isGameFocused()) return false; // disable external (chat) input if game is focused
	WaitInput(true);
	return repeated[button];
}

bool Input::IsExternalReleased(InputButton button) {
	assert(!IsSystemButton(button));
	if(isGameFocused()) return false; // disable external (chat) input if game is focused
	WaitInput(false);
	return released[button];
}

std::string& Input::getExternalTextInput() {
	if(isGameFocused()) return textInputDisabled; // disable external (chat) input if game is focused
	return textInput;
}

std::string Input::getClipboardText() {
	return DisplayUi->getClipboardText();
}

Point Input::GetMousePosition() {
	return source->GetMousePosition();
}

void Input::AddRecordingData(Input::RecordingData type, StringView data) {
	assert(source);
	source->AddRecordingData(type, data);
}

bool Input::IsRecording() {
	assert(source);
	return source->IsRecording();
}

Input::KeyStatus Input::GetMask() { // !! Disable input fetching when !gameFocused from this as well?
	assert(source);
	return source->GetMask();
}

void Input::SetMask(Input::KeyStatus new_mask) {
	auto& old_mask = source->GetMask();

#if defined(USE_MOUSE) && defined(SUPPORT_MOUSE)
	if (!Player::mouse_flag) {
		// Mask mouse input when mouse input is not enabled
		constexpr std::array<Input::Keys::InputKey, 7> mouse_keys = {
			Input::Keys::MOUSE_LEFT,
			Input::Keys::MOUSE_RIGHT,
			Input::Keys::MOUSE_MIDDLE,
			Input::Keys::MOUSE_XBUTTON1,
			Input::Keys::MOUSE_XBUTTON2,
			Input::Keys::MOUSE_SCROLLUP,
			Input::Keys::MOUSE_SCROLLDOWN
		};
		for (auto k: mouse_keys) {
			new_mask[k] = true;
		}
	}
#endif

	old_mask = new_mask;
}

void Input::ResetMask() {
	assert(source);
	SetMask(source->GetMask());
}
