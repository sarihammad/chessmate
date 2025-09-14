#pragma once

#include "utils/utils.hpp"
#include "piece.hpp"
#include <array>
#include <memory>
#include <optional>

namespace chess {

class Board {
private:
    using Square = std::unique_ptr<Piece>;
    using Rank = std::array<Square, 8>;
    std::array<Rank, 8> squares_{};

public:
    constexpr Board() = default;
    Board(const Board& other);
    Board& operator=(const Board& other);
    Board(Board&&) noexcept = default;
    Board& operator=(Board&&) noexcept = default;
    ~Board() = default;

    [[nodiscard]] const Piece* piece_at(Position pos) const noexcept;
    [[nodiscard]] Piece* piece_at(Position pos) noexcept;
    
    void set_piece(Position pos, std::unique_ptr<Piece> piece) noexcept;
    void move_piece(Position from, Position to) noexcept;
    void clear(Position pos) noexcept;
    
    void setup_initial_position();
    Board clone() const;

    [[nodiscard]] bool is_empty(Position pos) const noexcept;
    [[nodiscard]] bool is_check(Color color) const noexcept;
    
    // Legacy interface for compatibility
    [[nodiscard]] std::shared_ptr<Piece> getPieceAt(const Position& pos) const;
    void setPieceAt(const Position& pos, std::shared_ptr<Piece> piece);
    void movePiece(const Position& from, const Position& to);
    void setupInitialPosition();
    [[nodiscard]] bool isEmpty(const Position& pos) const;
};

}
