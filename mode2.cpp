#include <iostream>
#include <string>
using namespace std;

struct Page
{
    string url;
    int expiry;         
};

Page cache[50];         
int numPages = 0;       
int capacity, ttl;
int now = 0;            
int hits = 0, misses = 0, evictions = 0, expirations = 0;

int findPage(string url)
{
    for (int i = 0; i < numPages; i++)
    {
        if (cache[i].url == url)
            return i;
    }
    return -1;
}

void removeAt(int pos)
{
    for (int i = pos; i < numPages - 1; i++)
    {
        cache[i] = cache[i + 1];
    }
    numPages--;
}

void addFront(string url)
{
    for (int i = numPages; i > 0; i--)
    {
        cache[i] = cache[i - 1];
    }
    cache[0].url = url;
    cache[0].expiry = now + ttl;
    numPages++;
}

void moveToFront(int pos)
{
    Page temp = cache[pos];

    for (int i = pos; i > 0; i--)
    {
        cache[i] = cache[i - 1];
    }

    cache[0] = temp;
}

void visitPage(string url)
{
    int pos = findPage(url);

    if (pos != -1)
    {
        if (now >= cache[pos].expiry)
        {
            cout << "Cached copy has EXPIRED. Removing it.\n";
            removeAt(pos);
            expirations++;
        }
        else
        {
            hits++;
            cout << "CACHE HIT: loaded from cache ("
                 << cache[pos].expiry - now << "s of TTL left)\n";
            moveToFront(pos);
            return;
        }
    }

    misses++;
    cout << "CACHE MISS: downloading " << url << "...\n";

    if (numPages == capacity)
    {
        cout << "Cache full. Removing least recently used page: "
             << cache[numPages - 1].url << "\n";
        removeAt(numPages - 1);
        evictions++;
    }

    addFront(url);
    cout << "Page saved in cache (TTL " << ttl << "s).\n";
}

void showCache()
{
    cout << "\nCache (" << numPages << "/" << capacity
         << "), most recent first:\n";

    if (numPages == 0)
    {
        cout << "  (empty)\n";
        return;
    }

    for (int i = 0; i < numPages; i++)
    {
        int left = cache[i].expiry - now;
        cout << "  " << i + 1 << ". " << cache[i].url;

        if (left > 0)
            cout << "  [" << left << "s left]\n";
        else
            cout << "  [EXPIRED]\n";
    }
}

bool contains(string text, string word)
{
    int a = text.length();
    int b = word.length();

    for (int i = 0; i + b <= a; i++)
    {
        int j = 0;

        while (j < b && text[i + j] == word[j])
        {
            j++;
        }

        if (j == b)
            return true;
    }
    return false;
}

void searchCache(string word)
{
    bool found = false;

    for (int i = 0; i < numPages; i++)
    {
        if (contains(cache[i].url, word))
        {
            cout << "  Found: " << cache[i].url << "\n";
            found = true;
        }
    }

    if (!found)
        cout << "  No page matches '" << word << "'.\n";
}

void deletePage(string url)
{
    int pos = findPage(url);

    if (pos == -1)
    {
        cout << "Page not in cache.\n";
        return;
    }

    removeAt(pos);
    cout << "Deleted " << url << ".\n";
}

void removeExpired()
{
    int removed = 0;

    for (int i = numPages - 1; i >= 0; i--)
    {
        if (now >= cache[i].expiry)
        {
            removeAt(i);
            expirations++;
            removed++;
        }
    }

    cout << "Removed " << removed << " expired page(s).\n";
}

void showStats()
{
    int total = hits + misses;

    cout << "\n STATISTICS \n";
    cout << "Hits        : " << hits << endl;
    cout << "Misses      : " << misses << endl;
    cout << "Evictions   : " << evictions << endl;
    cout << "Expirations : " << expirations << endl;

    if (total > 0)
        cout << "Hit Ratio   : " << (float)hits / total << endl;
    else
        cout << "Hit Ratio   : 0\n";
}

void mode2()
{
    cout << "\n MODE 2: BROWSER CACHE MANAGER \n";

    cout << "Enter cache capacity (1 to 50): ";
    cin >> capacity;
    cout << "Enter default TTL in seconds: ";
    cin >> ttl;

    if (capacity < 1 || capacity > 50 || ttl < 1)
    {
        cout << "Invalid input!\n";
        return;
    }

    numPages = 0;
    now = 0;
    hits = 0;
    misses = 0;
    evictions = 0;
    expirations = 0;

    int choice;
    string text;
    int number;

    do
    {
        cout << "\n Browser Cache Menu \n";
        cout << "1. Visit a page\n";
        cout << "2. Show cache\n";
        cout << "3. Search cache\n";
        cout << "4. Delete a page\n";
        cout << "5. Remove expired pages\n";
        cout << "6. Simulate time passing\n";
        cout << "7. Change default TTL\n";
        cout << "8. Statistics\n";
        cout << "9. Clear cache\n";
        cout << "0. Exit Mode 2\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "Enter URL: ";
            cin >> text;
            visitPage(text);
            break;

        case 2:
            showCache();
            break;

        case 3:
            cout << "Enter keyword: ";
            cin >> text;
            searchCache(text);
            break;

        case 4:
            cout << "Enter URL to delete: ";
            cin >> text;
            deletePage(text);
            break;

        case 5:
            removeExpired();
            break;

        case 6:
            cout << "Seconds to skip: ";
            cin >> number;
            if (number >= 1)
            {
                now = now + number;
                cout << number << " seconds passed.\n";
            }
            else
                cout << "Invalid number!\n";
            break;

        case 7:
            cout << "New TTL in seconds: ";
            cin >> number;
            if (number >= 1)
            {
                ttl = number;
                cout << "TTL changed (applies to new pages).\n";
            }
            else
                cout << "Invalid TTL!\n";
            break;

        case 8:
            showStats();
            break;

        case 9:
            numPages = 0;
            cout << "Cache cleared.\n";
            break;

        case 0:
            cout << "Exiting Mode 2...\n";
            break;

        default:
            cout << "Invalid choice!\n";
        }

    } while (choice != 0);
}