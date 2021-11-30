#include "pennant.h"

#include <stdexcept>
#include <cassert>

Pennant::Pennant(const Pennant &other)
        : vertex(other.vertex), size(other.size),
          left(other.left ? std::make_unique<Pennant>(*other.left) : nullptr),
          right(other.right ? std::make_unique<Pennant>(*other.right) : nullptr) {}

Pennant &Pennant::operator=(const Pennant &other) {
    vertex = other.vertex;
    size = other.size;
    left = other.left ? std::make_unique<Pennant>(*other.left) : nullptr;
    right = other.right ? std::make_unique<Pennant>(*other.right) : nullptr;
    return *this;
}


std::unique_ptr<Pennant> Pennant::split() noexcept {
    if (size < 2)
        return nullptr;
    std::unique_ptr<Pennant> other = std::move(left);
    left = std::move(other->right);
    size >>= 1;
    return other;
}


void Pennant::merge(std::unique_ptr<Pennant> other) {
    if (!other)
        return;

    if (other->size != size)
        throw std::runtime_error("Failed to_pre merge pennants of a different size.");

    other->right = std::move(left);
    left = std::move(other);
    size <<= 1;
}