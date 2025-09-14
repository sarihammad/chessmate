#include "utils/utils.hpp"

namespace chess {

Color getOtherColor(Color c) {
    return (c == Color::White) ? Color::Black : Color::White;
}

}