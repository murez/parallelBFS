#ifndef PENNANT_H
#define PENNANT_H

#include <cstddef>
#include <memory>
#include <cassert>

class Pennant
{
public:
	constexpr Pennant(int vertex) noexcept
		: vertex(vertex), size(1) {}

	Pennant(const Pennant& other);

	Pennant(Pennant&& other) = default;

	Pennant& operator = (const Pennant& other);

	Pennant& operator = (Pennant&& other) = default;

	constexpr int getVertex() const noexcept { assert(size > 0);  return vertex; }

	constexpr std::size_t getSize() const noexcept { return size; }

	std::unique_ptr<Pennant> split() noexcept;

	void merge(std::unique_ptr<Pennant> other);

	// TODO: perhaps implement an overloaded version of merge:
	// Pennant & merge(Pennant &&other);

private:
	int vertex;
	std::size_t size;
	std::unique_ptr<Pennant> left, right;
};	// Pennant

#endif // PENNANT_H