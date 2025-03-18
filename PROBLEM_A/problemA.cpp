#include <iostream>
#include <vector>
#include <string>
#include <climits>
using namespace std;

const int INF = 1e9;
int R, C, minTurrets;
vector<string> grid;

// Directions: left, up, right, down
int dr[] = {0, -1, 0, 1};
int dc[] = {-1, 0, 1, 0};

// Check if (r,c) is inside the grid.
bool isValid(int r, int c) {
    return r >= 0 && r < R && c >= 0 && c < C;
}

// Before placing a turret at (r,c), check that in every direction no turret is visible.
// We scan both forward and backward in each direction (since beams are symmetric)
// and stop the scan when a wall ('#') or an outpost (digit) is encountered.
bool turretConflict(int r, int c, const vector<vector<int>>& placed) {
    // Check forward directions
    for (int d = 0; d < 4; d++) {
        int nr = r, nc = c;
        while (true) {
            nr += dr[d]; nc += dc[d];
            if (!isValid(nr, nc)) break;
            char cell = grid[nr][nc];
            if (cell == '#' || (cell >= '0' && cell <= '4'))
                break;
            if (placed[nr][nc] == 1)
                return true;
        }
    }
    // Check reverse directions (to catch turrets behind the current cell)
    for (int d = 0; d < 4; d++) {
        int nr = r, nc = c;
        while (true) {
            nr -= dr[d]; nc -= dc[d];
            if (!isValid(nr, nc)) break;
            char cell = grid[nr][nc];
            if (cell == '#' || (cell >= '0' && cell <= '4'))
                break;
            if (placed[nr][nc] == 1)
                return true;
        }
    }
    return false;
}

// When a turret is placed at (r,c), update the coverage.
// A turret covers its own cell and extends its beam in each direction
// until a wall or an outpost blocks it.
void updateCoverage(int r, int c, vector<vector<bool>>& cover) {
    cover[r][c] = true;
    for (int d = 0; d < 4; d++) {
        int nr = r, nc = c;
        while (true) {
            nr += dr[d]; nc += dc[d];
            if (!isValid(nr, nc)) break;
            char cell = grid[nr][nc];
            if (cell == '#' || (cell >= '0' && cell <= '4'))
                break;
            cover[nr][nc] = true;
        }
    }
}

// Check that every empty cell ('.') is covered by at least one turret beam.
bool allCovered(const vector<vector<bool>>& cover) {
    for (int i = 0; i < R; i++){
        for (int j = 0; j < C; j++){
            if (grid[i][j] == '.' && !cover[i][j])
                return false;
        }
    }
    return true;
}

// For each outpost (cells with digit '0'-'4'), count adjacent (orthogonal) turrets
// and check that it exactly matches the required number.
bool validOutposts(const vector<vector<int>>& placed) {
    for (int i = 0; i < R; i++){
        for (int j = 0; j < C; j++){
            char cell = grid[i][j];
            if (cell >= '0' && cell <= '4'){
                int required = cell - '0';
                int count = 0;
                for (int d = 0; d < 4; d++){
                    int nr = i + dr[d], nc = j + dc[d];
                    if (isValid(nr, nc) && placed[nr][nc] == 1)
                        count++;
                }
                if (count != required)
                    return false;
            }
        }
    }
    return true;
}

// Backtracking over all grid cells. idx runs from 0 to R*C.
// turretCount holds the number of turrets placed so far.
// 'placed' is a matrix indicating turret placements (1 if turret is placed).
// 'cover' is a matrix tracking which cells are covered by at least one turret beam.
void backtrack(int idx, int turretCount, vector<vector<int>>& placed, vector<vector<bool>>& cover) {
    if (turretCount >= minTurrets)
        return; // prune if already not optimal
    if (idx == R * C) { // reached the end of the grid
        if (allCovered(cover) && validOutposts(placed))
            minTurrets = turretCount;
        return;
    }
    int r = idx / C, c = idx % C;
    // If the cell is not empty (i.e. it's a wall or an outpost), skip it.
    if (grid[r][c] != '.') {
        backtrack(idx + 1, turretCount, placed, cover);
        return;
    }
    // Option 1: Do not place a turret at (r, c)
    backtrack(idx + 1, turretCount, placed, cover);
    
    // Option 2: Try placing a turret here.
    // Check if placing a turret here would conflict with any turret already placed.
    if (!turretConflict(r, c, placed)) {
        // Create copies of the current state.
        vector<vector<int>> newPlaced = placed;
        vector<vector<bool>> newCover = cover;
        newPlaced[r][c] = 1;
        updateCoverage(r, c, newCover);
        backtrack(idx + 1, turretCount + 1, newPlaced, newCover);
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int T;
    cin >> T;
    while (T--){
        cin >> R >> C;
        grid.resize(R);
        for (int i = 0; i < R; i++){
            cin >> grid[i];
        }
        
        minTurrets = INF;
        // Initialize matrices for turret placements and coverage.
        vector<vector<int>> placed(R, vector<int>(C, 0));
        vector<vector<bool>> cover(R, vector<bool>(C, false));
        
        backtrack(0, 0, placed, cover);
        
        if (minTurrets == INF)
            cout << "noxus will rise!\n";
        else
            cout << minTurrets << "\n";
    }
    return 0;
}
