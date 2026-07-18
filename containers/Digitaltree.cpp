#include <iostream>
#include <string_view>
#include "../types.h"
using namespace std;

class DigitalTree {
private:
    static constexpr LSI AlphabetSize = 26;  
    struct Node {
        Node* children[AlphabetSize];
        LSC letter;
		LSB isEnd;
        Node(LSC value = '\0') : letter(value), isEnd(false)
        {
            for(LSI i = 0; i < AlphabetSize; ++i) {children[i] = nullptr;}
        }
    };
    Node* m_root;

    // Libera recursivamente todos los nodos
    void Clear(Node* node)
    {
        if(node == nullptr) {return;}
        for(LSI i = 0; i < AlphabetSize; ++i) {Clear(node->children[i]);}
        delete node;
    }

    // Recorre el Tree siguiendo una palabra o prefijo
    Node* Traverse(const LSS& text) const
    {
        Node* current = m_root;
        for(LSC c : text)
        {
            LSI index = static_cast<LSI>(c - 'a');
            if(index < 0 || index >= AlphabetSize) {return nullptr;}
            if(current->children[index] == nullptr) {return nullptr;}
            current = current->children[index];
        }
        return current;
    }


public:
    DigitalTree() {m_root = new Node();}
    ~DigitalTree() {Clear(m_root);}
    void Insert(const LSS& text)
    {
        Node* current = m_root;
        for(LSC c : text)
        {
            LSI index = static_cast<LSI>(c - 'a');
            if(index < 0 || index >= AlphabetSize) {return;}
            if(current->children[index] == nullptr) {current->children[index] = new Node(c);}
            current = current->children[index];
        }
        current->isEnd = true;
    }
    LSB Search(const LSS& text) const
    {
        Node* node = Traverse(text);
        return (node != nullptr && node->isEnd);
    }
    LSB StartsWith(const LSS& prefix) const
    {
        return Traverse(prefix) != nullptr;
    }
    LSB IsEmpty() const
    {
        for(LSI i = 0; i < AlphabetSize; ++i) {if(m_root->children[i] != nullptr) {return false;}}
        return true;
    }

private:
	void PrintNode(Node* node, const LSS& prefix, LSB isLast, ostream& os) const
	{
    	if(node == nullptr) {return;}
    	if(node == m_root) {os << "(root)\n";}
    	else
    	{
        	os << prefix;
        	if(isLast) {os << "\\-- ";}
        	else {os << "|-- ";}
        	os << node->letter;
        	if(node->isEnd) {os << '*';}
        	os << '\n';
    	}
    	LSS newPrefix = prefix;
    	if(node != m_root)
    	{
        	if(isLast) {newPrefix += "    ";}
        	else {newPrefix += "|   ";}
    	}
    	LSI totalChildren = 0;
    	for(LSI i = 0; i < AlphabetSize; ++i)
    	{
        	if(node->children[i] != nullptr) {totalChildren = totalChildren + 1;}
    	}
    	LSI currentChild = 0;
    	for(LSI i = 0; i < AlphabetSize; ++i)
    	{
        	if(node->children[i] == nullptr) {continue;}
        	currentChild = currentChild + 1;
        	PrintNode(node->children[i], newPrefix, currentChild == totalChildren, os);
    	}
	}

public:
ostream& Print(ostream& os) const
{
    PrintNode(m_root, "", true, os);
    return os;
}
};

// Inserción
static void DemoInsert(DigitalTree& tree, ostream& os)
{
    os << "\nInsertar\n";
    tree.Insert("ana");
    tree.Insert("andres");
    tree.Insert("andrea");
    tree.Insert("carlos");
    tree.Insert("carmen");
    tree.Insert("daniel");
    tree.Insert("diego");
    tree.Insert("luis");
    tree.Insert("luisa");
    tree.Insert("pedro");
    os << "Palabras insertadas correctamente.\n";
}

// Búsqueda
static void DemoSearch(const DigitalTree& tree, ostream& os)
{
    os << "\nBuscar\n";
    LSS texts[] ={"ana", "carlos", "pedro", "maria"};
    for(const auto& text : texts)
    {
        os << text << " -> ";
        if(tree.Search(text)) {os << "Encontrada\n";}
        else {os << "No encontrada\n";}
    }
}

// Búsqueda de prefijos
static void DemoStartsWith(const DigitalTree& tree, ostream& os)
{
    os << "\nPrefijos\n";
    LSS prefixes[] = {"an", "car", "di", "ma"};

    for(const auto& prefix : prefixes)
    {
        os << prefix << " -> ";
        if(tree.StartsWith(prefix)) {os << "Existe\n";}
        else {os << "No existe\n";}
    }
}

// Imprimir Arbol
static void DemoPrint(const DigitalTree& tree, ostream& os)
{
    os << "\nArbol Digital Tree\n";
    tree.Print(os);
}

void DemoTree(ostream& os)
{
    os << "\nPrueba Digital Tree\n";
    DigitalTree tree;
    os << "\n¿Tree vacio?: ";
    if(tree.IsEmpty()) {os << "Si\n";}
    else {os << "No\n";}
    DemoInsert(tree, os);
    os << "\n¿Tree vacio?: ";
    if(tree.IsEmpty()) {os << "Si\n";}
    else {os << "No\n";}
    DemoSearch(tree, os);
    DemoStartsWith(tree, os);
    DemoPrint(tree, os);
    os << "\nComplejidad teorica:\n";
    os << "Insertar     : O(n)\n";
    os << "Buscar       : O(n)\n";
    os << "Comenzar con : O(n)\n";
    os << "Digital Tree organiza las claves carácter por carácter compartiendo prefijos comunes.\n";
}

int main()
{
    DemoTree(cout);
    return 0;
}
