#pragma once

enum Cell { CELL_EMPTY, CELL_BLACK, CELL_WHITE };

constexpr int SCREEN_WIDTH = 1050;
constexpr int SCREEN_HEIGHT = 850;

constexpr int BOARD_WIDTH = 800;
constexpr int BOARD_HEIGHT = 800;
constexpr int GRID_SIZE = 19;
constexpr int BOARD_MARGIN = 40;
constexpr int CELL_SIZE = (BOARD_HEIGHT - 2 * BOARD_MARGIN) / (GRID_SIZE - 1);