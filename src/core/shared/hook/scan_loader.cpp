#include <includes_shared.hpp>
#include <utils/utils.hpp>
#include <hook/scan_loader.hpp>

namespace hook::scan_loader {

    void PackScan(
        const char* pattern, std::vector<uint8_t>& mask, std::vector<uint8_t>& searched, size_t& delta, const char* name
    ) {
        bool mid{ true };

        mask.clear();
        searched.clear();
        delta = 0;

        // parse pattern
        const char* str{ pattern };
        while (*str) {
            char c = *(str++);
            if (isspace(c)) {
                continue;
            }

            mid = !mid;

            if (c == '?') {
                if (mid) {
                    throw std::runtime_error(
                        utils::va(
                            actssec("Wildcard pattern in half byte! %s (%s)"),
                            pattern,
                            name ? name : actssec("no name")
                        )
                    );
                }
                if (str[0] == '?') {
                    // test if we are in a packed context
                    if (!str[1] || isspace(str[1])) {
                        str++; // consume both ??
                    }
                }
                // consume both
                mid = !mid;
                mask.push_back(0);
                searched.push_back(0);
                continue;
            }

            auto b = utils::ctob(c);

            if (mid) {
                *(mask.end() - 1) |= 0xF;
                *(searched.end() - 1) |= b;
            } else {
                mask.push_back(0xF0);
                searched.push_back(b << 4);
            }
        }

        // reversed because we set it by default to 0
        if (!mid) {
            throw std::runtime_error(
                utils::va(actssec("Scan pattern has half byte! %s (%s)"), pattern, name ? name : actssec("no name"))
            );
        }

        auto it1 = mask.begin();
        auto it2 = searched.begin();

        // clear start
        while (it1 != mask.end()) {
            if (*it1) {
                break;
            }
            it1 = mask.erase(it1);
            it2 = searched.erase(it2);
            delta++;
        }

        if (!mask.size()) {
            throw std::runtime_error(
                std::format(actssec("Empty pattern! %s (%s)"), pattern, name ? name : actssec("no name"))
            );
        }

        // clear end
        auto eit1 = mask.end() - 1;
        auto eit2 = searched.end() - 1;
        while (eit1 != mask.begin()) {
            if (*eit1) {
                break;
            }
            eit1 = mask.erase(eit1) - 1;
            eit2 = searched.erase(eit2) - 1;
        }

        if (!mask.size()) {
            throw std::runtime_error(
                std::format(actssec("Empty pattern! %s (%s)"), pattern, name ? name : actssec("no name"))
            );
        }
    }

} // namespace hook::scan_loader