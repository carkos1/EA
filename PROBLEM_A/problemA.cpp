#include <bits/stdc++.h>
using namespace std;

#ifdef LOCAL
// Redirect standard input from "inputmd.txt" when testing locally.
void redirectStdin() {
    if (!freopen("inputmd.txt", "r", stdin)) {
        cerr << "Error: cannot open input file 'inputmd.txt'\n";
        exit(1);
    }
}
#endif

// Maximum candidate cells for a 15x15 grid (225 cells) – 256 is a safe upper bound.
const int MAX_CAND = 256;

struct Outpost {
    int r, c, req;
    vector<int> adj; // candidate indices adjacent to this outpost
};

struct Candidate {
    int r, c;
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    #ifdef LOCAL
        redirectStdin();
    #endif

    int T;
    cin >> T;
    while(T--){
        int R, C;
        cin >> R >> C;
        vector<string> grid(R);
        for (int i = 0; i < R; i++){
            cin >> grid[i];
        }
        
        // Build candidate list: all empty cells ('.') are potential turret placements.
        vector<Candidate> candidates;
        vector<vector<int>> candIndex(R, vector<int>(C, -1));
        for (int i = 0; i < R; i++){
            for (int j = 0; j < C; j++){
                if(grid[i][j] == '.'){
                    int idx = candidates.size();
                    candidates.push_back({i, j});
                    candIndex[i][j] = idx;
                }
            }
        }
        int E = candidates.size();
        
        // If grid contains no obstacles (walls/outposts), answer is simply max(R, C).
        bool hasObstacle = false;
        for (int i = 0; i < R && !hasObstacle; i++){
            for (int j = 0; j < C; j++){
                if(grid[i][j] != '.'){
                    hasObstacle = true;
                    break;
                }
            }
        }
        if(!hasObstacle){
            cout << max(R, C) << "\n";
            continue;
        }
        
        // Precompute each candidate’s coverage bitset:
        // A turret covers its own cell plus all cells in the four directions until an obstacle is hit.
        vector<bitset<MAX_CAND>> candCoverage(E), candConflict(E);
        for (int i = 0; i < E; i++){
            candCoverage[i].reset();
            candCoverage[i].set(i);
        }
        int dr[4] = {0, 0, 1, -1};
        int dc[4] = {1, -1, 0, 0};
        for (int i = 0; i < E; i++){
            int r = candidates[i].r, c = candidates[i].c;
            for (int d = 0; d < 4; d++){
                int nr = r + dr[d], nc = c + dc[d];
                while(nr >= 0 && nr < R && nc >= 0 && nc < C && grid[nr][nc] == '.'){
                    int idx = candIndex[nr][nc];
                    candCoverage[i].set(idx);
                    nr += dr[d];
                    nc += dc[d];
                }
            }
        }
        // Conflict bitset: a turret cannot be placed where another turret’s line‐of‐sight (coverage) exists.
        for (int i = 0; i < E; i++){
            candConflict[i] = candCoverage[i];
            candConflict[i].reset(i);
        }
        
        // fullCoverage: all empty candidate cells must be covered.
        bitset<MAX_CAND> fullCoverage;
        fullCoverage.reset();
        for (int i = 0; i < E; i++){
            fullCoverage.set(i);
        }
        
        // Process outposts: these are cells containing a digit ('0' to '4') indicating how many adjacent turrets are required.
        vector<Outpost> outposts;
        for (int i = 0; i < R; i++){
            for (int j = 0; j < C; j++){
                if(isdigit(grid[i][j])){
                    Outpost op;
                    op.r = i; op.c = j;
                    op.req = grid[i][j] - '0';
                    int dr2[4] = {0, 0, 1, -1};
                    int dc2[4] = {1, -1, 0, 0};
                    for (int d = 0; d < 4; d++){
                        int ni = i + dr2[d], nj = j + dc2[d];
                        if(ni >= 0 && ni < R && nj >= 0 && nj < C && grid[ni][nj] == '.'){
                            int idx = candIndex[ni][nj];
                            op.adj.push_back(idx);
                        }
                    }
                    outposts.push_back(op);
                }
            }
        }
        
        // For each candidate, note which outposts it is adjacent to.
        vector<vector<int>> candToOutposts(E);
        for (int opIdx = 0; opIdx < (int)outposts.size(); opIdx++){
            for (int cand : outposts[opIdx].adj){
                candToOutposts[cand].push_back(opIdx);
            }
        }
        
        // Order candidate indices by a heuristic: those covering more cells come first.
        vector<int> candOrder(E);
        for (int i = 0; i < E; i++){
            candOrder[i] = i;
        }
        sort(candOrder.begin(), candOrder.end(), [&](int a, int b) {
            return candCoverage[a].count() > candCoverage[b].count();
        });
        int nOrder = candOrder.size();
        
        // Use iterative deepening DFS to select a subset of turret placements that:
        // 1) Cover all empty cells,
        // 2) Satisfy each outpost’s required adjacent turret count exactly,
        // 3) Do not have conflicting placements.
        bool solutionFound = false;
        int best = INT_MAX;
        vector<int> initOutCount(outposts.size(), 0);
        
        function<void(int, int, bitset<MAX_CAND>, bitset<MAX_CAND>, vector<int>&)> dfs = 
        [&](int pos, int count, bitset<MAX_CAND> currCoverage, bitset<MAX_CAND> forbidden, vector<int>& outCount) {
            if(solutionFound) return;
            if(count == best){
                if(currCoverage == fullCoverage){
                    bool valid = true;
                    for (int i = 0; i < (int)outposts.size(); i++){
                        if(outCount[i] != outposts[i].req) { valid = false; break; }
                    }
                    if(valid) {
                        solutionFound = true;
                    }
                }
                return;
            }
            if(count + (nOrder - pos) < best) return;
            bitset<MAX_CAND> potential = currCoverage;
            for (int i = pos; i < nOrder; i++){
                int candIdx = candOrder[i];
                if(!forbidden.test(candIdx))
                    potential |= candCoverage[candIdx];
            }
            if(potential != fullCoverage) return;
            
            for (int i = pos; i < nOrder; i++){
                int candIdx = candOrder[i];
                if(forbidden.test(candIdx)) continue;
                
                bitset<MAX_CAND> newCoverage = currCoverage | candCoverage[candIdx];
                bitset<MAX_CAND> newForbidden = forbidden;
                newForbidden.set(candIdx);
                newForbidden |= candConflict[candIdx];
                
                vector<int> newOutCount = outCount;
                for (int opIdx : candToOutposts[candIdx]){
                    newOutCount[opIdx]++;
                    if(newOutCount[opIdx] > outposts[opIdx].req)
                        goto skip_candidate;
                }
                for (int opIdx = 0; opIdx < (int)outposts.size(); opIdx++){
                    int needed = outposts[opIdx].req - newOutCount[opIdx];
                    int possible = 0;
                    for (int cand : outposts[opIdx].adj) {
                        if(!newForbidden.test(cand)) possible++;
                    }
                    if(possible < needed)
                        goto skip_candidate;
                }
                
                dfs(i+1, count+1, newCoverage, newForbidden, newOutCount);
                if(solutionFound) return;
            skip_candidate:
                ;
            }
        };
        
        for (int k = 1; k <= E; k++){
            best = k;
            solutionFound = false;
            bitset<MAX_CAND> initCoverage;
            initCoverage.reset();
            bitset<MAX_CAND> initForbidden;
            initForbidden.reset();
            dfs(0, 0, initCoverage, initForbidden, initOutCount);
            if(solutionFound){
                cout << k << "\n";
                break;
            }
        }
        if(!solutionFound)
            cout << "noxus will rise!" << "\n";
    }
    return 0;
}
