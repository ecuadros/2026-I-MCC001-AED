#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include "containers/binarytree.h"

using namespace std;

template <typename Node>
void PrintInOrder(const Node* node) {
	if (!node) {
		return;
	}
	PrintInOrder(node->GetChild(0));
	cout << *node << " ";
	PrintInOrder(node->GetChild(1));
}

template <typename Node>
void PrintPreOrder(const Node* node) {
	if (!node) {
		return;
	}
	cout << *node << " ";
	PrintPreOrder(node->GetChild(0));
	PrintPreOrder(node->GetChild(1));
}

template <typename Node>
void PrintPostOrder(const Node* node) {
	if (!node) {
		return;
	}
	PrintPostOrder(node->GetChild(0));
	PrintPostOrder(node->GetChild(1));
	cout << *node << " ";
}
//BFS: Level-order traversal
template <typename Node>
void PrintLevelOrder(const Node* root) {
	if (!root) {
		return;
	}

	queue<const Node*> q;
	q.push(root);

	while (!q.empty()) {
		const Node* current = q.front();
		q.pop();

		cout << *current << " ";

		if (current->GetChild(0)) {
			q.push(current->GetChild(0));
		}
		if (current->GetChild(1)) {
			q.push(current->GetChild(1));
		}
	}
}
using BTI = BinaryTree<AscendingBinaryTreeListTrait<TI>>;
void BinaryTreeDemo() {
	BTI tree;

	tree.insert(50, 1);
	tree.insert(30, 2);
	tree.insert(70, 3);
	tree.insert(20, 4);
	tree.insert(40, 5);
	tree.insert(60, 6);
	tree.insert(80, 7);
	tree.insert(35, 8);

	cout << "BinaryTreeDemo: insercion de 8 elementos" << endl;

	cout << "Pre-order  : ";
	PrintPreOrder(tree.root());
	cout << endl;

    cout << "--- Iteradores con stack/queue ---" << endl;

	cout << "Pre-order  iterator (NLR): ";
	for (auto& node : tree.preorder()) {
		cout << node << " ";
	}
	cout << endl;

	cout << "Backward pre-order iterator (RLN): ";
	for (auto& node : tree.rpreorder()) {
		cout << node << " ";
	}
	cout << endl;

	cout << "In-order   : ";
	PrintInOrder(tree.root());
	cout << endl;
    
    cout<< "In-order iterator con node parents: ";

    cout << "In-order iterator: ";
	for (auto it = tree.begin(); it != tree.end(); ++it) {
		cout << *it << " ";
	}
	cout << endl;

	cout << "Backward in-order iterator: ";
	for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
		cout << *it << " ";
	}
	cout << endl;

	cout << "Post-order : ";
	PrintPostOrder(tree.root());
	cout << endl;

    cout << "--- Iteradores con stack/queue ---" << endl;

    cout << "Post-order iterator (LRN): ";
	for (auto& node : tree.postorder()) {
		cout << node << " ";
	}
	cout << endl;

	cout << "Backward post-order iterator (NRL): ";
	for (auto& node : tree.rpostorder()) {
		cout << node << " ";
	}
	cout << endl;

	cout << "Level-order: ";
	PrintLevelOrder(tree.root());
	cout << endl;

	cout << "--- ForEach / FirstThat ---" << endl;
	TI sum = 0;
	tree.ForEach([](auto& node, TI& acc) {
		acc += node.GetData();
	}, sum);
	cout << "Suma de nodos (in-order): " << sum << endl;

	TI sumPost = 0;
	tree.ForEach<&BTI::postorder>([](auto& node, TI& acc) {
		acc += node.GetData();
	}, sumPost);
	cout << "Suma de nodos (post-order): " << sumPost << endl;

	TI n = 55;
	auto firstGreater = tree.FirstThat([](auto& node, TI limit) {
		return node.GetData() > limit;
	}, n);
	if (firstGreater != tree.end()) {
		cout << "Primer nodo mayor que " << n << ": " << *firstGreater << endl;
	} else {
		cout << "No existe nodo mayor que " << n << endl;
	}

	cout << "--- Guardar / cargar arbol (<< y >>) ---" << endl;

	// 1) Guardar en memoria (string) y recargar
	std::stringstream mem;
	mem << tree;
	cout << "Serializado: " << mem.str() << endl;

	BinaryTree<AscendingBinaryTreeListTrait<TI>> loadedFromMemory;
	mem >> loadedFromMemory;
	cout << "Cargado desde string (in-order): ";
	for (auto& node : loadedFromMemory.inorder()) {
		cout << node << " ";
	}
	cout << endl;

	// 2) Guardar en archivo y recargar
	{
		ofstream ofs("tree_dump.txt");
		ofs << tree;
	}

	BinaryTree<AscendingBinaryTreeListTrait<TI>> loadedFromFile;
	{
		ifstream ifs("tree_dump.txt");
		ifs >> loadedFromFile;
	}
	cout << "Cargado desde archivo (in-order): ";
	for (auto& node : loadedFromFile.inorder()) {
		cout << node << " ";
	}
	cout << endl;

	

	
}
