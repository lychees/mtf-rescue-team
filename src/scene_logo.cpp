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
#include "scene_logo.h"
#include "async_handler.h"
#include "bitmap.h"
#include "filefinder.h"
#include "game_battle.h"
#include "input.h"
#include "player.h"
#include "scene_title.h"
#include "scene_gamebrowser.h"
#include "output.h"
#include "logo.h"

Scene_Logo::Scene_Logo() :
	frame_counter(0) {
	type = Scene::Logo;
}

void Scene_Logo::Start() {
	if (!Player::debug_flag && !Game_Battle::battle_test.enabled) {
		logo_img = Bitmap::Create(easyrpg_logo, sizeof(easyrpg_logo), false);
		logo.reset(new Sprite());
		logo->SetBitmap(logo_img);
		logo->SetX((SCREEN_TARGET_WIDTH - logo->GetWidth()) / 2);
		logo->SetY((SCREEN_TARGET_HEIGHT - logo->GetHeight()) / 2);
	}
}

void Scene_Logo::Update() {
	static bool is_valid = false;

	if (frame_counter == 0) {
		auto fs = FileFinder::Game();

		if (!fs) {
			fs = FileFinder::Root().Create(Main_Data::GetDefaultProjectPath());
			if (!fs) {
				Output::Error("{} is not a valid path", Main_Data::GetDefaultProjectPath());
			}
			FileFinder::SetGameFilesystem(fs);
		}

#ifdef EMSCRIPTEN
		static bool once = true;
		if (once) {
			FileRequestAsync* index = AsyncHandler::RequestFile("index.json");
			index->SetImportantFile(true);
			request_id = index->Bind(&Scene_Logo::OnIndexReady, this);
			once = false;
			index->Start();
			return;
		}

		if (!async_ready) {
			return;
		}
#endif

		if (FileFinder::IsValidProject(fs)) {
			Player::CreateGameObjects();
			is_valid = true;
		}
	}

	++frame_counter;

	if (Player::debug_flag ||
		Game_Battle::battle_test.enabled ||
		frame_counter == 60 ||
		Input::IsTriggered(Input::DECISION) ||
		Input::IsTriggered(Input::CANCEL)) {

		if (is_valid) {
			if (!Player::startup_language.empty()) {
				Player::translation.SelectLanguage(Player::startup_language);
			}
			Scene::Push(std::make_shared<Scene_Title>(), true);
			if (Player::load_game_id > 0) {
				auto save = FileFinder::Save();

				std::stringstream ss;
				ss << "Save" << (Player::load_game_id <= 9 ? "0" : "") << Player::load_game_id << ".lsd";

				Output::Debug("Loading Save {}", ss.str());

				std::string save_name = save.FindFile(ss.str());
				Player::LoadSavegame(save_name, Player::load_game_id);
			}
		}
		else {
			Scene::Push(std::make_shared<Scene_GameBrowser>(), true);
		}
	}
}

void Scene_Logo::DrawBackground(Bitmap& dst) {
	dst.Clear();
}

void Scene_Logo::OnIndexReady(FileRequestResult*) {
	async_ready = true;

	if (!FileFinder::Game().Exists("index.json")) {
		Output::Debug("index.json not found. The game does not exist or was not correctly deployed.");
		return;
	}

	AsyncHandler::CreateRequestMapping("index.json");

	FileRequestAsync* db = AsyncHandler::RequestFile(DATABASE_NAME);
	db->SetImportantFile(true);
	FileRequestAsync* tree = AsyncHandler::RequestFile(TREEMAP_NAME);
	tree->SetImportantFile(true);
	FileRequestAsync* ini = AsyncHandler::RequestFile(INI_NAME);
	ini->SetImportantFile(true);
	FileRequestAsync* exfont = AsyncHandler::RequestFile("Font/ExFont");
	exfont->SetImportantFile(true);
	FileRequestAsync* soundfont = AsyncHandler::RequestFile("easyrpg.soundfont");
	soundfont->SetImportantFile(true);
	FileRequestAsync* autorun_ineluki = AsyncHandler::RequestFile("autorun.script");
	autorun_ineluki->SetImportantFile(true);
	FileRequestAsync* font_gothic = AsyncHandler::RequestFile("Font/Font");
	font_gothic->SetImportantFile(true);
	FileRequestAsync* font_mincho = AsyncHandler::RequestFile("Font/Font2");
	font_mincho->SetImportantFile(true);

	db->Start();
	tree->Start();
	ini->Start();
	exfont->Start();
	soundfont->Start();
	autorun_ineluki->Start();
	font_gothic->Start();
	font_mincho->Start();
}
