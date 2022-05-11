#pragma once

#include "Shape.h"
#include <optional>
#include <span>
#include <functional>
#include <any>

namespace Fastboi {
    namespace Collision {
        struct AABBTree;

        struct AABBHandle {
            AABBTree* tree;
            std::any owner; // Holds a pointer to the owner to be used for queries on the tree

            private:
            int selfIndex;

            public:
            AABBHandle() = default;
            AABBHandle(AABBTree& tree, std::any&& owner, int selfIndex);

            AABBHandle(const AABBHandle& copy) = delete;
            AABBHandle(AABBHandle&& move);

            ~AABBHandle();

            AABBHandle& operator=(const AABBHandle& copy) = delete;
            AABBHandle& operator=(AABBHandle&& move);

            bool operator==(const AABBHandle& rhs) { return tree == rhs.tree && selfIndex == rhs.selfIndex; }

            BoundingBox GetInTreeBounds() const;
            void Reset();

            friend struct AABBTree;
        };

        struct AABBTree {
            struct Node;
            private:
            struct BestRotation_t;

            public:
            std::vector<Node> nodes;
            int rootIndex = -1;
            int firstFreeIndex = -1;

            float fatteningFactor;

            AABBTree(float fatteningFactor) : fatteningFactor(fatteningFactor) { }

            private:
            AABBHandle InsertLeaf(const BoundingBox& tightBox, std::any&& owner);

            public:
            template<typename Owner>
            requires std::is_pointer_v<Owner>
                  or std::is_reference_v<Owner> 
                  or std::is_same_v<Owner, std::nullptr_t>
            AABBHandle InsertLeaf(const BoundingBox& tightBox, Owner owner) {
                if constexpr (std::is_reference_v<Owner>)
                    return InsertLeaf(tightBox, std::any(&owner));
                else
                    return InsertLeaf(tightBox, std::any(owner));
            }

            void TestMovedTightAABB(AABBHandle& handle, const BoundingBox& tightBox);
            void ForAllOverlapping(const BoundingBox& box, const std::function<void(const AABBHandle& handle)>& pred) const;
            void ForAllOverlapping(const BoundingBox& box, const std::function<void(AABBHandle& handle)>& pred);

            private:
            int CreateNode(const BoundingBox& bounds, int parentIndex, int leftIndex, int rightIndex);
            int CreateNode(const BoundingBox& bounds, int parentIndex) { return CreateNode(bounds, parentIndex, -1, -1); };
            int CreateNode(const BoundingBox& bounds) { return CreateNode(bounds, -1, -1, -1); };
            
            void RemoveLeaf(const AABBHandle& handle);

            int FindBestSibling(const BoundingBox& newBox) const;
            int CreateParent(int leftIndex, int rightIndex);
            int MakeSiblings(int childIndex, int orphanIndex);

            void RefitAncestors(const Node& startingPoint);
            void RefitAndRotateAncestors(const Node& startingPoint);

            // targetB will always be the child of the parent with the changing area
            BestRotation_t FindBestRotation(const Node& grandparent);
            void TentativeRotate(const Node& node);
            void Rotate(Node& a, Node& b);

            void ForAllAncestors(const Node& startingPoint, const std::function<void(Node& ancestor)> pred);
            void ForAllAncestors(const Node& startingPoint, const std::function<void(const Node& ancestor)> pred) const;
            void ForAllOverlappingHelper(const BoundingBox& box, const Node& node, const std::function<void(const AABBHandle& handle)>& pred) const;
            void ForAllOverlappingHelper(const BoundingBox& box, const Node& node, const std::function<void(AABBHandle& handle)>& pred);



            friend struct AABBHandle;

            

            struct BestRotation_t {
                Node* targetA;
                Node* targetB;
                float newArea;
            };

            public:
            struct Node {
                AABBHandle* handle = nullptr;
                BoundingBox bounds;

                int parentIndex;
                int selfIndex;
                int leftIndex;
                int rightIndex;

                Node() : parentIndex(-1), selfIndex(-1), leftIndex(-1), rightIndex(-1) { };
                Node(const BoundingBox& box, int self, int parent, int left, int right) : bounds(box), parentIndex(parent), selfIndex(self), leftIndex(left), rightIndex(right) { }
                Node(const BoundingBox& box, int self, int left, int right) : Node(box, self, -1, left, right) { }
                Node(const BoundingBox& box, int self) : Node(box, self, -1, -1) { }
                Node(const BoundingBox& box) : Node(box, -1, -1, -1) { }

                bool HasParent() const { return parentIndex >= 0; }
                bool HasLeft() const { return leftIndex >= 0; }
                bool HasRight() const { return rightIndex >= 0; }
                bool HasBoth() const { return HasLeft() && HasRight(); }

                bool IsLeaf() const { return handle != nullptr; }

                      Node& GetParent(std::span<Node> nodes) const       { return nodes[parentIndex]; }
                const Node& GetParent(std::span<const Node> nodes) const { return nodes[parentIndex]; }

                      Node& GetSibling(std::span<Node> nodes) const;
                const Node& GetSibling(std::span<const Node> nodes) const;

                      Node& GetLeft(std::span<Node> nodes) const       { return nodes[leftIndex]; }
                const Node& GetLeft(std::span<const Node> nodes) const { return nodes[leftIndex]; }

                      Node& GetRight(std::span<Node> nodes) const       { return nodes[rightIndex]; }
                const Node& GetRight(std::span<const Node> nodes) const { return nodes[rightIndex]; }
                
                
                std::pair<Node&, Node&>             GetBothChildren(std::span<Node> nodes) const       { return { nodes[leftIndex], nodes[rightIndex] }; }
                std::pair<const Node&, const Node&> GetBothChildren(std::span<const Node> nodes) const { return { nodes[leftIndex], nodes[rightIndex] }; }
            };
        };

        
        inline AABBHandle::AABBHandle(AABBTree& tree, std::any&& owner, int selfIndex)
        : tree(&tree)
        , owner(std::move(owner))
        , selfIndex(selfIndex)
        {
            tree.nodes[selfIndex].handle = this;
        }

        inline AABBHandle::AABBHandle(AABBHandle&& move) : AABBHandle(*move.tree, std::move(move.owner), move.selfIndex) {
            move.tree = nullptr;
            move.owner = nullptr;
        }

        inline AABBHandle::~AABBHandle() {
            if (tree != nullptr)
                tree->RemoveLeaf(*this);
        }

        inline AABBHandle& AABBHandle::operator=(AABBHandle&& move) {
            tree = move.tree;
            selfIndex = move.selfIndex;
            owner = move.owner;

            move.tree = nullptr;
            move.owner = nullptr;

            tree->nodes[selfIndex].handle = this;

            return *this;
        }

        inline BoundingBox AABBHandle::GetInTreeBounds() const {
            return tree->nodes[selfIndex].bounds;
        }

        inline void AABBHandle::Reset() {
            if (tree != nullptr) {
                tree->RemoveLeaf(*this);
                tree = nullptr;
            }
        }
    }
}