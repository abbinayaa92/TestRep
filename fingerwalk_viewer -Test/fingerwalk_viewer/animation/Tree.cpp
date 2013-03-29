#include <queue>
#include <GL/glut.h>
#include "LinearR3.h"
#include "Tree.h"
#include "Node.h"

using namespace std;

/*
 * Default constructor
 * Initializes the root and the counts
 */
Tree::Tree()
{
    root_ = NULL;
    nodeCount_ = effectorCount_ = jointCount_ = 0;
}

/*
 * Makes the given node effector if it was joint
 */
void Tree::MakeEffector(Node *node)
{
    if (node->purpose_ == JOINT) {
        node->purpose_ = EFFECTOR;
        node->effectorId_ = effectorCount_++;
        node->jointId_ = -1;
    }
}

/*
 * Makes the given node effector-joint if it was joint
 */
void Tree::MakeJointAndEffector(Node *node)
{
    if (node->purpose_ == JOINT) {
        node->purpose_ = BOTH;
        node->effectorId_ = effectorCount_++;
    }
}

/*
 * Resets the tree to an empty state
 */
void Tree::Reset()
{
    root_ = NULL;
    nodeCount_ = effectorCount_ = jointCount_ = 0;
}

/*
 * Gets a list of all nodes
 */
vector<Node*> Tree::GetAllNodes() const
{
    vector<Node*> allNodes;

    queue<Node*> q;
    q.push(root_);
    while (!q.empty()) {
        Node *n = q.front();
        q.pop();
        allNodes.push_back(n);
        for (vector<Node*>::const_iterator it = n->children_.begin();
                                           it < n->children_.end();
                                           ++it) {
            q.push(*it);
        }
    }

    return allNodes;
}

void Tree::ComputMatrix()
{
    vector<Node*> allNodes;

    queue<Node*> q;
    q.push(root_);
    while (!q.empty()) {
        Node *n = q.front();
        q.pop();
        allNodes.push_back(n);
        n->ComputeMatrix();
        for (vector<Node*>::const_iterator it = n->children_.begin();
            it < n->children_.end();
            ++it) {
                q.push(*it);
        }
    }
}

/*
 * Prints the tree under node
 */
void Tree::PrintSubTree(Node *node, int level) const
{
    if (node != 0) {
        node->Print(level);
        for (vector<Node*>::const_iterator it = node->children_.begin();
                                           it < node->children_.end();
                                           ++it) {
            PrintSubTree(*it, level + 1);
        }
    }
}

/*
 * Prints the tree, starting at the root
 */
void Tree::Print() const
{
    printf("--------------------------------\n");
    PrintSubTree(root_, 0);
    printf("--------------------------------\n");
}

/*
 * Adds the root
 */
void Tree::InsertRoot(Node *root, bool isFrozen)
{
    assert(nodeCount_ == 0);

    root->id_ = nodeCount_++;

    Tree::root_ = root;

    if (isFrozen)
        root->Freeze();

    SetId(root);
}

/*
 * Adds a child to parent
 */
void Tree::InsertChild(Node* parent, Node* child)
{
    assert(parent);

    child->id_ = nodeCount_++;

    parent->children_.push_back(child);
    child->parent_ = parent;

    SetId(child);
}

/*
 * Adds a sibling to parent
 */
void Tree::InsertRightSibling(Node* parent, Node* child)
{
    assert(parent);

    child->id_ = nodeCount_++;

    parent->parent_->children_.push_back(child);
    child->parent_ = parent->parent_;

    SetId(child);
}

/*
 * Sets an id for node
 */
void Tree::SetId(Node *node)
{
    switch (node->purpose_) {
    case JOINT:
        node->jointId_ = jointCount_++;
        node->effectorId_ = -1;
        break;
    case EFFECTOR:
        node->jointId_ = -1;
        node->effectorId_ = effectorCount_++;
        break;
    case BOTH:
        node->jointId_ = jointCount_++;
        node->effectorId_ = effectorCount_++;
        assert(node->jointId_ >= 0 && node->effectorId_ >= 0);
        break;
    default:
        assert(false);
    }
}

/*
 * Search recursively below node for the joint with index value
 */
Node *Tree::SearchJoint(Node* node, int index) const
{
    Node *ret;
    if (node != NULL) {
        if (node->jointId_ == index) {
            return node;
        } else {
            for (vector<Node*>::const_iterator it = node->children_.begin();
                                               it < node->children_.end();
                                               ++it)
                if (ret = SearchJoint(*it, index))
                    return ret;
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

/*
 * Get the joint with the index value
 */
Node *Tree::GetJoint(int index)
{
    return SearchJoint(root_, index);
}

/*
 * Search recursively below node for the end effector with the index value
 */
Node *Tree::SearchEffector(Node* node, int index) const
{
    Node* ret;
    if (node != 0) {
        if (node->effectorId_ == index) {
            return node;
        } else {
            for (vector<Node*>::const_iterator it = node->children_.begin();
                                               it < node->children_.end();
                                               ++it)
                if (ret = SearchEffector(*it, index))
                    return ret;
            return NULL;
        }
    } else {
        return NULL;
    }
}

/*
 * Get the end effector for the index value
 */
Node *Tree::GetEffector(int index)
{
    return SearchEffector(root_, index);
}

/*
 * Returns the global position of the effectors
 */
const VectorR3& Tree::GetEffectorPosition(int index)
{
    Node *effector = GetEffector(index);
    assert(effector);
    return (effector->globalPosition_);
}

/*
 * Computes the global data for the tree under node
 */
void Tree::ComputeTree(Node *node)
{
    if (node != NULL) {
        node->ComputeGlobalData();
        for (vector<Node*>::const_iterator it = node->children_.begin();
                                           it < node->children_.end();
                                           ++it)
            ComputeTree(*it);
    }
}

/*
 * Computes the global data for the tree under root
 */
void Tree::Compute(void)
{
    ComputeTree(root_);
}

/*
 * Draws the tree under node
 */
void Tree::DrawTree(Node *node) const
{
    if (node != 0) {
        glPushMatrix();
        //node->Draw(node == root_);
        node->DrawUsingMatrix(node == root_);
        for (vector<Node*>::const_iterator it = node->children_.begin();
                                           it < node->children_.end();
                                           ++it)
            DrawTree(*it);
        glPopMatrix();
    }
}

/*
 * Draws a line from each effect to its target for the tree under root
 */
void Tree::DrawDebug(const vector<VectorR3>& targets) const
{
    DrawTreeDebug(root_, targets);
}

/*
 * Draws a line from each effect to its target for the tree under node
 */
void Tree::DrawTreeDebug(Node *node, const vector<VectorR3>& targets) const
{
    if (node != 0) {
        node->DrawDebug(targets);
        for (vector<Node*>::const_iterator it = node->children_.begin();
                                           it < node->children_.end();
                                           ++it)
            DrawTreeDebug((*it), targets);
    }
}

/*
 * Draws the tree
 */
void Tree::Draw(void) const
{
   DrawTree(root_);
}