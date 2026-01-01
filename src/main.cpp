#include "App.h"
#include <iostream>
#include <limits>

using namespace std;

int main() {
    App socialApp;
    int choice;
    
    do {
        cout << "\n\t=================================\n";
        cout << "\t     Welcome to SocialShell\n";
        cout << "\t=================================\n";
        cout << "\n\t1. Register user";
        cout << "\n\t2. Login user";
        cout << "\n\t3. Exit";
        cout << "\n\t=================================\n";
        cout << "\n\tEnter choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        
        switch (choice) {
            case 1: 
                socialApp.registerUser(); 
                break;
            case 2: {
                int uid = socialApp.loginUser();
                if (uid != -1) socialApp.userDashboard(uid);
                break;
            }
            case 3:
                cout << "\n\tExiting... Goodbye!\n";
                break;
            default:
                cout << "\n\tInvalid choice. Try again.\n"; 
                break;
        }
    } while (choice != 3);
    
    return 0;
}