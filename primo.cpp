#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// ------------------------
// Global constants and arrays
// ------------------------
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
int dr[] = { 0, -1, 0, 1 };
int dc[] = { -1, 0, 1, 0 };


// ------------------------
// Utility function to check if position is within grid bounds.
// ------------------------
bool isValid(int r, int c) {
    return r >= 0 && r < R && c >= 0 && c < C;
}

// ------------------------
// Function to mark turret coverage starting at (r, c).
// ------------------------
void placeTurret(int r, int c, vector<vector<bool>>& tempCovered) {
    tempCovered[r][c] = true;
    for (int d = 0; d < 4; d++) {
        int nr = r, nc = c;
        while (true) {
            nr += dr[d];
            nc += dc[d];
            if (!isValid(nr, nc) || grid[nr][nc] == '#' || (grid[nr][nc] >= '0' && grid[nr][nc] <= '4'))
                break; // Stop at walls or outposts
            tempCovered[nr][nc] = true;
        }
    }
}

// ------------------------
// Checks if all empty cells are covered.
// ------------------------
bool allCovered(const vector<vector<bool>>& tempCovered) {
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (grid[r][c] == '.' && !tempCovered[r][c])
                return false;
        }
    }
    return true;
}

// ------------------------
// Checks if outpost turret requirements are met.
// ------------------------
bool validOutpostPlacement(const vector<vector<int>>& placedTurrets) {
    for (size_t i = 0; i < outposts.size(); i++) {
        int r = outposts[i].r;
        int c = outposts[i].c;
        int count = 0;
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d], nc = c + dc[d];
            if (isValid(nr, nc) && placedTurrets[nr][nc])
                count++;
        }
        if (count != requiredTurrets[r][c])
            return false;
    }
    return true;
}

// ------------------------
// Checks if placing a turret at (r, c) would conflict with any turret already placed.
// Two turrets conflict if they are in the same row or column with no wall or outpost between them.
// ------------------------
bool isTurretConflict(int r, int c, const vector<vector<int>>& placedTurrets) {
    for (int d = 0; d < 4; d++) {
        int nr = r, nc = c;
        while (true) {
            nr += dr[d];
            nc += dc[d];
            if (!isValid(nr, nc) || grid[nr][nc] == '#' || (grid[nr][nc] >= '0' && grid[nr][nc] <= '4'))
                break;
            if (placedTurrets[nr][nc] == 1)
                return true;
        }
    }
    return false;
}

// ------------------------
// Backtracking function to try turret placements.
// ------------------------
void solve(int idx, int turretCount, vector<vector<int>>& placedTurrets, vector<vector<bool>>& tempCovered) {
    if (turretCount >= minTurrets)
        return; // Prune if we already exceed current minimum

    if (idx == R * C) { // Reached end of grid
        if (validOutpostPlacement(placedTurrets) && allCovered(tempCovered))
            minTurrets = turretCount;
        return;
    }

    int r = idx / C, c = idx % C;

    // Skip walls and outposts
    if (grid[r][c] == '#' || (grid[r][c] >= '0' && grid[r][c] <= '4')) {
        solve(idx + 1, turretCount, placedTurrets, tempCovered);
        return;
    }

    // Option 1: Do not place a turret here
    solve(idx + 1, turretCount, placedTurrets, tempCovered);

    // Option 2: Place a turret here if it does not conflict with others
    if (!isTurretConflict(r, c, placedTurrets)) {
        vector<vector<bool>> newCovered = tempCovered;
        vector<vector<int>> newPlacedTurrets = placedTurrets;
        newPlacedTurrets[r][c] = 1;
        placeTurret(r, c, newCovered);
        solve(idx + 1, turretCount + 1, newPlacedTurrets, newCovered);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

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
                    outposts.push_back({ r, c });
                }
            }
        }

        // Initialize minimum turret count to a large number
        minTurrets = INF;

        vector<vector<int>> placedTurrets(R, vector<int>(C, 0));
        solve(0, 0, placedTurrets, covered);

        // Print result
        if (minTurrets == INF)
            cout << "noxus will rise!\n";
        else
            cout << minTurrets << "\n";
    }
    return 0;
}
