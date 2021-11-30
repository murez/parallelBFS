#include "bag.h"
#include "omp.h"

Bag::Bag(const Bag &other) {
    this->pennants.reserve(other.pennants.size());
    for (const std::unique_ptr<Pennant> &pennant: other.pennants) {
        this->pennants.push_back(pennant ? std::make_unique<Pennant>(*pennant) : nullptr);
    }
}

Bag &Bag::operator=(const Bag &other) {
    this->pennants.reserve(other.pennants.size());
    for (const std::unique_ptr<Pennant> &pennant: other.pennants) {
        this->pennants.push_back(pennant ? std::make_unique<Pennant>(*pennant) : nullptr);
    }
    return *this;
}


//void Bag::insert(std::unique_ptr<Pennant> pennant)
void Bag::insert(int vertex) {
    auto new_pennant = std::make_unique<Pennant>(vertex);

    for (auto &pennant: pennants) {
        if (!pennant) {
            pennant = std::move(new_pennant);
            return;
        } else {
            new_pennant->merge(std::move(pennant));
        }
    }
    pennants.push_back(std::move(new_pennant));
}


Bag &Bag::merge(Bag &&other) {
    std::unique_ptr<Pennant> carry;
    if (getSize() <= other.getSize()){
        std::swap(other, *this);
    }
#pragma omp parallel for
    for (auto i = 0; i < other.getSize(); ++i) {
        fullAdd(this->pennants[i], other.pennants[i], carry);
    }
    for (auto i = other.getSize(); carry && i < getSize(); ++i) {
        add(this->pennants[i], carry);
    }
    if (carry)
        this->pennants.push_back(std::move(carry));
    return *this;
}

void Bag::fullAdd(std::unique_ptr<Pennant> &pennant, std::unique_ptr<Pennant> &other,
                  std::unique_ptr<Pennant> &carry) const {
    int state = (pennant ? 4 : 0) + (other ? 2 : 0) + (carry ? 1 : 0);
    switch (state) {
        case 0:
        case 1:
            pennant = std::move(carry);
            break;
        case 2:
            pennant = std::move(other);
            break;
        case 3:
        case 7:
            carry->merge(std::move(other));
            break;
        case 4:
            break;
        case 5:
            carry->merge(std::move(pennant));
            break;
        case 6:
            pennant->merge(std::move(other));
            carry = std::move(pennant);
            break;
    }
    return;
}

void Bag::add(std::unique_ptr<Pennant> &pennant, std::unique_ptr<Pennant> &carry) const {
    if (carry) {
        if (pennant) {
            carry->merge(std::move(pennant));
        } else {
            pennant = std::move(carry);
        }
    }
}