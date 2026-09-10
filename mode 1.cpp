#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Result
{
    int hit;
    int miss;
    float hitRatio;
    float missRatio;
};

// ---------------- FIFO ----------------
Result FIFO(vector<int> pages, int frames)
{
    vector<int> memory;
    int hit = 0, miss = 0;
    int pointer = 0;

    for (int page : pages)
    {
        bool found = false;

        for (int x : memory)
        {
            if (x == page)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            hit++;
        }
        else
        {
            miss++;

            if (memory.size() < frames)
            {
                memory.push_back(page);
            }
            else
            {
                memory[pointer] = page;
                pointer = (pointer + 1) % frames;
            }
        }
    }

    Result r;
    r.hit = hit;
    r.miss = miss;
    r.hitRatio = (float)hit / pages.size();
    r.missRatio = (float)miss / pages.size();

    return r;
}

// ---------------- LRU ----------------
Result LRU(vector<int> pages, int frames)
{
    vector<int> memory;
    int hit = 0, miss = 0;

    for (int page : pages)
    {
        auto it = find(memory.begin(), memory.end(), page);

        if (it != memory.end())
        {
            hit++;

            memory.erase(it);
            memory.push_back(page);
        }
        else
        {
            miss++;

            if (memory.size() == frames)
            {
                memory.erase(memory.begin());
            }

            memory.push_back(page);
        }
    }

    Result r;
    r.hit = hit;
    r.miss = miss;
    r.hitRatio = (float)hit / pages.size();
    r.missRatio = (float)miss / pages.size();

    return r;
}

// ---------------- Optimal ----------------
Result Optimal(vector<int> pages, int frames)
{
    vector<int> memory;
    int hit = 0, miss = 0;

    for (int i = 0; i < pages.size(); i++)
    {
        int page = pages[i];

        if (find(memory.begin(), memory.end(), page) != memory.end())
        {
            hit++;
            continue;
        }

        miss++;

        if (memory.size() < frames)
        {
            memory.push_back(page);
        }
        else
        {
            int replaceIndex = -1;
            int farthest = -1;

            for (int j = 0; j < memory.size(); j++)
            {
                int nextUse = -1;

                for (int k = i + 1; k < pages.size(); k++)
                {
                    if (pages[k] == memory[j])
                    {
                        nextUse = k;
                        break;
                    }
                }

                if (nextUse == -1)
                {
                    replaceIndex = j;
                    break;
                }

                if (nextUse > farthest)
                {
                    farthest = nextUse;
                    replaceIndex = j;
                }
            }

            memory[replaceIndex] = page;
        }
    }

    Result r;
    r.hit = hit;
    r.miss = miss;
    r.hitRatio = (float)hit / pages.size();
    r.missRatio = (float)miss / pages.size();

    return r;
}

// ---------------- Display Result ----------------
void displayResult(string algorithm, Result r)
{
    cout << "\n===== " << algorithm << " =====\n";
    cout << "Page Hits   : " << r.hit << endl;
    cout << "Page Misses : " << r.miss << endl;
    cout << "Hit Ratio   : " << r.hitRatio << endl;
    cout << "Miss Ratio  : " << r.missRatio << endl;
}

// ---------------- Compare All ----------------
void compareAll(vector<int> pages, int frames)
{
    Result fifo = FIFO(pages, frames);
    Result lru = LRU(pages, frames);
    Result optimal = Optimal(pages, frames);

    displayResult("FIFO", fifo);
    displayResult("LRU", lru);
    displayResult("Optimal", optimal);

    cout << "\n===== BEST ALGORITHM =====\n";

    if (optimal.hit >= fifo.hit && optimal.hit >= lru.hit)
        cout << "Best: Optimal" << endl;
    else if (lru.hit >= fifo.hit)
        cout << "Best: LRU" << endl;
    else
        cout << "Best: FIFO" << endl;
}

// ---------------- MODE 1 ----------------
void mode1()
{
    int n, frames;

    cout << "\n===== MODE 1: PAGE REPLACEMENT =====\n";

    cout << "Enter number of pages: ";
    cin >> n;

    vector<int> pages(n);

    cout << "Enter page reference string:\n";
    for (int i = 0; i < n; i++)
    {
        cin >> pages[i];
    }

    cout << "Enter number of frames: ";
    cin >> frames;

    int choice;

    do
    {
        cout << "\n----- Page Replacement Menu -----\n";
        cout << "1. FIFO\n";
        cout << "2. LRU\n";
        cout << "3. Optimal\n";
        cout << "4. Compare All\n";
        cout << "5. Exit Mode 1\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            displayResult("FIFO", FIFO(pages, frames));
            break;

        case 2:
            displayResult("LRU", LRU(pages, frames));
            break;

        case 3:
            displayResult("Optimal", Optimal(pages, frames));
            break;

        case 4:
            compareAll(pages, frames);
            break;

        case 5:
            cout << "Exiting Mode 1...\n";
            break;

        default:
            cout << "Invalid choice!\n";
        }

    } while (choice != 5);
}