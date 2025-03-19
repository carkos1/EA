#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <climits>
using namespace std;

const int MAX_CAND = 256;

struct Outpost {
    int r, c, req;
    vector<int> adj; // adjacent candidate indices
};

struct Candidate {
    int r, c;
};

void readGrid(int R, int C, vector<string>& grid);
vector<Candidate> getCandidates(int R, int C, const vector<string>& grid, vector<vector<int>>& candIndex);
bool hasObstacle(int R, int C, const vector<string>& grid);
void computeCoverage(int R, int C, const vector<string>& grid, const vector<Candidate>& candidates,vector<bitset<MAX_CAND>>& candCoverage, vector<bitset<MAX_CAND>>& candConflict, vector<vector<int>>& candIndex);
vector<Outpost> getOutposts(int R, int C, const vector<string>& grid, const vector<vector<int>>& candIndex);
void dfs(int pos, int count, bitset<MAX_CAND> currCoverage, bitset<MAX_CAND> forbidden, vector<int>& outCount,const vector<bitset<MAX_CAND>>& candCoverage, const vector<bitset<MAX_CAND>>& candConflict,const vector<Outpost>& outposts, const vector<vector<int>>& candToOutposts,bitset<MAX_CAND> fullCoverage, const vector<int>& candOrder, int& best, bool& solutionFound);

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int R, C;
        cin >> R >> C;
        vector<string> grid(R);
        readGrid(R, C, grid);

        // Build candidate list for turret placements (cells with '.')
        vector<vector<int>> candIndex(R, vector<int>(C, -1));
        vector<Candidate> candidates = getCandidates(R, C, grid, candIndex);
        int E = candidates.size();

        // If grid has no obstacles (i.e. all '.'), answer is max(R, C)
        if (!hasObstacle(R, C, grid)) {
            cout << max(R, C) << "\n";
            continue;
        }

        // Precompute coverage and conflict bitsets for each candidate.
        vector<bitset<MAX_CAND>> candCoverage(E), candConflict(E);
        computeCoverage(R, C, grid, candidates, candCoverage, candConflict, candIndex);

        // Process outposts (cells with digits) and record their adjacent candidate indices.
        vector<Outpost> outposts = getOutposts(R, C, grid, candIndex);

        // For each candidate, record which outposts it is adjacent to.
        vector<vector<int>> candToOutposts(E);
        for (int opIdx = 0; opIdx < (int)outposts.size(); opIdx++) {
            for (int cand : outposts[opIdx].adj) {
                candToOutposts[cand].push_back(opIdx);
            }
        }

        // Order candidate indices by heuristic: those covering more cells come first.
        vector<int> candOrder(E);
        for (int i = 0; i < E; i++)
            candOrder[i] = i;
        sort(candOrder.begin(), candOrder.end(), [&](int a, int b) {
            return candCoverage[a].count() > candCoverage[b].count();
            });

        // fullCoverage: bitset with all candidate cells set.
        bitset<MAX_CAND> fullCoverage;
        fullCoverage.reset();
        for (int i = 0; i < E; i++) {
            fullCoverage.set(i);
        }

        bool solutionFound = false;
        int best = INT_MAX;
        vector<int> initOutCount(outposts.size(), 0);

        // Iterative deepening: try turret counts from 1 to E.
        for (int k = 1; k <= E; k++) {
            best = k;
            solutionFound = false;
            bitset<MAX_CAND> initCoverage, initForbidden;
            initCoverage.reset();
            initForbidden.reset();
            dfs(0, 0, initCoverage, initForbidden, initOutCount,
                candCoverage, candConflict, outposts, candToOutposts,
                fullCoverage, candOrder, best, solutionFound);
            if (solutionFound) {
                cout << k << "\n";
                break;
            }
        }
        if (!solutionFound)
            cout << "noxus will rise!" << "\n";
    }
    return 0;
}

void readGrid(int R, int C, vector<string>& grid) {
    for (int i = 0; i < R; i++)
        cin >> grid[i];
}

vector<Candidate> getCandidates(int R, int C, const vector<string>& grid, vector<vector<int>>& candIndex) {
    vector<Candidate> candidates;
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            if (grid[i][j] == '.') {
                int idx = candidates.size();
                candidates.push_back({ i, j });
                candIndex[i][j] = idx;
            }
        }
    }
    return candidates;
}

bool hasObstacle(int R, int C, const vector<string>& grid) {
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            if (grid[i][j] != '.')
                return true;
        }
    }
    return false;
}

void computeCoverage(int R, int C, const vector<string>& grid, const vector<Candidate>& candidates,
    vector<bitset<MAX_CAND>>& candCoverage, vector<bitset<MAX_CAND>>& candConflict,
    vector<vector<int>>& candIndex) {
    int dr[4] = { 0, 0, 1, -1 };
    int dc[4] = { 1, -1, 0, 0 };
    int E = candidates.size();
    for (int i = 0; i < E; i++) {
        int r = candidates[i].r, c = candidates[i].c;
        candCoverage[i].reset();
        candCoverage[i].set(i); // covers itself
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d], nc = c + dc[d];
            while (nr >= 0 && nr < R && nc >= 0 && nc < C && grid[nr][nc] == '.') {
                candCoverage[i].set(candIndex[nr][nc]);
                nr += dr[d];
                nc += dc[d];
            }
        }
        // Conflict bitset: same as coverage but without itself.
        candConflict[i] = candCoverage[i];
        candConflict[i].reset(i);
    }
}

vector<Outpost> getOutposts(int R, int C, const vector<string>& grid, const vector<vector<int>>& candIndex) {
    vector<Outpost> outposts;
    int dr[4] = { 0, 0, 1, -1 };
    int dc[4] = { 1, -1, 0, 0 };
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            if (isdigit(grid[i][j])) {
                Outpost op = { i, j, grid[i][j] - '0' };
                for (int d = 0; d < 4; d++) {
                    int ni = i + dr[d], nj = j + dc[d];
                    if (ni >= 0 && ni < R && nj >= 0 && nj < C && grid[ni][nj] == '.') {
                        op.adj.push_back(candIndex[ni][nj]);
                    }
                }
                outposts.push_back(op);
            }
        }
    }
    return outposts;
}

void dfs(int pos, int count, bitset<MAX_CAND> currCoverage, bitset<MAX_CAND> forbidden, vector<int>& outCount,
    const vector<bitset<MAX_CAND>>& candCoverage, const vector<bitset<MAX_CAND>>& candConflict,
    const vector<Outpost>& outposts, const vector<vector<int>>& candToOutposts,
    bitset<MAX_CAND> fullCoverage, const vector<int>& candOrder, int& best, bool& solutionFound) {
    if (solutionFound)
        return;
    if (count == best) {
        if (currCoverage == fullCoverage) {
            bool valid = true;
            for (size_t i = 0; i < outposts.size(); i++) {
                if (outCount[i] != outposts[i].req) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                solutionFound = true;
            }
        }
        return;
    }
    int nOrder = candOrder.size();
    if (count + (nOrder - pos) < best)
        return;

    // Compute potential coverage from current state plus all remaining available candidates.
    bitset<MAX_CAND> potential = currCoverage;
    for (int i = pos; i < nOrder; i++) {
        int candIdx = candOrder[i];
        if (!forbidden.test(candIdx))
            potential |= candCoverage[candIdx];
    }
    if (potential != fullCoverage)
        return;

    // Try adding candidates from the ordered list.
    for (int i = pos; i < nOrder; i++) {
        int candIdx = candOrder[i];
        if (forbidden.test(candIdx))
            continue;

        bitset<MAX_CAND> newCoverage = currCoverage | candCoverage[candIdx];
        bitset<MAX_CAND> newForbidden = forbidden;
        newForbidden.set(candIdx);
        newForbidden |= candConflict[candIdx]; // prevent conflicting placements

        vector<int> newOutCount = outCount;
        bool skipCandidate = false;
        // Update outpost counts for outposts adjacent to candIdx.
        for (int opIdx : candToOutposts[candIdx]) {
            newOutCount[opIdx]++;
            if (newOutCount[opIdx] > outposts[opIdx].req) {
                skipCandidate = true;
                break;
            }
        }
        if (skipCandidate)
            continue;

        // For each outpost, check that it's still possible to meet its required count.
        for (size_t opIdx = 0; opIdx < outposts.size(); opIdx++) {
            int needed = outposts[opIdx].req - newOutCount[opIdx];
            int possible = 0;
            for (int cand : outposts[opIdx].adj) {
                if (!newForbidden.test(cand))
                    possible++;
            }
            if (possible < needed) {
                skipCandidate = true;
                break;
            }
        }
        if (skipCandidate)
            continue;

        dfs(i + 1, count + 1, newCoverage, newForbidden, newOutCount,
            candCoverage, candConflict, outposts, candToOutposts, fullCoverage, candOrder, best, solutionFound);
        if (solutionFound)
            return;
    }
}
