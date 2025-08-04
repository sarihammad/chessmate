#!/usr/bin/env python3
import os
import glob

def get_file_content(file_path):
    """Read file content with error handling for encoding issues"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            return f.read()
    except UnicodeDecodeError:
        try:
            with open(file_path, 'r', encoding='latin-1') as f:
                return f.read()
        except:
            return f"Error reading file: {file_path}"

def find_all_source_files():
    """Find all .cpp, .hpp, and .java files in the project"""
    source_files = []
    
    # Game C++ files
    game_cpp_files = [
        'game/main.cpp',
        'game/src/main.cpp',
        'game/src/application/game_controller.cpp',
        'game/src/domain/board.cpp',
        'game/src/domain/game.cpp',
        'game/src/domain/pieces/bishop.cpp',
        'game/src/domain/pieces/king.cpp',
        'game/src/domain/pieces/knight.cpp',
        'game/src/domain/pieces/pawn.cpp',
        'game/src/domain/pieces/queen.cpp',
        'game/src/domain/pieces/rook.cpp',
        'game/src/gui/board_view.cpp',
        'game/src/gui/texture_manager.cpp',
        'game/src/gui/widgets/Button.cpp',
        'game/src/gui/widgets/sprite_piece.cpp',
        'game/src/infrastructure/players/ai_player.cpp',
        'game/src/infrastructure/players/human_player.cpp',
        'game/src/infrastructure/players/network_player.cpp',
        'game/src/infrastructure/network/websocket_client.cpp',
        'game/src/infrastructure/ai/ai_evaluator.cpp',
        'game/src/utils/utils.cpp',
        'game/tests/test_application.cpp',
        'game/tests/test_domain.cpp'
    ]
    
    # Game header files
    game_header_files = [
        'game/include/application/game_controller.hpp',
        'game/include/domain/board.hpp',
        'game/include/domain/enums.hpp',
        'game/include/domain/game.hpp',
        'game/include/domain/gamestate.hpp',
        'game/include/domain/move.hpp',
        'game/include/domain/piece.hpp',
        'game/include/domain/player.hpp',
        'game/include/domain/pieces/bishop.hpp',
        'game/include/domain/pieces/king.hpp',
        'game/include/domain/pieces/knight.hpp',
        'game/include/domain/pieces/movable_piece.hpp',
        'game/include/domain/pieces/pawn.hpp',
        'game/include/domain/pieces/queen.hpp',
        'game/include/domain/pieces/rook.hpp',
        'game/include/gui/board_view.hpp',
        'game/include/gui/gui.cpp',
        'game/include/gui/gui.hpp',
        'game/include/gui/texture_manager.hpp',
        'game/include/gui/widgets/Button.hpp',
        'game/include/gui/widgets/sprite_piece.hpp',
        'game/include/infrastructure/ai/ai_evaluator.hpp',
        'game/include/infrastructure/network/websocket_client.hpp',
        'game/include/infrastructure/players/ai_player.hpp',
        'game/include/infrastructure/players/human_player.hpp',
        'game/include/infrastructure/players/network_player.hpp',
        'game/include/utils/utils.hpp'
    ]
    
    # Java files
    java_files = [
        'server/src/main/java/com/devign/chessmate/ChessmateServerApplication.java',
        'server/src/main/java/com/devign/chessmate/config/WebSocketConfig.java',
        'server/src/main/java/com/devign/chessmate/service/MatchmakingService.java',
        'server/src/main/java/com/devign/chessmate/socket/GameSocketHandler.java',
        'server/src/test/java/com/devign/chessmate/ChessmateServerApplicationTests.java'
    ]
    
    all_files = game_cpp_files + game_header_files + java_files
    
    # Filter out files that don't exist
    existing_files = []
    for file_path in all_files:
        if os.path.exists(file_path):
            existing_files.append(file_path)
        else:
            print(f"Warning: File not found: {file_path}")
    
    return existing_files

def create_combined_file():
    """Create a combined file with all source code"""
    files = find_all_source_files()
    
    with open('combined_source_code.txt', 'w', encoding='utf-8') as output_file:
        output_file.write("CHESSMATE PROJECT - COMBINED SOURCE CODE\n")
        output_file.write("=" * 50 + "\n\n")
        
        for file_path in sorted(files):
            print(f"Processing: {file_path}")
            
            content = get_file_content(file_path)
            
            output_file.write(f"\n{'='*80}\n")
            output_file.write(f"FILE: {file_path}\n")
            output_file.write(f"{'='*80}\n\n")
            output_file.write(content)
            output_file.write("\n\n")
    
    print(f"\nCombined file created: combined_source_code.txt")
    print(f"Total files processed: {len(files)}")

if __name__ == "__main__":
    create_combined_file() 