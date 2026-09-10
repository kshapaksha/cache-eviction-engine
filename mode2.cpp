#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
using namespace std;


// Function declared in browser_cache_manager.cpp
void runBrowserCacheSystem();

// Functions from other files
void mode1();
void runBrowserCacheSystem();


struct Resource {
    string url = "";
    string type = "Other";
    int sizeKB = 0;
    int accessCount = 0;
    time_t insertTime = 0;
    int ttlSeconds = 0;

    bool isExpired() const {
        return (time(nullptr) - insertTime) > ttlSeconds;
    }
};

struct Node {
    Resource data;
    Node* prev;
    Node* next;
    Node(Resource r) : data(r), prev(nullptr), next(nullptr) {}
};

class CacheManager {
private:
    int capacityKB;
    int defaultTTL;
    int usedKB;
    unordered_map<string, Node*> table;   // hash map: url -> node
    Node* head;                           // dummy; head->next = most recently used
    Node* tail;                           // dummy; tail->prev = least recently used
    long hits, misses, evictions, expiredCount;
    vector<string> opLog;                 // doubles as access/eviction/operation history

    void log(const string& entry) {
        opLog.push_back(entry);
    }

    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void addToFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void evictLRU() {
        if (tail->prev == head) return; // empty
        Node* victim = tail->prev;
        usedKB -= victim->data.sizeKB;
        removeNode(victim);
        table.erase(victim->data.url);
        log("EVICTED: " + victim->data.url);
        delete victim;
        evictions++;
    }

public:
    CacheManager(int capKB, int ttl) {
        capacityKB = capKB;
        defaultTTL = ttl;
        usedKB = 0;
        hits = misses = evictions = expiredCount = 0;
        head = new Node(Resource());
        tail = new Node(Resource());
        head->next = tail;
        tail->prev = head;
    }

    ~CacheManager() {
        Node* curr = head;
        while (curr != nullptr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    // ADD / ACCESS — sizeKB and type only get used if this url isn't already cached
    void requestResource(string url, int sizeKB, string type) {
        auto it = table.find(url);
        if (it != table.end()) {
            Node* node = it->second;
            if (node->data.isExpired()) {
                usedKB -= node->data.sizeKB;
                removeNode(node);
                table.erase(it);
                log("EXPIRED: " + url);
                delete node;
                expiredCount++;
                cout << "Was cached but expired -> treating as fresh fetch.\n";
            } else {
                hits++;
                node->data.accessCount++;
                removeNode(node);
                addToFront(node);
                log("HIT: " + url);
                cout << "Cache HIT: " << url << "\n";
                return;
            }
        }

        misses++;

        // Reject oversized resource BEFORE evicting anything
        if (sizeKB > capacityKB) {
            cout << "Resource is larger than cache capacity. Cannot store it.\n";
            return;
        }

        // Evict LRU items until there's enough room, or the cache is empty
        while (usedKB + sizeKB > capacityKB && head->next != tail) {
            evictLRU();
        }

        Resource r;
        r.url = url;
        r.type = type;
        r.sizeKB = sizeKB;
        r.accessCount = 1;
        r.insertTime = time(nullptr);
        r.ttlSeconds = defaultTTL;
        Node* node = new Node(r);
        addToFront(node);
        table[url] = node;
        usedKB += sizeKB;
        log("MISS+STORED: " + url);
        cout << "Cache MISS: " << url << " (fetched & stored, " << sizeKB << "KB, " << type << ")\n";
    }

    // SEARCH — read-only lookup, does NOT affect LRU order (an admin peek, not a "visit")
    void searchResource(string url) {
        auto it = table.find(url);
        if (it == table.end()) {
            cout << "Not found in cache.\n";
            return;
        }
        Resource r = it->second->data;
        cout << "URL: " << r.url << " | Type: " << r.type
             << " | Size: " << r.sizeKB << "KB"
             << " | Accessed: " << r.accessCount << " times"
             << " | Status: " << (r.isExpired() ? "EXPIRED" : "VALID") << "\n";
    }

    // UPDATE — manually refresh a resource's TTL
    void updateResource(string url) {
        auto it = table.find(url);
        if (it == table.end()) {
            cout << "Not found in cache.\n";
            return;
        }
        Node* node = it->second;
        node->data.insertTime = time(nullptr);
        removeNode(node);
        addToFront(node);
        log("UPDATED: " + url);
        cout << "Refreshed: " << url << "\n";
    }

    // DELETE — manual removal
    void deleteResource(string url) {
        auto it = table.find(url);
        if (it == table.end()) {
            cout << "Not found in cache.\n";
            return;
        }
        Node* node = it->second;
        usedKB -= node->data.sizeKB;
        removeNode(node);
        table.erase(it);
        log("DELETED: " + url);
        delete node;
        cout << "Deleted: " << url << "\n";
    }

    // VIEW ALL — MRU -> LRU, with usage
    void displayAll() {
        cout << "Cache [MRU -> LRU] (" << usedKB << "/" << capacityKB << " KB used):\n";
        Node* curr = head->next;
        if (curr == tail) {
            cout << "  (empty)\n";
            return;
        }
        while (curr != tail) {
            cout << "  " << curr->data.url << " [" << curr->data.type
                 << ", " << curr->data.sizeKB << "KB]";
            if (curr->data.isExpired()) cout << " (expired)";
            cout << "\n";
            curr = curr->next;
        }
    }

    void removeExpired() {
        Node* curr = head->next;
        while (curr != tail) {
            Node* next = curr->next;
            if (curr->data.isExpired()) {
                usedKB -= curr->data.sizeKB;
                removeNode(curr);
                table.erase(curr->data.url);
                log("EXPIRED: " + curr->data.url);
                delete curr;
                expiredCount++;
            }
            curr = next;
        }
        cout << "Expired resources cleared.\n";
    }

    void showStats() {
        long total = hits + misses;
        double ratio = (total == 0) ? 0 : (double)hits / total * 100;
        cout << "Capacity: " << capacityKB << "KB | Used: " << usedKB
             << "KB | Resources: " << table.size() << "\n";
        cout << "Hits: " << hits << "  Misses: " << misses << "  Hit Ratio: " << ratio << "%\n";
        cout << "Evictions: " << evictions << "  Expired removed: " << expiredCount << "\n";
    }

    // Doubles as access history + eviction history + operation log
    void showLog() {
        if (opLog.empty()) {
            cout << "No operations logged yet.\n";
            return;
        }
        cout << "----- Operation Log -----\n";
        for (auto& entry : opLog) cout << "  " << entry << "\n";
    }

    // Most/least accessed, reusing the accessCount already being tracked
    void showAccessReport() {
        if (table.empty()) {
            cout << "Cache is empty.\n";
            return;
        }
        vector<Resource> all;
        for (auto& pair : table) all.push_back(pair.second->data);
        sort(all.begin(), all.end(), [](const Resource& a, const Resource& b) {
            return a.accessCount > b.accessCount;
        });

        cout << "Most accessed:\n";
        for (int i = 0; i < (int)all.size() && i < 3; i++)
            cout << "  " << all[i].url << " (" << all[i].accessCount << " times)\n";

        cout << "Least accessed:\n";
        for (int i = (int)all.size() - 1, count = 0; i >= 0 && count < 3; i--, count++)
            cout << "  " << all[i].url << " (" << all[i].accessCount << " times)\n";
    }
};






void runBrowserCacheSystem() {
    int capKB, ttlSec;
    cout << "Enter cache capacity in KB: ";
    cin >> capKB;
    cout << "Enter default TTL in seconds: ";
    cin >> ttlSec;
    CacheManager cache(capKB, ttlSec);

    int choice;
    string url;
    string typeNames[] = {"HTML", "CSS", "JS", "Image", "Video", "Other"};

    while (true) {
        cout << "\n----- BROWSER CACHE MANAGEMENT -----\n";
        cout << "1. Add / Access Resource (URL)\n";
        cout << "2. Search Resource\n";
        cout << "3. Update (Refresh) Resource\n";
        cout << "4. Delete Resource\n";
        cout << "5. View All Resources\n";
        cout << "6. Remove Expired Resources\n";
        cout << "7. View Stats\n";
        cout << "8. View Operation Log\n";
        cout << "9. Access Report (Most/Least Used)\n";
        cout << "10. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "Enter URL: "; cin >> url;
                int sizeKB;
                cout << "Enter size in KB (only used if this is new): ";
                cin >> sizeKB;
                cout << "Type - 1:HTML 2:CSS 3:JS 4:Image 5:Video 6:Other: ";
                int t; cin >> t;
                string type = (t >= 1 && t <= 6) ? typeNames[t - 1] : "Other";
                cache.requestResource(url, sizeKB, type);
                break;
            }
            case 2:
                cout << "Enter URL: "; cin >> url;
                cache.searchResource(url);
                break;
            case 3:
                cout << "Enter URL: "; cin >> url;
                cache.updateResource(url);
                break;
            case 4:
                cout << "Enter URL: "; cin >> url;
                cache.deleteResource(url);
                break;
            case 5:
                cache.displayAll();
                break;
            case 6:
                cache.removeExpired();
                break;
            case 7:
                cache.showStats();
                break;
            case 8:
                cache.showLog();
                break;
            case 9:
                cache.showAccessReport();
                break;
            case 10:
                return; // back to main.cpp's menu
            default:
                cout << "Invalid choice.\n";
        }
    }
}