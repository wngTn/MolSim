#pragma once
#include <vector>
#include <functional>

#include "Particle.h"

/*
 * @brief Custom Iterator iterating over all distinct pairs in the Container
 */
struct PairIterator {
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = std::pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>>;
	using pointer = value_type *;
	using reference = value_type;

	/**
	 * @brief Definition of the * operator
	 * @return The particle pair
	 */
	reference operator*() {
		//return std::make_pair(vec[i],vec[j]);
		return {vec[i], vec[j]};
	}

	/**
	 * @brief Prefix incrementation operator
	 * @return the incremented PairIterator
	 */
	PairIterator &operator++() {

		if (j == vec.size() - 1) {
			i++;
			j = i + 1;
		} else {
			j++;
		}
		return *this;
	}

	// postfix
	// CLang tidy bug?
	PairIterator operator++(int) {
		PairIterator tmp = *this;
		++(*this);
		return tmp;
	}

	bool operator==(const PairIterator &b) const {
		const size_t max = vec.size();
		if ((this->i >= max || this->j >= max) && (b.i >= max || b.j >= max)) {
			return true;
		}
		return this->i == b.i && this->j == b.j;
	}

	bool operator!=(const PairIterator &b) const {
		return !(*this == b);
	}

	PairIterator(std::vector<Particle> &vec, size_t i, size_t j) : vec{vec}, i{i}, j{j} {}

private:

	std::vector<Particle> &vec;
	size_t i;
	size_t j;

	friend class ParticleContainer;
};