#include <includes.hpp>
#include <core/config.hpp>
#include <rapidcsv.h>
#include "tools/tools_ui.hpp"
#include "tools/tools_nui.hpp"
#include "tools/hashes/hash_scanner.hpp"
#include "tools/hashes/text_expand.hpp"
#include "actscli.hpp"

namespace {
	int lookuptool(Process& proc, int argc, const char* argv[]) {
		hashutils::ReadDefaultFile();
		for (int i = 2; i < argc; i++) {
			uint64_t v = strtoull(argv[i], nullptr, 0x10);
			const char* extract = hashutils::ExtractPtr(v);
			if (extract) {
				LOG_INFO("{:x}={}", v, extract);
			}
			else {
				LOG_INFO("{:x}=can't be find", v);
			}
		}
		return 0;
	}

	int expand_gen(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) return tool::BAD_USAGE;

		size_t c = std::stoull(argv[2]);

		tool::hash::text_expand::GetDynamicPtr(c, [](const char* str, void* d) { 
			LOG_INFO("{:x},{}", hash::Hash64(str), str);
		});
		return tool::OK;
	}

	int hash32(Process& proc, int argc, const char* argv[]) {
		for (int i = 2; i < argc; i++) {
			LOG_INFO("{}={:x}", argv[i], hash::HashT89Scr(argv[i]));
		}
		return 0;
	}

	int hash64(Process& proc, int argc, const char* argv[]) {
		uint64_t sur;
		int i = 2;
		if (argc >= 3 && !_strcmpi("--iw", argv[2])) {
			sur = 0x47F5817A5EF961BA;
			i++;
			LOG_INFO("(iw)");
		}
		else {
			sur = 0xcbf29ce484222325LL;
		}
		for (; i < argc; i++) {
			LOG_INFO("{}={:x}", argv[i], hash::Hash64(argv[i], sur));
		}
		return 0;
	}

	int hasht7(Process& proc, int argc, const char* argv[]) {
		for (int i = 2; i < argc; i++) {
			LOG_INFO("{}={:x}", argv[i], hash::HashT7(argv[i]));
		}
		return 0;
	}

	int collisiontool(Process& proc, int argc, const char* argv[]) {
		LOG_INFO("Searching...");
		const char* file = actscli::options().defaultHashFile;

		if (!file) {
			file = hashutils::DEFAULT_HASH_FILE;
		}
		int issues = hashutils::LoadMap(file, false);

		if (issues) {
			LOG_ERROR("Found {} collision(s)", issues);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("No collision.");
		return tool::OK;
	}

    struct {
		std::string hash{};
		std::string val{};
		std::string iv{ "100000001b3" };
		std::string lookup{};
        HWND hashEdit{};
		HWND hashEditLabel{};

		HWND hash64Edit{};
		HWND hash64EditLabel{};
		HWND hash32Edit{};
		HWND hash32EditLabel{};
		HWND hash64IW2Edit{};
		HWND hash64IW2EditLabel{};
		HWND hash64IW3Edit{};
		HWND hash64IW3EditLabel{};
		HWND hash32IW4Edit{};
		HWND hash32IW4EditLabel{};
		HWND hashT7Edit{};
		HWND hashT7EditLabel{};
		HWND hash64IWDVEdit{};
		HWND hash64IWDVEditLabel{};
		HWND hash64IWCerFieldEdit{};
		HWND hash64IWCerFieldEditLabel{};

		HWND hashFNV1AReverseValEdit{};
		HWND hashFNV1AReverseIVEdit{};
		HWND hashFNV1AReverseValLabel{};
		HWND hashFNV1AReverseStartEdit{};
		HWND hashFNV1AReverseStartEditLabel{};
		HWND hashFNV1ACustomEdit{};
		HWND hashFNV1ACustomEditLabel{};
		HWND hashLookupEdit{};
		HWND hashLookupEditRet{};
		HWND hashLookupEditLabel{};

		HWND titleLabel{};
    } info{};

	const std::string DefaultVal(uint64_t h) {
		return h ? std::format("{:x}", h) : "N/A";
	}

	void ComputeHashes() {
		info.hash = utils::WStrToStr(tool::ui::GetWindowTextVal(info.hashEdit));


		std::string hash64Val = DefaultVal(hash::Hash64(info.hash.c_str()));
		std::string hash32Val = DefaultVal(hash::HashT89Scr(info.hash.c_str()));
		std::string hash64IW2Val = DefaultVal(hash::HashIWRes(info.hash.c_str()));
		std::string hash64IW3Val = DefaultVal(hash::HashJupScr(info.hash.c_str()));
		std::string hash32IW4Val = DefaultVal(hash::Hash64(info.hash.c_str(), 0x811C9DC5, 0x1000193) & 0xFFFFFFFF);
		std::string hashT7Val = DefaultVal(hash::HashT7(info.hash.c_str()));
		std::string hash64IWDvar = DefaultVal(hash::HashIWDVar(info.hash.c_str()));
		std::string hash64IWCerField = DefaultVal(hash::HashT10Scr(info.hash.c_str()));

		Edit_SetText(info.hash64Edit, hash64Val.c_str());
		Edit_SetText(info.hash32Edit, hash32Val.c_str());
		Edit_SetText(info.hash64IW2Edit, hash64IW2Val.c_str());
		Edit_SetText(info.hash64IW3Edit, hash64IW3Val.c_str());
		Edit_SetText(info.hash32IW4Edit, hash32IW4Val.c_str());
		Edit_SetText(info.hashT7Edit, hashT7Val.c_str());
		Edit_SetText(info.hash64IWDVEdit, hash64IWDvar.c_str());
		Edit_SetText(info.hash64IWCerFieldEdit, hash64IWCerField.c_str());
		HWND hashFNV1ACustomEdit{};
		HWND hashFNV1ACustomEditLabel{};

		info.val = utils::WStrToStr(tool::ui::GetWindowTextVal(info.hashFNV1AReverseValEdit));
		info.iv = utils::WStrToStr(tool::ui::GetWindowTextVal(info.hashFNV1AReverseIVEdit));

		uint64_t val{};
		uint64_t iv{};
		try {
			val = std::strtoull(info.val.data(), nullptr, 16);
			iv = std::strtoull(info.iv.data(), nullptr, 16);

			if (val && iv) {
				std::string customstr = std::format("{:x}", hash::Hash64A(info.hash.c_str(), val, iv));
				Edit_SetText(info.hashFNV1ACustomEdit, customstr.c_str());

				uint64_t mask = 0xFFFF;
				uint64_t found = 0;
				uint64_t disc{};

				while (found < 64) {
					uint64_t k;
					for (k = 0; k < 0x10000; k++) {
						uint64_t v = (k << found) | disc;
						if ((hash::Hash64A(info.hash.data(), v, iv) & mask) == (val & mask)) {
							break;
						}
					}
					if (k == 0x10000) {
						throw std::runtime_error("Invalid value");
					}

					disc |= (k & 0xF) << found;

					found += 4;
					mask = (mask << 4) | 0xF;
				}

				std::string discstr = std::format("{:x}", disc);
				Edit_SetText(info.hashFNV1AReverseStartEdit, discstr.c_str());
			}
			else {
				Edit_SetText(info.hashFNV1AReverseStartEdit, "N/A");
				Edit_SetText(info.hashFNV1ACustomEdit, "N/A");
			}
		}
		catch (std::runtime_error& e) {
			std::string discstr = e.what();
			Edit_SetText(info.hashFNV1AReverseStartEdit, discstr.c_str());
			if (!val || !iv) {
				Edit_SetText(info.hashFNV1ACustomEdit, "N/A");
			}
		}
	}

	void ComputeLookup() {
		info.lookup = utils::WStrToStr(tool::ui::GetWindowTextVal(info.hashLookupEdit));

		try {
			uint64_t val = std::strtoull(info.lookup.data(), nullptr, 16);

			const char* lookupVal = hashutils::ExtractPtr(val);

			std::string discstr = lookupVal ? lookupVal : "can't find";
			Edit_SetText(info.hashLookupEditRet, discstr.c_str());
		}
		catch (std::runtime_error& e) {
			std::string discstr = e.what();
			Edit_SetText(info.hashLookupEditRet, discstr.c_str());
		}
	}
	struct HashAlg {
		const char* id;
		const char* desc;
		std::function<uint64_t(const char* text)> hashFunc;
		char buffer[0x20]{ 0 };
		bool selected{};
	};

	static HashAlg algs[]
	{
		{ "h64", "fnv1a", [](const char* text) -> uint64_t { return hash::Hash64A(text); } },
		{ "res", "iw res", [](const char* text) -> uint64_t { return hash::HashIWRes(text); } },
		{ "h32", "t89 canon", [](const char* text) -> uint64_t { return hash::HashT89Scr(text); } },
		{ "iw9", "mwii/iii canon", [](const char* text) -> uint64_t { return hash::HashJupScr(text); } },
		{ "bo6", "bo6 canon", [](const char* text) -> uint64_t { return hash::HashT10Scr(text); } },
		{ "bo6sp", "bo6 canon (SP)", [](const char* text) -> uint64_t { return hash::HashT10ScrSP(text); } },
		{ "t7", "t7 fnv1a", [](const char* text) -> uint64_t { return hash::HashT7(text); } },
		{ "tag", "IW tag", [](const char* text) -> uint64_t { return hash::HashIWTag(text); } },
		{ "dvar", "IW Dvar", [](const char* text) -> uint64_t { return hash::HashIWDVar(text); } },
	};

	static void SyncAlgCfg() {
		static std::once_flag of;

		std::call_once(of, [] {
			for (HashAlg& alg : algs) {
				alg.selected = core::config::GetBool(std::format("hash.alg.{}", alg.id), true);
			}
		});
	}

	void hash_nui() {
		tool::nui::NuiUseDefaultWindow dw{};
		static char hashBuff[0x100];
		SyncAlgCfg();

		ImGui::SeparatorText("Hashes");

		bool update{};
		if (ImGui::InputText("String", hashBuff, sizeof(hashBuff))) {
			for (HashAlg& alg : algs) {
				sprintf_s(alg.buffer, "%llx", alg.hashFunc(hashBuff));
			}
			update = true;
		}

		ImGui::SeparatorText("Values");

		for (HashAlg& alg : algs) {
			ImGui::InputText(alg.desc, alg.buffer, sizeof(alg.buffer), ImGuiInputTextFlags_ReadOnly);
		}

		ImGui::SeparatorText("Reverse / Custom");

		static char reversePrimeBuffer[0x20]{ "" };
		static char reverseIvBuffer[0x20]{ "100000001b3" };
		static char reverseOutputBuffer[0x80]{ "" };
		static char reverseCustomBuffer[0x80]{ "" };

		bool cr = update;

		cr |= ImGui::InputText("Reverse (Prime)", reversePrimeBuffer, sizeof(reversePrimeBuffer));
		cr |= ImGui::InputText("Reverse (IV)", reverseIvBuffer, sizeof(reverseIvBuffer));

		if (cr) {
			uint64_t val{};
			uint64_t iv{};
			try {
				val = std::strtoull(reversePrimeBuffer, nullptr, 16);
				iv = std::strtoull(reverseIvBuffer, nullptr, 16);

				if (val && iv) {
					sprintf_s(reverseCustomBuffer, "%llx", hash::Hash64A(hashBuff, val, iv));

					uint64_t mask = 0xFFFF;
					uint64_t found = 0;
					uint64_t disc{};

					while (found < 64) {
						uint64_t k;
						for (k = 0; k < 0x10000; k++) {
							uint64_t v = (k << found) | disc;
							if ((hash::Hash64A(hashBuff, v, iv) & mask) == (val & mask)) {
								break;
							}
						}
						if (k == 0x10000) {
							throw std::runtime_error("Invalid value");
						}

						disc |= (k & 0xF) << found;

						found += 4;
						mask = (mask << 4) | 0xF;
					}

					sprintf_s(reverseOutputBuffer, "%llx", disc);
				}
				else {
					sprintf_s(reverseOutputBuffer, "N/A");
					sprintf_s(reverseCustomBuffer, "N/A");
				}
			}
			catch (std::runtime_error& e) {
				sprintf_s(reverseOutputBuffer, e.what());
				if (!val || !iv) {
					sprintf_s(reverseCustomBuffer, "N/A");
				}
			}
		}

		ImGui::InputText("Reverse (Start)", reverseOutputBuffer, sizeof(reverseOutputBuffer), ImGuiInputTextFlags_ReadOnly);
		ImGui::InputText("Custom", reverseCustomBuffer, sizeof(reverseCustomBuffer), ImGuiInputTextFlags_ReadOnly);

		ImGui::SeparatorText("Lookup");

		if (ImGui::Button("Load hashes")) {
			hashutils::ReadDefaultFile(true);
		}

		if (!hashutils::GetMap().empty()) {
			static char lookupInputBuffer[0x20]{ "" };
			static char lookupOutputBuffer[0x100]{ "" };

			if (ImGui::InputText("Hashed", lookupInputBuffer, sizeof(lookupInputBuffer))) {
				try {
					uint64_t val = std::strtoull(lookupInputBuffer, nullptr, 16);

					const char* lookupVal = hashutils::ExtractPtr(val);

					const char* discstr = lookupVal ? lookupVal : "can't find";
					snprintf(lookupOutputBuffer, sizeof(lookupOutputBuffer), "%s", discstr);
				}
				catch (std::runtime_error& e) {
					snprintf(lookupOutputBuffer, sizeof(lookupOutputBuffer), "%s", e.what());
				}
			}
			ImGui::InputText("Unhashed", lookupOutputBuffer, sizeof(lookupOutputBuffer), ImGuiInputTextFlags_ReadOnly);
		}
		else {
			ImGui::Text("No hashes available");
		}
	}

	constexpr uint64_t HASH_MASK = 0xFFFFFFFFFFFFFFF; // remove 2 last bits to match fnv1a63 and greyhound hashes

	void hashsearch_nui() {
		tool::nui::NuiUseDefaultWindow dw{};
		static char guessIn[0x100]{ 0 };
		static char guessInCpy[sizeof(guessIn)]{0};
		static char loadPath[0x100]{ 0 };
		static std::string guessOut{};
		static std::unordered_set<uint64_t> hashes{};
		auto LoadHashes = [] {
			hashes.clear();
			std::vector<std::filesystem::path> files{};

			utils::GetFileRecurse(loadPath, files, [](const std::filesystem::path& p) {
				std::string name = p.string();
				return
					name.ends_with(".gsc") || name.ends_with(".csc")
					|| name.ends_with(".csv") || name.ends_with(".tsv")
					|| name.ends_with(".dec.lua")
					|| name.ends_with(".json");
				});

			std::unordered_set<uint64_t> tmp{};
			// TODO: add loading bar
			tool::hash::scanner::ScanHashes(files, tmp);

			for (uint64_t t : tmp) {
				hashes.insert(t & HASH_MASK);
			}
		};
		{
			static std::once_flag of;

			std::call_once(of, [&LoadHashes] {
				std::string loadPathCfg = core::config::GetString("hash.path", "");

				snprintf(loadPath, sizeof(loadPath), "%s", loadPathCfg.c_str());
				if (!loadPathCfg.empty() && core::config::GetBool("hash.loadPathAtStart", false)) {
					LoadHashes();
				}
			});
		}
		SyncAlgCfg();
		bool c = false;

		ImGui::SeparatorText("Hash searcher");

		if (hashes.empty()) {
			ImGui::Text("No hash loaded");
		}
		else {
			ImGui::Text("%llu hash(es) loaded", hashes.size());
		}

		if (ImGui::InputText("Path", loadPath, sizeof(loadPath))) {
			core::config::SetString("hash.path", loadPath);
			c = true;
		}

		if (ImGui::Button("Load hashes")) {
			LoadHashes();
		}

		bool edit{};
		if (ImGui::InputText("Guess", guessIn, sizeof(guessIn))) {
			edit = true;
		}

		if (ImGui::BeginCombo("Selected hashes", nullptr, ImGuiComboFlags_NoPreview)) {
			for (HashAlg& alg : algs) {
				if (ImGui::Checkbox(alg.desc, &alg.selected)) {
					core::config::SetBool(std::format("hash.alg.{}", alg.id), alg.selected);
					c = true;
					edit = true;
				}
			}
			ImGui::EndCombo();
		}

		if (edit) {
			guessOut = {};
			if (*guessIn) {
				size_t guessInLen = std::strlen(guessIn);
				memcpy(guessInCpy, guessIn, guessInLen);

				// expand by reducing the len
				for (size_t i = guessInLen; i > 0; i--) {
					guessInCpy[i] = 0;

					for (HashAlg& alg : algs) {
						if (!alg.selected) {
							continue;
						}

						uint64_t v = alg.hashFunc(guessInCpy);

						auto it = hashes.find(v & HASH_MASK);

						if (it != hashes.end()) {
							guessOut += std::format("{:x},{}\n", v, guessInCpy);
						}
					}
				}
			}
		}

		ImGui::InputTextMultiline("Output", guessOut.data(), guessOut.length(), ImVec2(0, 200), ImGuiInputTextFlags_ReadOnly);

		if (c) tool::nui::SaveNextConfig();
	}

    int Render(HWND window, HINSTANCE hInstance) {
		std::wstring hashw = utils::StrToWStr(info.hash);
		std::wstring valw = utils::StrToWStr(info.val);
		std::wstring ivw = utils::StrToWStr(info.iv);
		std::wstring lookupw = utils::StrToWStr(info.lookup);
		
		info.titleLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Hash",
			SS_CENTER | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hashEdit = CreateWindowExW(
			0,
			L"EDIT",
			hashw.c_str(),
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hashEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"String : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64Edit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64EditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"FNV1A : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash32Edit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash32EditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"T7/8/9 Canon : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IW2Edit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IW2EditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"IW Resource : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IW3Edit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IW3EditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"MWII/III Canon : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash32IW4Edit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash32IW4EditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"IW Tag : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hashT7Edit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hashT7EditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"T7 FNV1A : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IWDVEdit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IWDVEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"IW DVar : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IWCerFieldEdit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hash64IWCerFieldEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"BO6 Canon : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);


		info.hashFNV1AReverseValEdit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.hashFNV1AReverseIVEdit = CreateWindowExW(
			0,
			L"EDIT",
			ivw.c_str(),
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		info.hashFNV1AReverseValLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Reverse (Val/IV) : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		info.hashFNV1AReverseStartEdit = CreateWindowExW(
			0,
			L"EDIT",
			valw.c_str(),
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		info.hashFNV1AReverseStartEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Reverse (Start) : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		info.hashFNV1ACustomEdit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		info.hashFNV1ACustomEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Custom : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);


		info.hashLookupEdit = CreateWindowExW(
			0,
			L"EDIT",
			lookupw.c_str(),
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		info.hashLookupEditRet = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		info.hashLookupEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Lookup : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		
        if (
            info.hashEdit == NULL
			|| info.hashEditLabel == NULL
			|| info.hash64Edit == NULL
			|| info.hash64EditLabel == NULL
			|| info.hash32Edit == NULL
			|| info.hash32EditLabel == NULL
			|| info.hash64IW2Edit == NULL
			|| info.hash64IW2EditLabel == NULL
			|| info.hash64IW3Edit == NULL
			|| info.hash64IW3EditLabel == NULL
			|| info.hash32IW4Edit == NULL
			|| info.hash32IW4EditLabel == NULL
			|| info.hashT7Edit == NULL
			|| info.hashT7EditLabel == NULL
			|| info.hash64IWDVEdit == NULL
			|| info.hash64IWDVEditLabel == NULL
			|| info.hash64IWCerFieldEdit == NULL
			|| info.hash64IWCerFieldEditLabel == NULL
			|| info.hashFNV1AReverseValEdit == NULL
			|| info.hashFNV1AReverseIVEdit == NULL
			|| info.hashFNV1AReverseValLabel == NULL
			|| info.hashFNV1AReverseStartEdit == NULL
			|| info.hashFNV1AReverseStartEditLabel == NULL
			|| info.hashFNV1ACustomEdit == NULL
			|| info.hashFNV1ACustomEditLabel == NULL
			|| info.hashLookupEdit == NULL
			|| info.hashLookupEditLabel == NULL
			|| info.hashLookupEditRet == NULL
			|| info.titleLabel == NULL
            ) {
            return -1;
        }

		SendMessage(info.hashEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hash64Edit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hash32Edit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hash64IW2Edit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hash64IW3Edit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hash32IW4Edit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashT7Edit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hash64IWDVEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hash64IWCerFieldEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashFNV1AReverseValEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashFNV1AReverseIVEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashFNV1AReverseStartEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashFNV1ACustomEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashLookupEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashLookupEditLabel, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.hashLookupEditRet, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		
		Edit_SetReadOnly(info.hash64Edit, true);
		Edit_SetReadOnly(info.hash32Edit, true);
		Edit_SetReadOnly(info.hash64IW2Edit, true);
		Edit_SetReadOnly(info.hash64IW3Edit, true);
		Edit_SetReadOnly(info.hash32IW4Edit, true);
		Edit_SetReadOnly(info.hashT7Edit, true);
		Edit_SetReadOnly(info.hash64IWDVEdit, true);
		Edit_SetReadOnly(info.hashFNV1AReverseStartEdit, true);
		Edit_SetReadOnly(info.hashFNV1ACustomEdit, true);
		Edit_SetReadOnly(info.hashLookupEditRet, true);

		ComputeHashes();
		ComputeLookup();

        return 0;
    }

    LRESULT Update(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_COMMAND) {
            if (HIWORD(wParam) == EN_CHANGE) {
				if (info.hashEdit == (HWND)lParam || info.hashFNV1AReverseValEdit == (HWND)lParam || info.hashFNV1AReverseIVEdit == (HWND)lParam) {
					ComputeHashes();
				}
				if (info.hashLookupEdit == (HWND)lParam) {
					ComputeLookup();
				}
            }
        }
        else if (uMsg == WM_CTLCOLORSTATIC) {
            if (lParam == (LPARAM)info.hashEdit
                || lParam == (LPARAM)info.hashEditLabel
				|| lParam == (LPARAM)info.hash64Edit
				|| lParam == (LPARAM)info.hash64EditLabel
				|| lParam == (LPARAM)info.hash32Edit
				|| lParam == (LPARAM)info.hash32EditLabel
				|| lParam == (LPARAM)info.hash64IW2Edit
				|| lParam == (LPARAM)info.hash64IW2EditLabel
				|| lParam == (LPARAM)info.hash64IW3Edit
				|| lParam == (LPARAM)info.hash64IW3EditLabel
				|| lParam == (LPARAM)info.hash32IW4Edit
				|| lParam == (LPARAM)info.hash32IW4EditLabel
				|| lParam == (LPARAM)info.hashT7Edit
				|| lParam == (LPARAM)info.hashT7EditLabel
				|| lParam == (LPARAM)info.hash64IWDVEdit
				|| lParam == (LPARAM)info.hash64IWDVEditLabel
				|| lParam == (LPARAM)info.hash64IWCerFieldEdit
				|| lParam == (LPARAM)info.hash64IWCerFieldEditLabel
				|| lParam == (LPARAM)info.hashFNV1AReverseValEdit
				|| lParam == (LPARAM)info.hashFNV1AReverseIVEdit
				|| lParam == (LPARAM)info.hashFNV1AReverseValLabel
				|| lParam == (LPARAM)info.hashFNV1AReverseStartEdit
				|| lParam == (LPARAM)info.hashFNV1AReverseStartEditLabel
				|| lParam == (LPARAM)info.hashFNV1ACustomEdit
				|| lParam == (LPARAM)info.hashFNV1ACustomEditLabel
				|| lParam == (LPARAM)info.hashLookupEdit
				|| lParam == (LPARAM)info.hashLookupEditLabel
				|| lParam == (LPARAM)info.hashLookupEditRet
				|| lParam == (LPARAM)info.titleLabel
                ) {
				
                return 0;
            }
        }
        return 1;
    }
	void Resize(int width, int height) {
		int y{ height / 2 - 28 * 7 };
		SetWindowPos(info.titleLabel, NULL, 0, y - 68, width, 60, SWP_SHOWWINDOW);

		SetWindowPos(info.hashEdit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;

		y += 14;
		SetWindowPos(info.hash64Edit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hash64EditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.hash32Edit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hash32EditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.hash64IW2Edit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hash64IW2EditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.hash64IW3Edit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hash64IW3EditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.hash32IW4Edit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hash32IW4EditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.hashT7Edit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashT7EditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.hash64IWDVEdit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hash64IWDVEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.hash64IWCerFieldEdit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hash64IWCerFieldEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 14;

		y += 28;
		SetWindowPos(info.hashFNV1AReverseValEdit, NULL, width / 2 - 250, y, 298, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashFNV1AReverseIVEdit, NULL, width / 2 +  52, y, 248, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashFNV1AReverseValLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;

		SetWindowPos(info.hashFNV1AReverseStartEdit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashFNV1AReverseStartEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;

		SetWindowPos(info.hashFNV1ACustomEdit, NULL, width / 2 - 250, y, 550, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashFNV1ACustomEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 14;

		y += 28;
		SetWindowPos(info.hashLookupEdit, NULL, width / 2 - 250, y, 198, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashLookupEditRet, NULL, width / 2 - 48, y, 348, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.hashLookupEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;

		tool::ui::window().SetTitleFont(info.titleLabel);
	}

	int httest(Process& proc, int argc, const char* argv[]) {

		const char* str = "abilities_active"; // c0eb9bd84c300f2b
		const char* str2 = "assert"; // c0eb9bd84c300f2b

		std::cout << std::hex << "" << "\n";
		std::cout << std::hex << hash::Hash64("system", hash::Hash64("zt@f3yp(d[kkd=_@", 0x1C2F2E3C8A257D07, 0x10000000233LL), 0x10000000233LL) << "\n";
		std::cout << std::hex << hash::Hash64("abilities_active", hash::Hash64("zt@f3yp(d[kkd=_@", 0x1C2F2E3C8A257D07, 0x10000000233LL), 0x10000000233LL) << "\n";

		std::cout << std::hex << ((0x70f9c84fce473ef9ull / 0x10000000233LL) ^ 'i') << "\n";
		
		// 105 = 0x69
		// (0x70f9a1 ^ 'i') * 0x10000000233LL = 70f9c84fce473ef9

		//0x10000000233LL
		//70f9c84fce473ef9, i

		constexpr auto hashi = 0x70f9c84fce473ef9ull;

		//100000001b3 10000000233
		constexpr auto iv = 0x10000000233LL; // 0x10000000233LL
		constexpr auto ivr = iv & 0xFFFFF;
		constexpr auto bits = 35;
		constexpr auto bitsMask = (1ull << bits) - 1;

		constexpr auto lower = ((hashi / ivr) ^ 'i') & (bitsMask);

		constexpr auto upper = (1ull << (64 - bits));

		constexpr auto mask = 0x7FFFFFFFFFFFFFFF;

		std::cout << std::hex << hash::Hash64("abilities_active", 0x64a68ff7d4912fd2, 0x10000000233LL) << "\n";

		//for (size_t i = 0; i < upper; i++) {
		//	uint64_t key = (i << 48) | 0x8ff7d4912fd2;
		//	if ((hash::Hash64("abilities_active", key, iv) & mask) == (0xc0eb9bd84c300f2b & mask)) {
		//		std::cout << std::hex << key << "\n";
		//		
		//	}
		//
		//}
		//

		
		return tool::OK;

	}

	int fnv1acrack(Process& proc, int argc, const char* argv[]) {
		if (argc < 5) {
			return tool::BAD_USAGE;
		}
		// 0xc0eb9bd84c300f2b
		const char* str = argv[2];
		uint64_t key = std::strtoull(argv[3], nullptr, 16);
		uint64_t iv = std::strtoull(argv[4], nullptr, 16);

		LOG_DEBUG("Searching {} 0x{:x}/0x{:x}", str, key, iv);

		uint64_t mask = 0xFFFF;
		uint64_t found = 0;
		uint64_t disc{};

		while (found < 64) {
			uint64_t k;
			for (k = 0; k < 0x10000; k++) {
				uint64_t v = (k << found) | disc;
				if ((hash::Hash64A(str, v, iv) & mask) == (key & mask)) {
					LOG_TRACE("bit {}: {:x}", found, v);
					break;
				}
			}
			if (k == 0x10000) {
				LOG_ERROR("Invalid key");
				return tool::BASIC_ERROR;
			}

			disc |= (k & 0xF) << found;

			found += 4;
			mask = (mask << 4) | 0xF;
		}

		LOG_INFO("end .. 0x{:x}, 0x{:x}", disc, iv);
		LOG_INFO("test . hash(\"{}\", 0x{:x}, 0x{:x}) = 0x{:x}", str, disc, iv, hash::Hash64A(str, disc, iv));

		return tool::OK;
	}
	struct HashEntry {
		std::string str;
		uint64_t val;
	};


	inline bool FindAll(const std::vector<HashEntry>& entries, uint64_t testKey, uint64_t iv, uint64_t mask) {
		for (const HashEntry& e : entries) {
			if ((hash::Hash64A(e.str.data(), testKey, iv) & mask) != (e.val & mask)) {
				return false;
			}
		}
		return true;
	}

	uint64_t CrackFnv1a(const std::vector<HashEntry>& entries, uint64_t iv, uint64_t find, size_t depth, uint64_t mask) {
		uint64_t k;
		for (k = 0; k < 0x10000; k++) {
			uint64_t v = (k << depth) | find;
			if (FindAll(entries, v, iv, mask)) {
				uint64_t nf = find |= (k & 0xF) << depth;
				if (depth >= 60) {
					return nf;
				}
				uint64_t v2 = CrackFnv1a(entries, iv, nf, depth + 4, (mask << 4 | 0xF));
				if (v2) {
					return v2;
				}
			}
		}
		return 0;
	}

	uint64_t CrackFnv1aSingle(uint64_t key, const char* str, uint64_t iv = hash::IV_DEFAULT) {
		uint64_t mask = 0xF;
		uint64_t found{};
		uint64_t disc{};

		while (found < 64) {
			uint64_t k;
			for (k = 0; k < 0x10; k++) {
				uint64_t v = (k << found) | disc;
				if ((hash::Hash64A(str, v, iv) & mask) == (key & mask)) {
					goto foundHash;
				}
			}
			return 0;
		foundHash:
			disc |= k << found;

			found += 4;
			mask = (mask << 4) | 0xF;
		}

		return disc;
	}

	int fnv1acrack2(Process& proc, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path csv = argv[2];
		uint64_t iv = std::strtoull(argv[3], nullptr, 16);

		LOG_DEBUG("Searching {} 0x{:x}", csv.string(), iv);
		std::string buffer{};

		if (!utils::ReadFile(csv, buffer)) {
			LOG_WARNING("Can't read hash csv {}", csv.string());
			return tool::BASIC_ERROR;
		}

		rapidcsv::Document doc{};

		std::stringstream stream{ buffer };

		doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

		if (doc.GetColumnCount() < 2) {
			LOG_WARNING("Can't read hash csv {}: invalid file", csv.string());
			return tool::BASIC_ERROR;
		}

		std::vector<HashEntry> entries{};

		for (size_t i = 0; i < doc.GetRowCount(); i++) {
			const std::string hash = doc.GetCell<std::string>(0, i);
			const std::string value = doc.GetCell<std::string>(1, i);

			try {
				entries.emplace_back(value, std::strtoull(hash.c_str(), nullptr, 16));
			}
			catch (std::runtime_error& e) {
				LOG_WARNING("Error when reading {}: invalid line {}: {}", csv.string(), i, e.what());
			}
		}

		uint64_t v = CrackFnv1a(entries, iv, 0, 0, 0xFFFF);

		LOG_INFO("result -> {:x}", v);

		return tool::OK;
	}

	int fnv1acrack3(Process& proc, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path csv = argv[2];
		uint64_t iv = std::strtoull(argv[3], nullptr, 16);

		LOG_DEBUG("Searching {} 0x{:x}", csv.string(), iv);
		std::string buffer{};

		if (!utils::ReadFile(csv, buffer)) {
			LOG_WARNING("Can't read hash csv {}", csv.string());
			return tool::BASIC_ERROR;
		}

		rapidcsv::Document doc{};

		std::stringstream stream{ buffer };

		doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

		if (doc.GetColumnCount() < 2) {
			LOG_WARNING("Can't read hash csv {}: invalid file", csv.string());
			return tool::BASIC_ERROR;
		}

		
		std::vector<std::vector<HashEntry>> entries{};
		entries.resize(0x80);

		for (size_t i = 0; i < doc.GetRowCount(); i++) {
			const std::string hash = doc.GetCell<std::string>(0, i);
			std::string value = doc.GetCell<std::string>(1, i);

			utils::LowerCase(value.data());

			if (value.empty()) continue;
			try {
				entries[value[0]].emplace_back(value, std::strtoull(hash.c_str(), nullptr, 16));
			}
			catch (std::runtime_error& e) {
				LOG_WARNING("Error when reading {}: invalid line {}: {}", csv.string(), i, e.what());
			}
		}

		for (size_t i = 0; i <= 0x7F; i++) {
			if (entries[i].empty()) continue;
			uint64_t v = CrackFnv1a(entries[i], iv, 0, 0, 0xFFFF);

			if (v) {
				LOG_INFO("result {} -> {:x}", (char)i, v);
			}

		}


		return tool::OK;
	}

	int fakefnvds(Process& proc, int argc, const char* argv[]) {
		if (argc < 5) {
			return tool::BAD_USAGE;
		}

		const char* csv = argv[2];
		uint64_t key = std::strtoull(argv[3], nullptr, 16);
		uint64_t iv = std::strtoull(argv[4], nullptr, 16);


		std::ofstream os{ csv };

		if (!os) {
			LOG_ERROR("Can't open {}", csv);
			return tool::BASIC_ERROR;
		}

		utils::CloseEnd ce{ [&os]() {os.close(); } };

		srand(0);

		char buff[36]{};

		for (size_t i = 0; i < 0x100; i++) {
			int len = 5 + rand() % 30;

			for (int i = 0; i < len; i++) {
				buff[i] = rand() % ('Z' - 'A') + 'a';
			}
			buff[len] = 0;

			if (i) os << "\n";

			os << std::hex << hash::Hash64A(buff, key, iv) << "," << buff;
		}

		return tool::OK;
	}

	int fnv1acrackcommon(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) return tool::BAD_USAGE;

		uint64_t h1 = hash::HashPattern(argv[2]);
		uint64_t h2 = hash::HashPattern(argv[3]);
		uint64_t len = std::strtoull(argv[4], nullptr, 10);


		if (!len || !h1 || !h2) {
			return tool::BAD_USAGE;
		}


		struct Ctx {
			uint64_t h1;
			uint64_t h2;
			size_t max{ 1 };
			const char* s1;
			uint64_t lu1;
		};

		Ctx ctx{ h1, h2 };

		for (size_t i = 0; i < len; i++) {
			ctx.max *= sizeof(tool::hash::text_expand::cdict);
		}
		//291e1c117ebbf5e6 - b = 2dbc620bcf7203e0

		LOG_INFO("Searching common(0x{:x}, 0x{:x}, {}/{}/{})", ctx.h1, ctx.h2, len, ctx.max, ctx.max * ctx.max);

		tool::hash::text_expand::GetDynamic<Ctx>(ctx.max, [](const char* s, Ctx* ctx) {
			ctx->s1 = s;
			ctx->lu1 = CrackFnv1aSingle(ctx->h1, s);
			if (!ctx->lu1) {
				LOG_TRACE("bad 0x{:x} = {}", ctx->lu1, s);
				return; // bad
			}
			tool::hash::text_expand::GetDynamic<Ctx>(ctx->max, [](const char* s2, Ctx* ctx) {
				uint64_t lu2 = CrackFnv1aSingle(ctx->h2, s2);
				if (ctx->lu1 != lu2) return; // ignore
				
				LOG_INFO("FOUND SIMILAR HASH:");
				LOG_INFO("UnFnv1a(0x{:x},\"{}\") = 0x{:x}", ctx->h1, ctx->s1, lu2);
				LOG_INFO("UnFnv1a(0x{:x},\"{}\") = 0x{:x}", ctx->h2, s2, lu2);
			}, ctx);
		}, &ctx);


		return tool::OK;
	}

	int bruteforcecustom(int argc, const char* argv[]) {
		tool::hash::text_expand::GetDynamicAsync<void>(std::string::npos, [](const char* s, void* ctx) {
			constexpr uint64_t start = hash::Hash64("_", 0x7a42b57be462143f);
			uint64_t p = hash::Hash64(s, start);

			if (p == 0xa16ec18512a89f20) { LOG_INFO("find2 {}", s); }
			else if (p == 0xa2556a2905fd952) { LOG_INFO("finda255 {}", s); }
			//else if (p == 0x7a42b57be462143f) { LOG_INFO("find {}", s); } // Demspey Matryoshka Dolls
			else if (p == 0x291e1c117ebbf5e6) { LOG_INFO("find29 {}", s); } // Richtofen Matryoshka Dolls / rich
			else if (p == 0x7264d6f24a950a5b) { LOG_INFO("find72 {}", s); } // Nikolai Matryoshka Dolls / niko
			else if (p == 0x579652e2459b8c74) { LOG_INFO("find57 {}", s); } // Takeo Matryoshka Dolls / takeo
		});
		return tool::OK;
	}
}

ADD_TOOL_UI(hash, L"Hash", Render, Update, Resize);
ADD_TOOL_NUI(hash, "Hash", hash_nui);
ADD_TOOL_NUI(hashsearch, "Searcher", hashsearch_nui);

ADD_TOOL(lookup, "hash", " (string)*", "lookup strings", nullptr, lookuptool);
ADD_TOOL(expand_gen, "hash", " [count]", "expand test", nullptr, expand_gen);
ADD_TOOL(h32, "hash", " (string)*", "hash strings", nullptr, hash32);
ADD_TOOL(h64, "hash", " (string)*", "hash strings", nullptr, hash64);
ADD_TOOL(ht7, "hash", " (string)*", "hash strings", nullptr, hasht7);
ADD_TOOL(httest, "hash", " (string)*", "hash strings", nullptr, httest);
ADD_TOOL(fnv1acrack, "hash", " [string] [hash] [iv]", "crack fnv1a key, base iv: 100000001b3, 10000000233", nullptr, fnv1acrack);
ADD_TOOL(fnv1acrack2, "hash", " [csv] [iv]", "crack fnv1a key (one key based)", nullptr, fnv1acrack2);
ADD_TOOL(fnv1acrack3, "hash", " [csv] [iv]", "crack fnv1a keys (first char based)", nullptr, fnv1acrack3);
ADD_TOOL(fnv1acrackcommon, "hash", " [hash1] [hash2] [len]", "try to find a common prefix and suffix of 2 hashes", fnv1acrackcommon);
ADD_TOOL(str, "hash", "", "check collisions in the string file", nullptr, collisiontool);
#ifndef CI_BUILD
ADD_TOOL(fakefnvds, "hash", " [csv] [key] [iv]", "gen fake fnv1a dataset", nullptr, fakefnvds);
ADD_TOOL(bruteforcecustom, "hash", "", "", bruteforcecustom);
#endif
