#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

#include <algorithm>
#include <mutex>
#include <utility>

#include "binarytree.h"



template <typename T>
using AscendingAVLTreeTrait = AscendingBinaryTreeListTrait<T>;

template <typename T>
using DescendingAVLTreeTrait = DescendingBinaryTreeListTrait<T>;

template <typename Traits>
class AVLTree : public BinaryTree<Traits> {
public:
	using Base       = BinaryTree<Traits>;
	using value_type = typename Base::value_type;
	using BaseNode   = typename Base::Node;
	using Comp       = typename Base::Comp;
    using BaseNodePtr = typename Base::NodePtr;
    using TH         = TI;
	using TD = size_t;
	class AVLTreeNode : public BaseNode {
	public:
		using Parent  = BaseNode;
		using Node    = AVLTreeNode;
		using NodePtr = Node*;
        
	private:
		TH m_height = 1;

	public:
		AVLTreeNode(const value_type& data, const Ref& ref,
					BaseNodePtr left = nullptr, BaseNodePtr right = nullptr, BaseNodePtr parent = nullptr)
			: Parent(data, ref, left, right, parent) {
		}

		TH GetHeight() const override { return m_height; }
		void SetHeight(TH height) override { m_height = height; }
	};

	using Node    = AVLTreeNode;
	using NodePtr = Node*;

	AVLTree() = default;

	AVLTree(const AVLTree& other)
		: Base() {
		std::scoped_lock<std::mutex> lock(other.m_mtx);
		this->m_comp = other.m_comp;
		this->m_pRoot = CloneSubtree(other.m_pRoot, nullptr);
	}

	AVLTree(AVLTree&& other) noexcept
		: Base() {
		std::scoped_lock<std::mutex> lock(other.m_mtx);
		this->m_comp = std::move(other.m_comp);
		this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
	}

	AVLTree& operator=(const AVLTree& other) {
		if (this == &other) {
			return *this;
		}

		std::scoped_lock lock(this->m_mtx, other.m_mtx);
		delete this->m_pRoot;
		this->m_pRoot = nullptr;
		this->m_comp = other.m_comp;
		this->m_pRoot = CloneSubtree(other.m_pRoot, nullptr);
		return *this;
	}

	AVLTree& operator=(AVLTree&& other) noexcept {
		if (this == &other) {
			return *this;
		}

		std::scoped_lock lock(this->m_mtx, other.m_mtx);
		delete this->m_pRoot;
		this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
		this->m_comp = std::move(other.m_comp);
		return *this;
	}

private:
	static TH Height(BaseNode* node) {
		if (!node) {
			return 0;
		}
		return node->GetHeight();
	}

	static void UpdateHeight(BaseNode* node) {
		if (!node) {
			return;
		}
		TH left_height = Height(node->GetChild(0));
		TH right_height = Height(node->GetChild(1));
		node->SetHeight(1 + std::max(left_height, right_height));
	}

	static TH BalanceFactor(BaseNode* node) {
		if (!node) {
			return 0;
		}
		return Height(node->GetChild(0)) - Height(node->GetChild(1));
	}
    // en lugar de rotate left o rigth se puede colocar 0 o 1 para indicar la direccion de rotacion
    //TODO: agregar rotate left o right con direccion
    // Aun se dejara asi para mayor claridad
	//static BaseNodePtr RotateRight(BaseNodePtr y) {
	//	BaseNodePtr x = y->GetChild(0);
	//	BaseNodePtr t2 = x->GetChild(1);
//
	//	x->SetChild(1, y);
	//	x->SetParent(y->GetParent());
//
	//	y->SetChild(0, t2);
	//	if (t2) {
	//		t2->SetParent(y);
	//	}
	//	y->SetParent(x);
//
	//	UpdateHeight(y);
	//	UpdateHeight(x);
	//	return x;
	//}
//
	//static BaseNodePtr RotateLeft(BaseNodePtr x) {
	//	BaseNodePtr y = x->GetChild(1);
	//	BaseNodePtr t2 = y->GetChild(0);
//
	//	y->SetChild(0, x);
	//	y->SetParent(x->GetParent());
//
	//	x->SetChild(1, t2);
	//	if (t2) {
	//		t2->SetParent(x);
	//	}
	//	x->SetParent(y);
//
	//	UpdateHeight(x);
	//	UpdateHeight(y);
	//	return y;
	//}

	static BaseNodePtr Rotate(BaseNodePtr root, TD dir)
	{
		TD opposite = dir ^ 1;
		BaseNodePtr pivot = root->GetChild(dir);
		BaseNodePtr middle = pivot->GetChild(opposite);

		pivot->SetChild(opposite, root);
		pivot->SetParent(root->GetParent());

		root->SetChild(dir, middle);
		if (middle) {
			middle->SetParent(root);
		}

		root->SetParent(pivot);

		UpdateHeight(root);
		UpdateHeight(pivot);

		return pivot;
	}

	static BaseNodePtr RotateRight(BaseNodePtr node)
	{
		return Rotate(node, 0);
	}

	static BaseNodePtr RotateLeft(BaseNodePtr node)
	{
		return Rotate(node, 1);
	}



	BaseNodePtr Rebalance(BaseNodePtr node) {
		UpdateHeight(node);

		TH balance = BalanceFactor(node);

		if (balance > 1) {
			if (BalanceFactor(node->GetChild(0)) < 0) {
				BaseNodePtr new_left = RotateLeft(node->GetChild(0));
				node->SetChild(0, new_left);
				if (new_left) {
					new_left->SetParent(node);
				}
			}
			return RotateRight(node);
		}

		if (balance < -1) {
			if (BalanceFactor(node->GetChild(1)) > 0) {
				BaseNodePtr new_right = RotateRight(node->GetChild(1));
				node->SetChild(1, new_right);
				if (new_right) {
					new_right->SetParent(node);
				}
			}
			return RotateLeft(node);
		}

		return node;
	}

protected:
	BaseNodePtr create_node(const value_type& value, Ref ref, BaseNodePtr parent) override {
		return new Node(value, ref, nullptr, nullptr, parent);
	}

	BaseNodePtr after_insert(BaseNodePtr node) override {
		return Rebalance(node);
	}

	static BaseNodePtr CloneSubtree(BaseNodePtr src, BaseNodePtr parent) {
		if (!src) {
			return nullptr;
		}

		NodePtr copy = new Node(src->GetData(), src->GetRef(), nullptr, nullptr, parent);
		copy->SetHeight(src->GetHeight());

		BaseNodePtr left = CloneSubtree(src->GetChild(0), copy);
		BaseNodePtr right = CloneSubtree(src->GetChild(1), copy);
		copy->SetChild(0, left);
		copy->SetChild(1, right);
		return copy;
	}
};

#endif // __AVL_TREE_H__
