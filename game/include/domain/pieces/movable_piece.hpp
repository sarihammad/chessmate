#pragma once

namespace chess {

class MovablePiece {
public:
    virtual void markMoved() = 0;
    virtual bool hasMovedAlready() const = 0;
    virtual ~MovablePiece() = default;
};

}
