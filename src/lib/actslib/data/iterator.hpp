#pragma once
#include "../actslib.hpp"

namespace actslib::data::iterator {
	template<typename Type>
	class AIterator {
	public:
		virtual AIterator<Type>& operator++() = 0;
		virtual Type& operator*() = 0;
		virtual operator bool() = 0;
		inline Type* operator->() {
			return &**this;
		}
		inline bool operator!() {
			return !(bool)(*this);
		}
	};

	template<typename Type>
	class EmptyAIterator : public AIterator<Type> {
	public:

		EmptyAIterator<Type>& operator++() override {
			return *this;
		}
		Type& operator*() override {
			throw std::runtime_error("Empty iterator");
		}
		operator bool() override {
			return false;
		}
	};

	template<typename Type, typename ItType>
	class HandleAIterator : public AIterator<Type> {
		ItType it;
		ItType end;
	public:
		HandleAIterator(ItType it, ItType end) : it(it), end(end) {}

		HandleAIterator<Type, ItType>& operator++() override {
			++it;
			return *this;
		}
		Type& operator*() override {
			return *it;
		}
		operator bool() override {
			return it != end;
		}
	};

	template<typename Type>
	class MergeIteratorBasicComp {
	public:
		static constexpr bool Compare(Type& a, Type& b) {
			return a < b;
		}
	};

	template<typename Type, typename Comparator = MergeIteratorBasicComp<Type>>
	class MergeIterator : public AIterator<Type> {
		AIterator<Type>& left;
		AIterator<Type>& right;
		Type* next{};
	public:
		MergeIterator(AIterator<Type>& left, AIterator<Type>& right) : left(left), right(right) {};

		MergeIterator<Type, Comparator>& operator++() override {
			next = nullptr;
			return *this;
		}
		Type& operator*() override {
			if (*this) {
				return *next;
			}
			throw std::runtime_error("Empty iterator");
		}
		operator bool() override {
			if (next) {
				return true; // already computed
			}

			if (!left) {
				if (!right) {
					return false; // empty
				}
				next = &*right;
				++right;
				return true;
			}
			if (!right) {
				next = &*left;
				++left;
				return true;
			}

			// left < right?
			if (Comparator::Compare(*left, *right)) { 
				next = &*left;
				++left;
				return true;
			}
			else {
				next = &*right;
				++right;
				return true;
			}
		}
	};

	template<typename Type, typename InputType, typename Comparator = MergeIteratorBasicComp<Type>>
	class AllocatedMergeAIterator : public AIterator<Type> {
		std::vector<std::shared_ptr<AIterator<Type>>> its{};
		AIterator<Type>* main;

	public:
		AllocatedMergeAIterator(std::vector<InputType>& input, std::function<std::shared_ptr<AIterator<Type>>(InputType&)> map) {
			its.reserve(max(1, input.size() * 2));

			for (InputType& in : input) {
				its.emplace_back(map(in));
			}

			if (its.empty()) {
				its.emplace_back(std::make_shared<EmptyAIterator<Type>>());
			}
			else if (its.size() > 1) {
				// at least 2 elements, we need to merge

				size_t start{};
				size_t len = its.size();

				do {
					for (size_t i = 0; i + 1 < len; i += 2) {
						its.emplace_back(std::make_shared<MergeIterator<Type, Comparator>>(*its[start + i], *its[start + i + 1]));
					}

					if (len & 1) {
						// add last one just in case
						its.emplace_back(its[start + len - 1]);
					}

					start += len;
					len = its.size() - start;
				} while (len > 1);
			}
			main = &*its[its.size() - 1];
		}

		AllocatedMergeAIterator<Type, InputType, Comparator>& operator++() override {
			++(*main);
			return *this;
		}

		Type& operator*() override {
			return **main;
		}

		operator bool() override {
			return (*main);
		}
	};
}