#include <iostream>
#include <string>

using namespace std;

struct TrieNode { // estructura de cada casilla
    TrieNode* children[26]; // 26 punteros... correspodiente a las letras del abecedario
    bool isEndOfWord;

    TrieNode() {
        isEndOfWord = false; // nos asegurados que cada nuevo nodo no inicie marcado como final de palabra
        for (int i = 0; i < 26; i++) {
            children[i] = nullptr;
        }
    }
};

class Trie {
private:
    TrieNode* root;

    // Función recursiva que dibuja el árbol
    void printHelper(TrieNode* node, string prefix, bool isLastChild, char nodeChar) {
        // 1. Imprimir la sangría acumulada y el conector del nodo actual
        cout << prefix;
        if (isLastChild) {
            cout << "\\-- ";
            prefix += "    "; // Si es el último, los hijos debajo solo llevan espacios (4 espacios)
        } else {
            cout << "|-- ";
            prefix += "|   "; // Si no es el último, los hijos debajo necesitan la barra vertical
        }
        
        // 2. Imprimir el caracter y el asterisco si forma una palabra
        cout << nodeChar << (node->isEndOfWord ? "*" : "") << "\n";

        // 3. Buscar cuál es el último hijo válido de este nodo
        int lastChildIndex = -1;
        for (int i = 0; i < 26; i++) {
            if (node->children[i] != nullptr) {
                lastChildIndex = i;
            }
        }

        // 4. Llamada recursiva para todos los hijos existentes
        for (int i = 0; i < 26; i++) {
            if (node->children[i] != nullptr) {
                // Pasamos 'true' si el índice actual coincide con el último hijo encontrado
                printHelper(node->children[i], prefix, i == lastChildIndex, i + 'a');
            }
        }
    }

public:
    Trie() {
        root = new TrieNode();
    }

    void insert(string word) {
        TrieNode* curr = root;
        for (char c : word) {
            int index = c - 'a'; // convierte el caracter c en un entero, 0 - 26
            if (curr->children[index] == nullptr) {
                curr->children[index] = new TrieNode();
            }
            curr = curr->children[index];
        }
        curr->isEndOfWord = true; 
    }

    // Interfaz pública para imprimir el árbol
    void print() {
        cout << "(root)\n";
        
        // Determinar cuál es el último hijo directo de la raíz
        int lastChildIndex = -1;
        for (int i = 0; i < 26; i++) {
            if (root->children[i] != nullptr) {
                lastChildIndex = i;
            }
        }

        // Iniciar la recursión para cada hijo de la raíz
        for (int i = 0; i < 26; i++) {
            if (root->children[i] != nullptr) {
                printHelper(root->children[i], "", i == lastChildIndex, i + 'a');
            }
        }
    }
};

int main() {
    Trie* miTrie = new Trie();

    // Insertamos las palabras de tu ejemplo
    miTrie->insert("apple");
    miTrie->insert("car");
    miTrie->insert("cart");
    miTrie->insert("dog");
    miTrie->insert("matematica");
    miTrie->insert("mate");
    miTrie->insert("matematico");


    // Imprimimos el árbol
    miTrie->print();

    // Liberación de memoria (buena práctica en C++)
    delete miTrie; 
    return 0;
}