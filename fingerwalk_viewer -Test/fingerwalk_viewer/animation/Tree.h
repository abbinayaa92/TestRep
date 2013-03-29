#ifndef TREE_H_
#define TREE_H_

#include "LinearR3.h"
#include "Node.h"

class Tree {
public:
    Tree();
    void ComputMatrix();
    void MakeJointAndEffector(Node *node);
    bool IsRoot(Node *n) const { return n == root_; }
    int GetNumNode() const { return nodeCount_; }
    int GetNumEffector() const { return effectorCount_; }
    int GetNumJoint() const { return jointCount_; }
    void InsertRoot(Node *root, bool isFrozen);
    void InsertChild(Node* parent, Node* child);
    void InsertRightSibling(Node* parent, Node* child);

    // Accessors based on node numbers
    Node *GetJoint(int);
    Node *GetEffector(int);
    const VectorR3& GetEffectorPosition(int);

    // Accessors for tree traversal
    Node *GetRoot() const { return root_; }
    Node *GetSuccessor (const Node*) const;
    Node *GetParent(const Node *node) const { return node->parent_; }

    void Compute();
    void Draw() const;
    void DrawDebug(const vector<VectorR3>& targets) const;
    void DrawTreeDebug(Node *node, const vector<VectorR3>& targets) const;
    void Print() const;
    vector<Node*> GetAllNodes() const;
    void Restructure(Node *newRoot);
    void Reset();
    void Freeze(Node *node);
    void UnFreeze(Node *node);
    void MakeEffector(Node *node);

private:
    Node *root_;
    int nodeCount_;			// nNode = nEffector + nJoint
    int effectorCount_;
    int jointCount_;
    void SetId(Node *node);
    Node *SearchJoint(Node *node, int id) const;
    Node *SearchEffector(Node *ndoe, int id) const;
    void ComputeTree(Node *node);
    void DrawTree(Node *node) const;
    void PrintTree(Node *node) const;
    void InitTree(Node *node);
    void PrintSubTree(Node *node, int level) const;
    void Fix(Node *node);
};

#endif    // TREE_H_