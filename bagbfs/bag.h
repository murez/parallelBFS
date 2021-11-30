#ifndef BAG_H
#define BAG_H

#include "pennant.h"

#include <vector>
#include <memory>

class Bag {
public:
    Bag() = default;

    Bag(const Bag &other);

    Bag(Bag &&other) = default;

    Bag &operator=(const Bag &other);

    Bag &operator=(Bag &&other) = default;

    void insert(int vertex);

    bool isEmpty() const noexcept { return pennants.empty(); }

    std::size_t getSize() const noexcept { return pennants.size(); }

    Pennant *getPennant(int index) const { return pennants[index].get(); }

    Bag &merge(Bag &&other);

private:

    void
    fullAdd(std::unique_ptr<Pennant> &pennant, std::unique_ptr<Pennant> &other, std::unique_ptr<Pennant> &carry) const;

    void add(std::unique_ptr<Pennant> &pennant, std::unique_ptr<Pennant> &carry) const;

    std::vector<std::unique_ptr<Pennant>> pennants;
};

#endif