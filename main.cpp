#include<iostream>
using namespace std;

void mode1();
void mode2();

int main()
{
    int choice = -1;

    while (choice != 0)
    {
        cout<<"\nCACHE MANAGEMENT SYSTEM\n";
        cout<<"1. Page Replacement (FIFO/LRU/Optimal)\n";
        cout << "2. Browser Cache Manager (LRU+TTL)\n";
        cout<<"Enter choice:";
        cin >> choice;

        switch (choice)
        {
            case 1:
                mode1();
                break;

            case 2:
                mode2();
                break;
            case 0:
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}