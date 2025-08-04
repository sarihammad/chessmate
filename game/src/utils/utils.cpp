#include "utils/utils.hpp"
#include <tuple>

namespace chess {

bool Position::isValid() const {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

bool Position::operator==(const Position& other) const {
    return row == other.row && col == other.col;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}

bool Position::operator<(const Position& other) const {
    return std::tie(row, col) < std::tie(other.row, other.col);
}

Color getOtherColor(Color c) {
    return (c == Color::White) ? Color::Black : Color::White;
}

}