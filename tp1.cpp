#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int MAX_N = 15;
const int INF = 1e9;

struct Position {
    int r, c;
};

int R, C, minTurrets;
vector<string> grid;
vector<vector<bool>> covered;
vector<Position> outposts;
vector<vector<int>> requiredTurrets;

// Directions (Left, Up, Right, Down)
int dr[] = {0, -1, 0, 1};
int dc[] = {-1, 0, 1, 0};

// Checks if the position is within the grid
bool isValid(int r, int c) {
    return r >= 0 && r < R && c >= 0 && c < C;
}

// Places a turret at (r, c) and marks its coverage
void placeTurret(int r, int c, vector<vector<bool>> &tempCovered) {
    tempCovered[r][c] = true;
    for (int d = 0; d < 4; d++) {
        int nr = r, nc = c;
        while (true) {
            nr += dr[d];
            nc += dc[d];
            if (!isValid(nr, nc) || grid[nr][nc] == '#') break; // Stop at walls
            tempCovered[nr][nc] = true;
        }
    }
}

// Checks if all empty cells are covered
bool allCovered(const vector<vector<bool>> &tempCovered) {
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (grid[r][c] == '.' && !tempCovered[r][c]) return false;
        }
    }
    return true;
}

// Checks if all outpost constraints are met
bool validOutpostPlacement(const vector<vector<int>> &placedTurrets) {
    for (size_t i = 0; i < outposts.size(); i++) {
        int r = outposts[i].r;
        int c = outposts[i].c;
        int count = 0;
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d], nc = c + dc[d];
            if (isValid(nr, nc) && placedTurrets[nr][nc]) count++;
        }
        if (count != requiredTurrets[r][c]) return false;
    }

    return true;
}

// Backtracking function to find the minimum turrets
void solve(int idx, int turretCount, vector<vector<int>> &placedTurrets, vector<vector<bool>> &tempCovered) {
    if (turretCount >= minTurrets) return; // Prune if we already exceed current minimum

    if (idx == R * C) { // Reached end of grid
        if (validOutpostPlacement(placedTurrets) && allCovered(tempCovered)) {
            minTurrets = turretCount;
        }
        return;
    }

    int r = idx / C, c = idx % C;

    // If cell is a wall or an outpost, move to next
    if (grid[r][c] == '#' || (grid[r][c] >= '0' && grid[r][c] <= '4')) {
        solve(idx + 1, turretCount, placedTurrets, tempCovered);
        return;
    }

    // Try without placing a turret
    solve(idx + 1, turretCount, placedTurrets, tempCovered);

    // Try placing a turret
    vector<vector<bool>> newCovered = tempCovered;
    vector<vector<int>> newPlacedTurrets = placedTurrets;

    newPlacedTurrets[r][c] = 1;
    placeTurret(r, c, newCovered);
    
    solve(idx + 1, turretCount + 1, newPlacedTurrets, newCovered);
}

int main() {
    int T;
    cin >> T;

    while (T--) {
        cin >> R >> C;
        grid.resize(R);
        covered.assign(R, vector<bool>(C, false));
        requiredTurrets.assign(R, vector<int>(C, 0));
        outposts.clear();

        for (int r = 0; r < R; r++) {
            cin >> grid[r];
            for (int c = 0; c < C; c++) {
                if (grid[r][c] >= '0' && grid[r][c] <= '4') {
                    requiredTurrets[r][c] = grid[r][c] - '0';
                    outposts.push_back({r, c});
                }
            }
        }

        // Initialize minimum turret count to a large number
        minTurrets = INF;

        // Start backtracking
        vector<vector<int>> placedTurrets(R, vector<int>(C, 0));
        solve(0, 0, placedTurrets, covered);

        // Print result
        if (minTurrets == INF) {
            cout << "noxus will rise!\n";
        } else {
            cout << minTurrets << "\n";
        }
    }

    return 0;
}
