#include "AABBTree.h"
#include "Shape.h"
#include <queue>
#include <cassert>
#include <limits>

using namespace Fastboi;
using namespace Collision;

AABBHandle AABBTree::InsertLeaf(const BoundingBox& tightBox) {
    const BoundingBox box = tightBox.Fattened(fatteningFactor);

    int newLeafIndex = nodes.size();

    // Check if this is the first node. If it is, nothing to do.
    if (newLeafIndex == 0) {
        this->rootIndex = newLeafIndex;
        nodes.push_back(Node(box, newLeafIndex));
        return AABBHandle(*this, newLeafIndex);
    }

    int bestSiblingIndex = FindBestSibling(box);
    nodes.push_back(Node(box, newLeafIndex));

    int newParentIndex = MakeSiblings(bestSiblingIndex, newLeafIndex);

    RefitAndRotateAncestors(nodes[newParentIndex]);

    return AABBHandle(*this, newLeafIndex);
}

void AABBTree::RemoveLeaf(const AABBHandle& handle) {
    std::span<Node> nodesView(nodes.begin(), nodes.end());

    Node& remove = nodes[handle.selfIndex];
    
    if (!remove.HasParent()) {
        // This node leaf is also the root. Just clear our entire array.
        nodes.clear();
        this->rootIndex = -1;
        return;
    } 

    auto moveNode = [&](Node& node, int newIndex) {
        if (node.IsLeaf())
            node.handle->selfIndex = newIndex;
        else {
            node.GetLeft(nodesView).parentIndex = newIndex;
            node.GetRight(nodesView).parentIndex = newIndex;
        }

        if (node.HasParent()) {
            Node& parent = node.GetParent(nodesView);
            
            if (parent.leftIndex == node.selfIndex)
                parent.leftIndex = newIndex;
            else
                parent.rightIndex = newIndex;
        }

        node.selfIndex = newIndex;
        nodesView[newIndex] = node;

        node.handle = nullptr;
    };

    Node& parent = remove.GetParent(nodesView);
    Node& sibling = remove.GetSibling(nodesView);

    if (!parent.HasParent()) {
        // We don't have any grandparents to deal with, our parent is the root

        sibling.parentIndex = -1;

        if (!sibling.IsLeaf())
            RefitAndRotateAncestors(sibling.GetLeft(nodesView));

        moveNode(nodes[nodes.size() - 1], remove.selfIndex);     
        moveNode(nodes[nodes.size() - 2], parent.selfIndex);

        // The new location of the moved nodes are in remove and parent, check
        // if we moved the root so we can change the root index
        // if we didn't move the root, the sibling's location is the root
        if (remove.parentIndex == -1)
            this->rootIndex = remove.selfIndex;
        else if (parent.parentIndex == -1)
            this->rootIndex = parent.selfIndex;
        else 
            this->rootIndex = sibling.selfIndex;

        nodes.resize(nodes.size() - 2);
        return;
    }

    // The general case: we have a grandparent, so we replace the parent with the grandparent
    Node& grandparent = parent.GetParent(nodesView);

    // Replace the parent with the sibling
    sibling.parentIndex = grandparent.selfIndex;

    if (grandparent.leftIndex == parent.selfIndex)
        grandparent.leftIndex = sibling.selfIndex;
    else
        grandparent.rightIndex = sibling.selfIndex;

    if (sibling.IsLeaf())
        RefitAndRotateAncestors(sibling);
    else
        RefitAndRotateAncestors(sibling.GetLeft(nodesView));

    // Then move the last two nodes in the array to the two nodes we are taking out, so that we can shrink the array
    moveNode(nodes[nodes.size() - 1], remove.selfIndex);
    moveNode(nodes[nodes.size() - 2], parent.selfIndex);

    nodes.resize(nodes.size() - 2);
}

void AABBTree::TestMovedTightAABB(AABBHandle& handle, const BoundingBox& tightBox) {
    const BoundingBox& fatBox = nodes[handle.selfIndex].bounds;

    if (fatBox.ContainsBox(tightBox))
        return;

    RemoveLeaf(handle);
    handle = InsertLeaf(tightBox);
}

int AABBTree::MakeSiblings(int childIndex, int orphanIndex) {
    int oldParentIndex = nodes[childIndex].parentIndex;

    // Make a new parent then cover the 3 cases for how to reorganize the tree
    int newParentIndex = CreateParent(childIndex, orphanIndex);

    if (childIndex == this->rootIndex) { // The sibling we are replacing the parent at is the root
        this->rootIndex = newParentIndex;
    } else { // The sibling is a child of another parent
        Node& oldParent = nodes[oldParentIndex];

        if (oldParent.leftIndex == childIndex)
            oldParent.leftIndex = newParentIndex;
        else
            oldParent.rightIndex = newParentIndex;

        nodes[newParentIndex].parentIndex = oldParentIndex;
    }

    return newParentIndex;
}

int AABBTree::CreateParent(int leftIndex, int rightIndex) {
    int newParentIndex = nodes.size();

    nodes.push_back(
        Node(
              BoundingBox::Union(nodes[leftIndex].bounds, nodes[rightIndex].bounds)
            , newParentIndex
            , leftIndex
            , rightIndex
        )
    );

    nodes[leftIndex].parentIndex = newParentIndex;
    nodes[rightIndex].parentIndex = newParentIndex;

    return newParentIndex;
}

void AABBTree::RefitAndRotateAncestors(const Node& startingPoint) {
    ForAllAncestors(startingPoint, [&](Node& ancestor) {
        ancestor.bounds = BoundingBox::Union(nodes[ancestor.leftIndex].bounds, nodes[ancestor.rightIndex].bounds);

        TentativeRotate(ancestor);
    });
}

void AABBTree::ForAllAncestors(const Node& startingPoint, const std::function<void(Node&)> pred) {
    for (int curIdx = startingPoint.parentIndex; curIdx != -1; curIdx = nodes[curIdx].parentIndex) {
        pred(nodes[curIdx]);
    }
}

void AABBTree::ForAllAncestors(const Node& startingPoint, const std::function<void(const Node&)> pred) const {
    for (int curIdx = startingPoint.parentIndex; curIdx != -1; curIdx = nodes[curIdx].parentIndex) {
        pred(nodes[curIdx]);
    }
}

struct SubtreeCostInfo {
    int branchIndex;
    float searchCost;

    bool operator>(const SubtreeCostInfo& rhs) const {
        return searchCost > rhs.searchCost;
    }
};

int AABBTree::FindBestSibling(const BoundingBox& newBox) const {
    if (nodes.size() == 1) return rootIndex;

    float inheritedCost = 0;
    auto computeInheritedCost = [&](const Node& ancestor) {
        float curArea = ancestor.bounds.Area();
        float newArea = BoundingBox::Union(newBox, ancestor.bounds).Area();

        inheritedCost += newArea - curArea;
    };

    std::priority_queue<SubtreeCostInfo, std::vector<SubtreeCostInfo>, std::greater<SubtreeCostInfo>> prio;

    int bestSiblingIndex = rootIndex;
    float bestCost = BoundingBox::Union(newBox, nodes[rootIndex].bounds).Area();
    prio.push({ rootIndex, bestCost });

    do {
        if (prio.top().searchCost > bestCost) {
            prio.pop();
            continue;
        }
            
        int curNodeIndex = prio.top().branchIndex;
        prio.pop();

        Node curNode = nodes[curNodeIndex];

        // Compute tentative cost for choosing this node as an ancestor
        float directCost = BoundingBox::Union(newBox, curNode.bounds).Area();
        inheritedCost = 0;

        ForAllAncestors(curNode, computeInheritedCost);

        float chooseCost = directCost + inheritedCost;

        if (chooseCost <= bestCost) {
            bestCost = chooseCost;

            if (curNode.IsLeaf()) {
                bestSiblingIndex = curNodeIndex;
                continue;
            }
        } else {
            continue;
        }

        // Compute lower bound cost for searching this node's children
        inheritedCost = 0;
        ForAllAncestors(nodes[curNode.leftIndex], computeInheritedCost);

        float searchCost = newBox.Area() + inheritedCost;

        if (searchCost <= bestCost) {
            prio.push({ curNode.leftIndex, searchCost });
            prio.push({ curNode.rightIndex, searchCost });
        }
    } while (prio.size() != 0);

    assert(nodes[bestSiblingIndex].IsLeaf());

    return bestSiblingIndex;
}

void AABBTree::TentativeRotate(const Node& node) {
    auto [targetA, targetB, newArea] = FindBestRotation(node);

    if (targetA == nullptr) return;

    // targetB parent is always the node whose area changes
    Node& changingNode = nodes[targetB->parentIndex];

    float oldArea = changingNode.bounds.Area();

    if (newArea < oldArea) {
        Rotate(*targetA, *targetB);

        // If a node was rotated then we need to refit the node that changed area
        changingNode.bounds = BoundingBox::Union(nodes[changingNode.leftIndex].bounds, nodes[changingNode.rightIndex].bounds);

        return;
    }

    return;
}

void AABBTree::Rotate(Node& a, Node& b) {
    Node& oldParentA = nodes[a.parentIndex];
    Node& oldParentB = nodes[b.parentIndex];

    std::swap(a.parentIndex, b.parentIndex);

    if (oldParentA.leftIndex == a.selfIndex)
        oldParentA.leftIndex = b.selfIndex;
    else
        oldParentA.rightIndex = b.selfIndex;

    if (oldParentB.leftIndex == b.selfIndex)
        oldParentB.leftIndex = a.selfIndex;
    else
        oldParentB.rightIndex = a.selfIndex;
}

// Imagine it works on a tree such as:
//        A
//     B     C
//   D   E  F  G
AABBTree::BestRotation_t AABBTree::FindBestRotation(const Node& grandparent) {
    std::span<Node> nodes(this->nodes.begin(), this->nodes.end()); // Intentionally shadowing here
    
    // We will consider the four possible rotations underneath a single grandparent
    BestRotation_t bestRotation = { nullptr, nullptr, std::numeric_limits<float>::infinity() };
    
    Node& B = grandparent.GetLeft(nodes);
    Node& C = grandparent.GetRight(nodes);

    if (!B.IsLeaf()) {
        Node& D = B.GetLeft(nodes);
        Node& E = B.GetRight(nodes);

        // Swap C and E
        // Calculate surface area of  B = SA(D U F U G) = SA(D U C)
        if (float newArea = BoundingBox::Union(D.bounds, C.bounds).Area(); newArea < newArea < bestRotation.newArea) {
            bestRotation = { &C, &E, newArea };
        }

        // Swap C and D
        // Calculate surface area of B = SA(E U F U G) = SA(E U C)
        if (float newArea = BoundingBox::Union(E.bounds, C.bounds).Area(); newArea < bestRotation.newArea) {
            bestRotation = { &C, &D, newArea };
        }
    }

    if (!C.IsLeaf()) {
        Node& F = C.GetLeft(nodes);
        Node& G = C.GetRight(nodes);

        // Swap B and F
        // Calculate surface area of C = SA(D U E U G) = SA(B U G)
        if (float newArea = BoundingBox::Union(B.bounds, G.bounds).Area(); newArea < bestRotation.newArea) {
            bestRotation = { &B, &F, newArea };
        }

        // Swap B and G
        // Calculate surface area of C = SA(D U E U F) = SA(B U F)
        if (float newArea = BoundingBox::Union(B.bounds, F.bounds).Area(); newArea < bestRotation.newArea) {
            bestRotation = { &B, &G, newArea };
        }
    }

    return bestRotation;
}

AABBTree::Node& AABBTree::Node::GetSibling(std::span<Node> nodes) const {
    const Node& parent = nodes[parentIndex];

    if (selfIndex == parent.leftIndex) {
        // Is left child, need to get right
        return nodes[parent.rightIndex];
    } else {
        // Is right child, need to get left
        return nodes[parent.leftIndex];
    }
}


const AABBTree::Node& AABBTree::Node::GetSibling(std::span<const Node> nodes) const {
    const Node& parent = nodes[parentIndex];

    if (selfIndex == parent.leftIndex) {
        // Is left child, need to get right
        return nodes[parent.rightIndex];
    } else {
        // Is right child, need to get left
        return nodes[parent.leftIndex];
    }
}
