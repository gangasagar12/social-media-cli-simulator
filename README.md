## SocialShell /social-media-cli-simulator
### SocialShell is a Command Line Interface (CLI)-based social media simulator that replicates the some functionalities of popular social platforms in a text-based environment.
To mimic the some features of popular social platforms (like posting, following, liking) without a graphical interface.

## 🚀 Key Features

| Feature Category | Description |
|----------------|-------------|
| **User Registration & Authentication** | Secure user registration and login system with hashed password storage and masked password input. |
| **Cross-Platform Console Support** | Works on both Windows and Linux with platform-specific handling for screen clearing and password masking. |
| **User Profiles** | Each user has a profile containing username, bio, followers, following, post count, join date, and last login timestamp. |
| **Post Management** | Users can create, edit, delete, and view posts with full content or summarized previews. |
| **Timestamped Activity** | Automatic timestamping for account creation, login, post creation, post edits, and comments. |
| **Like & Comment System** | Users can like posts and add timestamped comments to any public post. |
| **Followers & Following System** | Users can follow or unfollow other users with real-time follower/following updates. |
| **Public Feed** | Displays all posts in reverse chronological order (newest first) with summaries and interaction options. |
| **Binary File Persistence** | All users and posts are stored securely using binary files for faster I/O and data integrity. |
| **Backward Compatibility** | Automatically loads legacy `.txt` data files and converts them into binary format. |
| **Efficient Data Serialization** | Custom serialization and parsing logic for users, posts, likes, comments, and relationships. |
| **Search Functionality** | Allows searching users by username keywords. |
| **Robust Input Validation** | Handles invalid inputs, empty fields, duplicate usernames, and incorrect menu choices safely. |
| **Object-Oriented Design** | Clean OOP structure using classes, inheritance, encapsulation, and polymorphism. |
| **Menu-Driven Interface** | Simple, intuitive, text-based menu system for easy navigation and usability. |
| **Automatic Data Saving** | Data is saved automatically on application exit using destructors. |

## 🛠 Technologies Used

- **Language:** C++
- **Paradigm:** Object-Oriented Programming (OOP)
- **File Handling:** Binary files (`.bin`)
- **Standard Libraries:** STL (vector, string, algorithm, functional, fstream)
- **Platform APIs:** Windows (`conio.h`), Linux (`termios.h`)

## 🛠 Installation & Usage Guide

### Prerequisites
- **C++ Compiler:** A C++17 compatible compiler (e.g., GCC, Clang, MSVC)  
- **CMake:** Version 3.16 or higher  

---

### Build Instructions (Structured Project Mode)

1. Clone or download the repository:
2. Open a terminal and navigate to the project directory.
3. Create a build directory and compile the project:

```bash
mkdir build
cd build
cmake ..
cmake --build .
---
## Running the application
after the sucessfully built  you can run application from the built directory
### On window
```bash
.\Socialmedia.exe
---
## On lunux/macOS
./Socialmedia

