#include <iostream>
#include <map>
#include "../types.h"

using namespace std;

/**
 * @brief Trait base para parametrizar un Digital Trie.
 *
 * Permite definir el tipo de palabra y el tipo de simbolo
 * que usara el Trie durante el recorrido.
 *
 * @tparam WordT Tipo completo de la palabra.
 * @tparam SymbolT Tipo del simbolo unitario de la palabra.
 */
template <typename WordT, typename SymbolT>
struct BaseDigitalTrieTrait {
    using word_type   = WordT;
    using symbol_type = SymbolT;
};

/**
 * @brief Trait concreto para palabras tipo string.
 */
using StringDigitalTrieTrait = BaseDigitalTrieTrait<TS, TC>;

/**
 * @brief Implementación simple de un Digital Trie parametrizado con Traits.
 *
 * Un Digital Trie almacena palabras carácter por carácter.
 * Cada nodo representa una letra dentro de un camino.
 * Si varios textos comparten prefijo, comparten también parte del camino.
 */
template <typename Traits>
class DigitalTrie {
private:
    using word_type   = typename Traits::word_type;
    using symbol_type = typename Traits::symbol_type;

    /**
     * @brief Nodo interno del Trie.
     *
     * Cada nodo guarda:
     * - si marca el fin de una palabra
     * - sus hijos, indexados por carácter
     */
    struct Node {
        TB isEndOfWord = false; ///< Indica si en este nodo termina una palabra
        map<symbol_type, Node*> children; ///< Hijos del nodo, uno por cada símbolo posible

        /**
         * @brief Destructor del nodo.
         *
         * Libera recursivamente toda la memoria de sus hijos.
         */
        ~Node() {
            for (auto& child : children) {
                delete child.second;
            }
        }
    };

    Node* m_root; ///< Raíz del Trie

    /**
     * @brief Imprime recursivamente la estructura interna del Trie.
     *
     * @param node Nodo actual del recorrido.
     * @param os Flujo de salida.
     * @param prefix Sangria acumulada para la visualizacion.
     */
    void printNode(const Node* node, ostream& os, const TS& prefix) const {
        for(const auto& [symbol, child] : node->children) {
            os << prefix << "|- " << symbol;
            if(child->isEndOfWord)
                os << " (*)";
            os << '\n';
            printNode(child, os, prefix + "|  ");
        }
    }

public:
    /**
     * @brief Constructor del Trie.
     *
     * Crea la raíz vacía del árbol.
     */
    DigitalTrie() : m_root(new Node()) {}

    /**
     * @brief Destructor del Trie.
     *
     * Libera toda la memoria del árbol a partir de la raíz.
     */
    ~DigitalTrie() {
        delete m_root;
    }

    /**
     * @brief Inserta una palabra en el Trie.
     *
     * Recorre carácter por carácter la palabra.
     * Si un nodo hijo no existe, lo crea.
     * Al final marca el último nodo como fin de palabra.
     *
     * @param word Palabra a insertar.
     */
    void insert(const word_type& word) {
        Node* current = m_root;

        for (symbol_type ch : word) {
            if (current->children.count(ch) == 0) {
                current->children[ch] = new Node();
            }
            current = current->children[ch];
        }

        current->isEndOfWord = true;
    }

    /**
     * @brief Busca una palabra exacta en el Trie.
     *
     * Recorre la palabra carácter por carácter.
     * Solo retorna true si el camino existe completo
     * y además el último nodo marca fin de palabra.
     *
     * @param word Palabra a buscar.
     * @return true si la palabra existe exactamente.
     * @return false si no existe.
     */
    TB search(const word_type& word) const {
        const Node* current = m_root;

        for (symbol_type ch : word) {
            auto it = current->children.find(ch);
            if (it == current->children.end()) {
                return false;
            }
            current = it->second;
        }

        return current->isEndOfWord;
    }

    /**
     * @brief Verifica si existe un prefijo dentro del Trie.
     *
     * A diferencia de search, aquí no importa si el último nodo
     * es fin de palabra; basta con que el camino exista.
     *
     * @param prefix Prefijo a verificar.
     * @return true si el prefijo existe.
     * @return false si no existe.
     */
    TB startsWith(const word_type& prefix) const {
        const Node* current = m_root;

        for (symbol_type ch : prefix) {
            auto it = current->children.find(ch);
            if (it == current->children.end()) {
                return false;
            }
            current = it->second;
        }

        return true;
    }

    /**
     * @brief Imprime el Trie como una estructura jerarquica.
     *
     * El simbolo (*) marca los nodos donde termina una palabra completa.
     *
     * @param os Flujo de salida donde se imprimira el Trie.
     */
    void print(ostream& os) const {
        os << "Raiz\n";
        printNode(m_root, os, "");
    }
};

void DigitalTrieDemo() {
    DigitalTrie<StringDigitalTrieTrait> trie;

    trie.insert("cat");
    trie.insert("car");
    trie.insert("cart");
    trie.insert("care");
    trie.insert("carbon");
    trie.insert("camera");
    trie.insert("camp");
    trie.insert("can");
    trie.insert("candle");
    trie.insert("dog");
    trie.insert("doll");
    trie.insert("door");
    trie.insert("dot");
    trie.insert("dove");
    trie.insert("down");
    trie.insert("data");
    trie.insert("date");
    trie.insert("deal");
    trie.insert("deer");

    cout << "===== DEMO DIGITAL TRIE =====\n\n";

    cout << "Estructura del Trie:\n";
    trie.print(cout);

    cout << "Busqueda exacta:\n";
    cout << "cat  -> " << (trie.search("cat") ? "encontrado" : "no encontrado") << '\n';
    cout << "car  -> " << (trie.search("car") ? "encontrado" : "no encontrado") << '\n';
    cout << "can  -> " << (trie.search("can") ? "encontrado" : "no encontrado") << '\n';
    cout << "dog  -> " << (trie.search("dog") ? "encontrado" : "no encontrado") << '\n';
    cout << "door -> " << (trie.search("door") ? "encontrado" : "no encontrado") << '\n';
    cout << "deal -> " << (trie.search("deal") ? "encontrado" : "no encontrado") << '\n';
    cout << "do   -> " << (trie.search("do") ? "encontrado" : "no encontrado") << '\n';

    cout << "\nBusqueda por prefijo:\n";
    cout << "ca   -> " << (trie.startsWith("ca") ? "si existe prefijo" : "no existe prefijo") << '\n';
    cout << "do   -> " << (trie.startsWith("do") ? "si existe prefijo" : "no existe prefijo") << '\n';
    cout << "de   -> " << (trie.startsWith("de") ? "si existe prefijo" : "no existe prefijo") << '\n';
    cout << "cam  -> " << (trie.startsWith("cam") ? "si existe prefijo" : "no existe prefijo") << '\n';
    cout << "dor  -> " << (trie.startsWith("dor") ? "si existe prefijo" : "no existe prefijo") << '\n';

    cout << "\n===== FIN DEMO =====\n";
}