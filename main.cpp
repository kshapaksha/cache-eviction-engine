
int main()
{
    cout << "============================================\n";
    cout << "       CACHE MANAGEMENT SYSTEM\n";
    cout << "============================================\n";

    int choice;

    do
    {
        cout << "\n--------------- MAIN MENU ---------------\n";
        cout << "1. Page Replacement Algorithms (Mode 1)\n";
        cout << "2. Browser Cache Management (Mode 2)\n";
        cout << "3. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
            cout<<"\n Page Replacement Algorithms:";
                mode1();
                break;

            case 2:
            cout<<"\n Browser Cache Manager:";
                runBrowserCacheSystem();
                break;

            case 3:
            cout<<"\n Exiting...";
                cout << "\nProgram ended.\n";
                 exit(0);
                break;

            default:
                cout << "\nInvalid choice! Please try again.\n";
        }

    } while (choice != 3);

    return 0;
}