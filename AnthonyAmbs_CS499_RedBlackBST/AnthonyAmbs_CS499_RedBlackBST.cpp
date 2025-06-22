//==========================================================================
// Name         : AnthonyAmbs_CS499_RedBlackBST.cpp 
// Author       : Anthony Ambs
// Description  : Hello World in C++, Ansi-style
//==========================================================================

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std::chrono;

// structure to hold course information
struct Course {
public:
    std::string courseId;
    std::string courseName;
    std::vector<std::string> prerequisiteCourses;
};

// internal structure for tree node
struct Node {
    Course course;
    Node* left;
    Node* right;
    Node* parent;
    std::string color;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
        parent = nullptr;
        color = "RED";
    }

    // initialize with a course
    Node(Course aCourse) :
        Node() {
        course = aCourse;
    }
};

/*
* class containing data members and methods to
* implement a red black binary search tree
*/
class RedBlackTree {
private:
    Node* root;
    Node* addNode(Node* node, Course course);
    void inOrder(Node* node);
    bool validatePrereqs(Node* node);

public:
    RedBlackTree();
    void InOrder();
    bool ValidatePrereqs();
    void Insert(Course course);
    void leftRotate(Node* x);
    void rightRotate(Node* x);
    void fixInsert(Node* x);
    Course Search(std::string courseId);
};

// default constructor
RedBlackTree::RedBlackTree() {
    root = nullptr;
}

// Traverse the tree in order to print courses
void RedBlackTree::InOrder() {
    inOrder(root);
}

// Traverse the tree in order to validate prerequisites
bool RedBlackTree::ValidatePrereqs() {
    bool valid = validatePrereqs(root);
    if (!valid) {
        return false;
    }
    return true;
}

// Insert a course
void RedBlackTree::Insert(Course course) {
    if (root == nullptr) {
        root = new Node(course);
        root->color = "BLACK";
    }
    else {
        Node* insertedNode = addNode(root, course);
        fixInsert(insertedNode);
    }
}

// Rotate node left
void RedBlackTree::leftRotate(Node* x) {
    Node* y = x->right;
    if (y == nullptr) {
        return;
    }
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// Rotate node right
void RedBlackTree::rightRotate(Node* x) {
    Node* y = x->left;
    if (y == nullptr) {
        return;
    }
    x->left = y->right;
    if (y->right != nullptr) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    }
    else if (x == x->parent->right) {
        x->parent->right = y;
    }
    else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

// Fix insertion
void RedBlackTree::fixInsert(Node* x) {
    while (x != root && x->parent->color == "RED") {
        Node* grandparent = x->parent->parent;
        if (grandparent == nullptr) break;

        if (x->parent == grandparent->left) {
            Node* uncle = grandparent->right;
            if (uncle != nullptr && uncle->color == "RED") {
                x->parent->color = "BLACK";
                uncle->color = "BLACK";
                grandparent->color = "RED";
                x = grandparent;
            }
            else {
                if (x == x->parent->right) {
                    x = x->parent;
                    leftRotate(x);
                }
                x->parent->color = "BLACK";
                grandparent->color = "RED";
                rightRotate(grandparent);
            }
        }
        else {
            Node* uncle = grandparent->left;
            if (uncle != nullptr && uncle->color == "RED") {
                x->parent->color = "BLACK";
                uncle->color = "BLACK";
                grandparent->color = "RED";
                x = grandparent;
            }
            else {
                if (x == x->parent->left) {
                    x = x->parent;
                    rightRotate(x);
                }
                x->parent->color = "BLACK";
               grandparent->color = "RED";
                leftRotate(grandparent);
            }
        }
    }
    RedBlackTree::root->color = "BLACK";
}

// Search for a course
Course RedBlackTree::Search(std::string courseId) {
    Node* currNode = root;
    while (currNode != nullptr) {
        if (courseId.compare(currNode->course.courseId) == 0) {
            return currNode->course;
        }
        else if (courseId.compare(currNode->course.courseId) < 0) {
            currNode = currNode->left;
        }
        else {
            currNode = currNode->right;
        }
    }

    Course course;
    return course;
}

// add a course to a node
Node* RedBlackTree::addNode(Node* node, Course course) {
    if (node == nullptr) {
        return new Node(course);
    }

    if (course.courseId < node->course.courseId) {
        Node* leftChild = addNode(node->left, course);
        node->left = leftChild;
        leftChild->parent = node;
    }
    else if (course.courseId > node->course.courseId) {
        Node* rightChild = addNode(node->right, course);
        node->right = rightChild;
        rightChild->parent = node;
    }
    return node;
}

// print the courses in alphanumeric order using inOrder traversal
void RedBlackTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        std::cout << node->course.courseId << ", " << node->course.courseName << std::endl;
        inOrder(node->right);
    }
}

// validate the prerequisite courses for each course 
bool RedBlackTree::validatePrereqs(Node* node) {
    if (node == nullptr) {
        return true;
    }
        
    bool validLeft = validatePrereqs(node->left);
       
    for (int i = 0; i < node->course.prerequisiteCourses.size(); ++i) {

        Course course = RedBlackTree::Search(node->course.prerequisiteCourses[i]);

        if (course.courseId.empty()) {
            std::cout << "Error: invalid prerequisite course " << node->course.prerequisiteCourses[i]
                << " for course " << node->course.courseId << "." << std::endl << std::endl;
            return false;
        }
    }
        
    bool validRight = validatePrereqs(node->right);

    return validLeft && validRight;
}

// load the courses from text file into binary search tree
void loadCourses(std::string filePath, RedBlackTree* rbt) {

    std::ifstream file(filePath);
    std::string line;
    int lineNumber = 0;

    std::cout << std::endl;

    while (std::getline(file, line)) {
        ++lineNumber;
        std::istringstream iss(line);
        Course course;
        std::string prerequisite;

        std::getline(iss, course.courseId, ',');

        std::getline(iss, course.courseName, ',');

        if (course.courseId.empty() || course.courseName.empty()) {
            std::cout << "Line skipped: line " << lineNumber << " has less than 2 parameters." << std::endl;
            continue;
        }

        while (std::getline(iss, prerequisite, ',')) {
            course.prerequisiteCourses.push_back(prerequisite);
        }
        rbt->Insert(course);
    }
    std::cout << "Courses loaded." << std::endl << std::endl;
}

// display a course's info
void displayCourse(Course course) {
    std::cout << course.courseId << ", " << course.courseName << std::endl;
    std::cout << "Prerequisites: ";

    if (course.prerequisiteCourses.size() > 0) {
        for (int j = 0; j < course.prerequisiteCourses.size() - 1; ++j) {
            std::cout << course.prerequisiteCourses[j] << ", ";
        }
        std::cout << course.prerequisiteCourses.back();
        std::cout << std::endl;
    }
    else {
        std::cout << "none" << std::endl;
    }
}

int main() {

    RedBlackTree* rbt;
    rbt = new RedBlackTree();
    std::string filePath;
    std::string courseId;
    Course course;
    auto start = std::chrono::steady_clock::now();
    auto stop = std::chrono::steady_clock::now();

    std::cout << "Enter file path:" << std::endl;
    std::cin >> filePath;

    // if file does not open, ouput that there was an error
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return 0;
    }

    int choice = 0;
    bool validPrereqs = true;
    while (choice != 9) {

        std::cout << "Menu:" << std::endl;
        std::cout << " 1. Load courses" << std::endl;
        std::cout << " 2. Print Course list" << std::endl;
        std::cout << " 3. Print Course" << std::endl;
        std::cout << " 9. Exit" << std::endl;


        std::cout << "Enter choice: ";
        std::cin >> choice;

        switch (choice) {
        // case for loading courses
        case 1:
            start = std::chrono::steady_clock::now();
            loadCourses(filePath, rbt);
            stop = std::chrono::steady_clock::now();
            std::cout << "Time: " << (stop - start) / 1.0ms << " ms" << std::endl << std::endl;
            break;
        // case for printing course list
        case 2:
            start = std::chrono::steady_clock::now();
            validPrereqs = rbt->ValidatePrereqs();
            if (validPrereqs) {
                std::cout << std::endl << "Here is a sample schedule: " << std::endl << std::endl;
                rbt->InOrder();
                std::cout << std::endl;
            }
            stop = std::chrono::steady_clock::now();
            std::cout << "Time: " << (stop - start) / 1.0ms << " ms" << std::endl << std::endl;
            break;
        // case for searching a particular course
        case 3:
            std::cout << "Enter course ID: ";
            std::cin >> courseId;
            std::cout << std::endl;
            start = std::chrono::steady_clock::now();
            course = rbt->Search(courseId);
            if (!course.courseId.empty()) {
                displayCourse(course);
            }
            else {
                std::cout << "Course not found." << std::endl;
            }
            std::cout << std::endl;
            stop = std::chrono::steady_clock::now();
            std::cout << "Time: " << (stop - start) / 1.0ms << " ms" << std::endl << std::endl;
            break;
        // case for exiting
        case 9:
            std::cout << "Thank you for using the course planner!" << std::endl;
            break;
        // default case for any other input by user
        default:
            std::cout << "Invalid input." << std::endl << std::endl;
            std::cin.clear();
            std::cin.ignore();
            break;
        }
    }
    return 0;
}