#include <iostream>
#include <string>
using namespace std;

struct Result
{
    int hit;
    int miss;
    float hitRatio;
    float missRatio;
};

void showStep(int page, int memory[], int count, int frames, bool found)
{
    cout << "Page " << page << "  |  ";

    for (int i = 0; i < frames; i++)
    {
        if (i < count)
            cout << memory[i] << " ";
        else
            cout << "- ";
    }

    if (found)
        cout << " |  HIT\n";
    else
        cout << " |  FAULT\n";
}

Result makeResult(int hit, int miss, int total)
{
    Result r;
    r.hit = hit;
    r.miss = miss;
    r.hitRatio = (float)hit / total;
    r.missRatio = (float)miss / total;
    return r;
}

int searchPage(int memory[], int count, int page)
{
    for (int i = 0; i < count; i++)
    {
        if (memory[i] == page)
            return i;
    }
    return -1;
}

Result FIFO(int pages[], int n, int frames, bool show)
{
    int memory[10];
    int count = 0;
    int hit = 0, miss = 0;
    int pointer = 0;

    for (int i = 0; i < n; i++)
    {
        int page = pages[i];
        bool found = (searchPage(memory, count, page) != -1);

        if (found)
        {
            hit++;
        }
        else
        {
            miss++;

            if (count < frames)
            {
                memory[count] = page;
                count++;
            }
            else
            {
                memory[pointer] = page;
                pointer = (pointer + 1) % frames;
            }
        }

        if (show)
            showStep(page, memory, count, frames, found);
    }

    return makeResult(hit, miss, n);
}

Result LRU(int pages[], int n, int frames, bool show)
{
    int memory[10];
    int count = 0;
    int hit = 0, miss = 0;

    for (int i = 0; i < n; i++)
    {
        int page = pages[i];
        int pos = searchPage(memory, count, page);
        bool found = (pos != -1);

        if (found)
        {
            hit++;

            for (int j = pos; j < count - 1; j++)
            {
                memory[j] = memory[j + 1];
            }
            memory[count - 1] = page;
        }
        else
        {
            miss++;

            if (count == frames)
            {
             
               for (int j = 0; j < count - 1; j++)
                {
                    memory[j] = memory[j + 1];
                }
                count--;
            }

            memory[count] = page;
            count++;
        }

        if (show)
            showStep(page, memory, count, frames, found);
    }

    return makeResult(hit, miss, n);
}

Result Optimal(int pages[], int n, int frames, bool show)
{
    int memory[10];
    int count = 0;
    int hit = 0, miss = 0;

    for (int i = 0; i < n; i++)
    {
        int page = pages[i];
        bool found = (searchPage(memory, count, page) != -1);

        if (found)
        {
            hit++;
        }
        else
        {
            miss++;

            if (count < frames)
            {
                memory[count] = page;
                count++;
            }
            else
            {
                int replaceIndex = 0;
                int farthest = -1;

                for (int j = 0; j < count; j++)
                {
                    int nextUse = -1;

                    for (int k = i + 1; k < n; k++)
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

        if (show)
            showStep(page, memory, count, frames, found);
    }

    return makeResult(hit, miss, n);
}

void displayResult(string algorithm, Result r)
{
    cout << "\n " << algorithm << " \n";
    cout << "Page Hits   : " << r.hit << endl;
    cout << "Page Misses : " << r.miss << endl;
    cout << "Hit Ratio   : " << r.hitRatio << endl;
    cout << "Miss Ratio  : " << r.missRatio << endl;
}

void compareAll(int pages[], int n, int frames)
{
    Result fifo = FIFO(pages, n, frames, false);
    Result lru = LRU(pages, n, frames, false);
    Result optimal = Optimal(pages, n, frames, false);

    displayResult("FIFO", fifo);
    displayResult("LRU", lru);
    displayResult("Optimal", optimal);

    cout << "\n BEST ALGORITHM \n";

    if (optimal.hit >= fifo.hit && optimal.hit >= lru.hit)
        cout << "Best: Optimal (needs to know the future, so it is only theoretical)\n";
    else if (lru.hit >= fifo.hit)
        cout << "Best: LRU\n";
    else
        cout << "Best: FIFO\n";

    cout << "LRU is the best practical choice.\n";
}

void mode1()
{
    int n, frames;
    int pages[50];

    cout << "\n MODE 1: PAGE REPLACEMENT \n";

    cout << "Enter number of pages (1 to 50): ";
    cin >> n;

    if (n < 1 || n > 50)
    {
        cout << "Invalid number of pages!\n";
        return;
    }

    cout << "Enter page reference string:\n";
    for (int i = 0; i < n; i++)
    {
        cin >> pages[i];
    }

    cout << "Enter number of frames (1 to 10): ";
    cin >> frames;

    if (frames < 1 || frames > 10)
    {
        cout << "Invalid number of frames!\n";
        return;
    }

    int choice;

    do
    {
        cout << "\n Page Replacement Menu \n";
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
            cout << "\n FIFO Steps \n";
            displayResult("FIFO", FIFO(pages, n, frames, true));
            break;

        case 2:
            cout << "\n LRU Steps \n";
            displayResult("LRU", LRU(pages, n, frames, true));
            break;

        case 3:
            cout << "\n Optimal Steps \n";
            displayResult("Optimal", Optimal(pages, n, frames, true));
            break;

        case 4:
            compareAll(pages, n, frames);
            break;

        case 5:
            cout << "Exiting Mode 1...\n";
            break;

        default:
            cout << "Invalid choice!\n";
        }

    } while (choice != 5);
}