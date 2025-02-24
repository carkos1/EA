#include <stdio.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
using namespace std;

int main(){
    int num,k,pivotf,pivotm;

    cin >> num;
    
    vector <int> guests;

    vector <int> males;

    vector <int> females;

    while(k != 0){
        cin >> k;
        guests.push_back(k);
    }
    for (int guest : guests) {
        if(guest < 0)
            males.push_back(guest);
        if(guest > 0)
            females.push_back(guest);
    }

    sort(males.begin(), males.end());

    sort(females.begin(), females.end());
    
    pivotm = sizeof(males)/2;
    pivotf = sizeof(females)-1;

    while (pivotf>=0)
    {
        if(abs(males.at(pivotm))>=females.at(pivotf))
            pivotm--;
        else{
            for(int i = pivotm - 1;i >= 0;i--){
                if(abs(males.at(pivotm))+abs(males.at(i))==females.at(pivotf)){
                    cout << "Fair";    
                    return 0;
                }
            pivotm++;
            }
        pivotf--;

        }
    }

    pivotm = sizeof(males)-1;
    pivotf = sizeof(females)/2;

    while (pivotm>=0)
    {
        if(abs(females.at(pivotf))>=males.at(pivotm))
            pivotf--;
        else{
            for(int i = pivotf - 1;i >= 0;i--){
                if(abs(females.at(pivotf))+abs(females.at(i))==males.at(pivotm)){
                    cout << "Fair";    
                    return 0;
                }
            pivotf++;
            }
        pivotm--;

        }
    }
    cout << "Rigged";
    return 0;
}