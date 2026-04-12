#pragma once

namespace bo3::steam {
	typedef uint64_t CSteamID;

	enum EChatEntryType {
		k_EChatEntryTypeChatMsg = 1,
		k_EChatEntryTypeTyping = 2,
		k_EChatEntryTypeInviteGame = 3,
		k_EChatEntryTypeEmote = 4,
		k_EChatEntryTypeLobbyGameStart = 5,
	};
	enum ELobbyType {
		k_ELobbyTypeFriendsOnly = 1,
		k_ELobbyTypePublic = 2,
		k_ELobbyTypeInvisible = 3,
	};
}