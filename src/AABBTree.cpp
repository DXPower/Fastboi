#include "AABBTree.h"
#include "Shape.h"
#include <algorithm>
#include <queue>
#include <cassert>
#include <limits>
#include <iostream>

using namespace Fastboi;
using namespace Collision;

#ifndef NDEBUG
#   define assert_m(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

int AABBTree::CreateNode(const BoundingBox& bounds, int parentIndex, int leftIndex, int rightIndex) {
    if (this->firstFreeIndex == -1) {
        nodes.emplace_back(bounds, nodes.size(), parentIndex, leftIndex, rightIndex);
        return nodes.back().selfIndex;
    } else {
        int nextFreeIndex = nodes[this->firstFreeIndex].parentIndex;

        Node& inserted = nodes[this->firstFreeIndex];
        inserted = Node(bounds, this->firstFreeIndex, parentIndex, leftIndex, rightIndex);

        this->firstFreeIndex = nextFreeIndex;

        return inserted.selfIndex;
    }
}

AABBHandle AABBTree::InsertLeaf(const BoundingBox& tightBox) {
    const BoundingBox box = tightBox.Fattened(fatteningFactor);

    // Check if this is the first node. If it is, nothing to do.
    if (nodes.size() == 0) {
        int newLeafIndex = CreateNode(box);
        this->rootIndex = newLeafIndex;
        return AABBHandle(*this, newLeafIndex);
    }

    int bestSiblingIndex = FindBestSibling(box);
    int newLeafIndex = CreateNode(box);

    int newParentIndex = MakeSiblings(bestSiblingIndex, newLeafIndex);

    RefitAndRotateAncestors(nodes[newParentIndex]);

    return AABBHandle(*this, newLeafIndex);
}

void AABBTree::RemoveLeaf(const AABBHandle& handle) {
    std::span<Node> nodesView(nodes.begin(), nodes.end());

    auto freeNode = [&](Node& remove) {
        remove.parentIndex = this->firstFreeIndex;
        this->firstFreeIndex = remove.selfIndex;
    };

    Node& remove = nodes[handle.selfIndex];

    if (remove.HasParent()) {
        Node& parent = remove.GetParent(nodesView);
        Node& sibling = remove.GetSibling(nodesView);

        sibling.parentIndex = parent.parentIndex;

        if (parent.HasParent()) {
            Node& grandparent = parent.GetParent(nodesView);

            if (grandparent.leftIndex == parent.selfIndex)
                grandparent.leftIndex = sibling.selfIndex;
            else
                grandparent.rightIndex = sibling.selfIndex;
            
            RefitAncestors(sibling);
        } else {
            this->rootIndex = sibling.selfIndex;
        }

        freeNode(parent);
    } else {
        this->rootIndex = -1;
    }

    remove.handle = nullptr;
    freeNode(remove);
}

// void AABBTree::RemoveLeaf(const AABBHandle& handle) {
//     std::span<Node> nodesView(nodes.begin(), nodes.end());

//     Node& remove = nodes[handle.selfIndex];
    
//     if (!remove.HasParent()) {
//         // This node leaf is also the root. Just clear our entire array.
//         nodes.clear();
//         this->rootIndex = -1;
//         return;
//     } 

//     auto moveNode = [&](Node& node, int newIndex) {
//         if (node.selfIndex == newIndex) return;

//         assert_m(node.selfIndex != node.parentIndex, "moveNode beginning");
//         std::cout << "self: " << node.selfIndex << "\nparent: " << node.parentIndex
//         << "\nleft: " << node.leftIndex << "\nright: " << node.rightIndex << "\nnew: " << newIndex << std::endl;

//         if (node.IsLeaf())
//             node.handle->selfIndex = newIndex;
//         else {
//             node.GetLeft(nodesView).parentIndex = newIndex;
//             node.GetRight(nodesView).parentIndex = newIndex;
//         }

//         if (node.HasParent()) {
//             Node& parent = node.GetParent(nodesView);
            
//             if (parent.leftIndex == node.selfIndex)
//                 parent.leftIndex = newIndex;
//             else
//                 parent.rightIndex = newIndex;
//         }

//         std::cout << "self: " << node.selfIndex << "\nparent: " << node.parentIndex
//         << "\nleft: " << node.leftIndex << "\nright: " << node.rightIndex << "\nnew: " << newIndex << std::endl;

//         node.selfIndex = newIndex;
//         nodesView[newIndex] = node;

//         node.handle = nullptr;

//         assert_m(node.selfIndex != node.parentIndex, "moveNode end");
//     };

//     Node& parent = remove.GetParent(nodesView);
//     Node& sibling = remove.GetSibling(nodesView);

//     if (!parent.HasParent()) {
//         // We don't have any grandparents to deal with, our parent is the root

//         sibling.parentIndex = -1;

//         moveNode(nodes[nodes.size() - 1], remove.selfIndex);     
//         moveNode(nodes[nodes.size() - 2], parent.selfIndex);

//         // The new location of the moved nodes are in remove and parent, check
//         // if we moved the root so we can change the root index
//         // if we didn't move the root, the sibling's location is the root
//         if (remove.parentIndex == -1)
//             this->rootIndex = remove.selfIndex;
//         else if (parent.parentIndex == -1)
//             this->rootIndex = parent.selfIndex;
//         else 
//             this->rootIndex = sibling.selfIndex;

//         nodes.resize(nodes.size() - 2);
//         return;
//     }

//     // The general case: we have a grandparent, so we replace the parent with the grandparent
//     Node& grandparent = parent.GetParent(nodesView);

//     // Replace the parent with the sibling
//     sibling.parentIndex = grandparent.selfIndex;

//     if (grandparent.leftIndex == parent.selfIndex)
//         grandparent.leftIndex = sibling.selfIndex;
//     else
//         grandparent.rightIndex = sibling.selfIndex;

//     RefitAncestors(sibling);

//     // Then move the last two nodes in the array to the two nodes we are taking out, so that we can shrink the array
//     if (Node& lastNode = nodes[nodes.size() - 1]; lastNode.parentIndex != remove.selfIndex)
//         moveNode(lastNode, remove.selfIndex);

//     if (Node& secondLastNode = nodes[nodes.size() - 2]; secondLastNode.parentIndex != parent.selfIndex)
//         moveNode(nodes[nodes.size() - 2], parent.selfIndex);

//     nodes.resize(nodes.size() - 2);
// }

void AABBTree::TestMovedTightAABB(AABBHandle& handle, const BoundingBox& tightBox) {
    const BoundingBox& fatBox = nodes[handle.selfIndex].bounds;

    if (fatBox.ContainsBox(tightBox))
        return;

    RemoveLeaf(handle);
    handle = InsertLeaf(tightBox); 
}

int AABBTree::MakeSiblings(int childIndex, int orphanIndex) {
    int oldParentIndex = nodes[childIndex].parentIndex;

    int newParentIndex = CreateParent(childIndex, orphanIndex);

    Node& child = nodes[childIndex];
    Node& newParent = nodes[newParentIndex];

    if (child.selfIndex == this->rootIndex) { // The sibling we are replacing the parent at is the root
        this->rootIndex = newParent.selfIndex;
    } else { // The sibling is a child of another parent
        Node& oldParent = nodes[oldParentIndex];

        if (oldParent.leftIndex == child.selfIndex)
            oldParent.leftIndex = newParent.selfIndex;
        else
            oldParent.rightIndex = newParent.selfIndex;

        newParent.parentIndex = oldParentIndex;
    }

    return newParentIndex;
}

int AABBTree::CreateParent(int leftIndex, int rightIndex) {
    int newParentIndex = CreateNode(
        BoundingBox::Union(nodes[leftIndex].bounds, nodes[rightIndex].bounds) 
        , -1
        , nodes[leftIndex].selfIndex 
        , nodes[rightIndex].selfIndex
    );

    nodes[leftIndex] .parentIndex = newParentIndex;
    nodes[rightIndex].parentIndex = newParentIndex;

    return newParentIndex;
}

void AABBTree::RefitAncestors(const Node& startingPoint) {
    ForAllAncestors(startingPoint, [&](Node& ancestor) {
        ancestor.bounds = BoundingBox::Union(nodes[ancestor.leftIndex].bounds, nodes[ancestor.rightIndex].bounds);
    });
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

// struct SubtreeCostInfo {
//     int branchIndex;
//     float searchCost;

//     bool operator>(const SubtreeCostInfo& rhs) const {
//         return searchCost > rhs.searchCost;
//     }
// };

// int AABBTree::FindBestSibling(const BoundingBox& newBox) const {
//     if (nodes.size() < 3) return rootIndex;

//     float inheritedCost = 0;

//     BoundingBox bubble;
//     auto computeInheritedCost = [&](const Node& ancestor) {
//         bubble = BoundingBox::Union(bubble, ancestor.bounds);

//         float curArea = ancestor.bounds.Area();
//         float newArea = bubble.Area();

//         inheritedCost += newArea - curArea;
//     };

//     std::priority_queue<SubtreeCostInfo, std::vector<SubtreeCostInfo>, std::greater<SubtreeCostInfo>> prio;

//     int bestSiblingIndex = rootIndex;
//     float bestCost = std::numeric_limits<float>::max();

//     prio.push({ nodes[rootIndex].GetLeft(nodes).selfIndex, std::numeric_limits<float>::min() });
//     prio.push({ nodes[rootIndex].GetRight(nodes).selfIndex, std::numeric_limits<float>::min() });

//     do {
//         if (prio.top().searchCost > bestCost) {
//             prio.pop();
//             continue;
//         }
            
//         int curNodeIndex = prio.top().branchIndex;
//         prio.pop();

//         Node curNode = nodes[curNodeIndex];

//         // Compute tentative cost for choosing this node as an ancestor
//         bubble = BoundingBox::Union(newBox, curNode.bounds);
//         float directCost = bubble.Area();
//         inheritedCost = 0;

//         ForAllAncestors(curNode, computeInheritedCost);

//         float chooseCost = directCost + inheritedCost;

//         if (chooseCost <= bestCost) {
//             bestCost = chooseCost;

//             if (curNode.IsLeaf()) {
//                 bestSiblingIndex = curNodeIndex;
//                 continue;
//             }
//         } else {
//             continue;
//         }

//         // Compute lower bound cost for searching this node's children
//         inheritedCost = 0;
//         bubble = BoundingBox::Union(newBox, nodes[curNode.leftIndex].bounds);
//         ForAllAncestors(nodes[curNode.leftIndex], computeInheritedCost);

//         float searchCost = newBox.Area() + inheritedCost;

//         if (searchCost <= bestCost) {
//             prio.push({ curNode.leftIndex, searchCost });
//             prio.push({ curNode.rightIndex, searchCost });
//         }
//     } while (prio.size() != 0);

//     assert(nodes[bestSiblingIndex].IsLeaf());

//     return bestSiblingIndex;
// }

int AABBTree::FindBestSibling(const BoundingBox& newBox) const {
    if (nodes.size() < 3) return rootIndex;

    auto minIt = std::ranges::min_element(nodes, [&](const Node& a, const Node& b) {
        return a.IsLeaf() 
            && BoundingBox::Union(newBox, a.bounds).Area() < BoundingBox::Union(newBox, b.bounds).Area();
    });

    // assert(minIt->IsLeaf());

    return minIt->selfIndex;

    // int bestSiblingIndex = -1;
    // float bestArea = std::numeric_limits<float>::max();

    // for (const Node& node : nodes) {
    //     if (!node.IsLeaf()) continue;


    //     float newArea = BoundingBox::Union(newBox, node.bounds).Area();

    //     if (newArea < bestArea) {
    //         bestArea = newArea;
    //         bestSiblingIndex = node.selfIndex;
    //     }
    // }

    // return bestSiblingIndex;
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
