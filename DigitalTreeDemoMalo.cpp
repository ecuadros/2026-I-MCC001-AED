#include <iostream>
#include <string>

using namespace std;

/*
(root)
|-- a
|   \-- p
|       \-- p
|           \-- l
|               \-- e*
|-- c
|   \-- a
|       |-- r*
|       \-- t*
\-- d
    \-- o
        \-- g*

*/

class DigitalTree {
private:
    enum class PrintStyle {
        ASCII,
        Unicode
    };

    struct Node {
        Node* children[26]; //26 LETRAS ? LISTA ENLAZADA? 
        bool isEnd;

        Node() : isEnd(false) {
            for (int i = 0; i < 26; i++)
                children[i] = nullptr;
        }
    };

    Node* root;

    Node* traverse(const string& text) const {
        Node* current = root;

        for (char c : text) {
            int index = c - 'a';

            if (index < 0 || index >= 26)
                return nullptr;

            if (!current->children[index])
                return nullptr;

            current = current->children[index];
        }

        return current;
    }

    void print(Node* node, const string& prefix, bool isLast, PrintStyle style) const {

        const string branchLast = (style == PrintStyle::Unicode) ? "└── " : "\\-- ";
        const string branchMid = (style == PrintStyle::Unicode) ? "├── " : "|-- ";
        const string pipePrefix = (style == PrintStyle::Unicode) ? "│   " : "|   ";

        if (node != root) {
            cout << prefix;

            if (isLast)
                cout << branchLast;
            else
                cout << branchMid;
        }

        if (node == root)
            cout << "(root)\n";
        else {
            char letter = '?';

            // Encontrar qué letra representa este nodo
            for (int i = 0; i < 26; i++) {
                if (node == parent->children[i]) {
                    letter = 'a' + i;
                    break;
                }
            }

            cout << letter;

            if (node->isEnd)
                cout << "*";

            cout << '\n';
        }

        string newPrefix = prefix;

        if (node != root)
            newPrefix += isLast ? "    " : pipePrefix;

        // Contar hijos
        int totalChildren = 0;
        for (int i = 0; i < 26; i++)
            if (node->children[i])
                totalChildren++;

        int visited = 0;

        for (int i = 0; i < 26; i++) {
            if (node->children[i]) {
                visited++;

                parent = node;
                print(node->children[i], newPrefix, visited == totalChildren, style);
            }
        }
    }

    mutable Node* parent = nullptr;

public:
    DigitalTree() {
        root = new Node();
    }

    void insert(const string& word) {
        Node* current = root;

        for (char c : word) {
            int index = c - 'a';

            if (!current->children[index])
                current->children[index] = new Node();

            current = current->children[index];
        }

        current->isEnd = true;
    }

    

    bool IsEmpty() const {
        for (int i = 0; i < 26; i++) {
            if (root->children[i])
                return false;
        }
        return true;
    }

    

    bool search(const string& word) const {
        Node* node = traverse(word);
        return node != nullptr && node->isEnd;
    }

    bool startsWith(const string& prefix) const {
        return traverse(prefix) != nullptr;
    }

    

    void print() const {
        printAscii();
    }

    void printAscii() const {
        parent = nullptr;
        print(root, "", true, PrintStyle::ASCII);
    }

    void printUnicode() const {
        parent = nullptr;
        print(root, "", true, PrintStyle::Unicode);
    }
};
int main() {

    DigitalTree tree;

    tree.insert("cat");
    tree.insert("car");
    tree.insert("dog");
    tree.insert("apple");

    cout << boolalpha;
    cout << "IsEmpty: " << tree.IsEmpty() << '\n';
    cout << "search(\"cat\"): " << tree.search("cat") << '\n';
    cout << "search(\"cap\"): " << tree.search("cap") << '\n';
    cout << "startsWith(\"ca\"): " << tree.startsWith("ca") << '\n';
    cout << "startsWith(\"do\"): " << tree.startsWith("do") << '\n';

    cout << "\nArbol (ASCII, compatible):\n";
    tree.printAscii();

    //cout << "\nArbol (Unicode):\n";
    //tree.printUnicode();
}