#include <includes.hpp>

namespace {
	constexpr uint64_t Fnv0(const char* str, uint64_t prime = hash::IV_TYPE2) {

		const char* d{ str };

		uint64_t hash{};

		while (*d) {
			hash = (hash * prime) ^ *(d++);
		}

		return hash;
	}

	size_t NumOnes(size_t num) {
		size_t c{};

		while (num) {
			c += num & 1;
			num >>= 1;
		}

		return c;
	}

	bool IsPrime(size_t p) {
		if (p == 2) return true;
		if ((p & 1) == 0) return false; // even

		for (size_t i = 3; i * i <= p; i += 2) {
			if ((p % i) == 0) return false;
		}

		return true;
	}

	uint64_t FindPrime(size_t bits) {
		size_t minPrime{ 0 };
		size_t t{ (5 + bits) / 12 };

		for (size_t b = 1; b < 256; b += 2) {
			size_t prime{ (1ull << (8 * t)) + 256 + b };

			constexpr size_t modval = (1ull << 40) - (1ull << 24) - 1;
			constexpr size_t minval = (1ull << 24) + (1ull << 8) + (1ull << 7);

			if (prime % modval <= minval) {
				continue;
			}

			size_t ones{ NumOnes(b) };

			if (ones != 4 && ones != 5) {
				continue; // bad count
			}

			if (!IsPrime(prime)) {
				continue;
			}

			LOG_INFO("{:x} 2^{} + 2^8 + 0x{:x} {}", prime, t * 8, b, ones);
			if (!minPrime || minPrime > prime) {
				minPrime = prime;
			}
		}

		return minPrime;
	}

	int hash_fnv(int argc, const char* argv[]) {

		//LOG_INFO("3: {} {} {} {} {} {}", IsPrime(3), IsPrime(5), IsPrime(6), IsPrime(7), IsPrime(33), IsPrime(31));

		const char* base{ "chongo <Landon Curt Noll> /\\../\\" };

		uint64_t prime{ FindPrime(64) };
		LOG_INFO("FindPrime(64) {:x}", prime);
		LOG_INFO("Fnv0(base)    {:x}", Fnv0(base, prime));
		LOG_INFO("isPrime2      {}", IsPrime(hash::IV_TYPE2));
		//uint64_t prime32{ FindPrime(32) };
		//LOG_INFO("FindPrime(32) {:x}", prime32);
		//LOG_INFO("Fnv0(base)    {:x}", Fnv0(base, prime32) & 0xFFFFFFFF);

		//LOG_INFO("Fnv0-t2(base)    {:x}", Fnv0(base, hash::IV_TYPE2));
		return tool::OK;
	}


	ADD_TOOL(hash_fnv, "hash", " [dir] [output]", "scan hashes in a directory", hash_fnv);
}