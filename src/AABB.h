/*
  Copyright (c) 2009 Erin Catto http://www.box2d.org
  Copyright (c) 2016-2018 Lester Hedges <lester.hedges+aabbcc@gmail.com>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.

  This code was adapted from parts of the Box2D Physics Engine,
  http://www.box2d.org
*/

/*
  This software has been modified by Dylan Ferris as early as May 1st, 2020.
  Under Lemma 2 of this copyright notice, this is an altered source version.
  The purpose of the modification was to limit the dimensionality to 2D,
  remove any periodicity, and make it natively interface with the Fastboi
  game engine, also created by Dylan Ferris.
*/

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Vec.h"

/// Null node flag.
const unsigned int NULL_NODE = 0xffffffff;

namespace Fastboi {
    struct Collider;

    namespace AABBTree {
        using ParticleIndex_t = Collider*;

        /*! \brief The axis-aligned bounding box object.

            Axis-aligned bounding boxes (AABBs) store information for the minimum
            orthorhombic bounding-box for an object. Support is provided for
            dimensions = 2

            Class member functions provide functionality for merging AABB objects
            and testing overlap with other AABBs.
        */
        class AABB {
            Vecf lowerBound;
            Vecf upperBound;
            Vecf centre;
            float surfaceArea;

            public:
            AABB();
            AABB(const Vecf& lowerBounds, const Vecf& upperBounds);
            ~AABB();

            inline Vecf getLowerBound() const { return lowerBound; };
            inline Vecf getUpperBound() const { return upperBound; };
            inline Vecf getCentre() const { return centre; };
            inline float getSurfaceArea() const { return surfaceArea; };

            // void updateBounds(const Vecf& lowerBounds, const Vecf& upperBounds);

            void merge(const AABB&, const AABB&); // Merge two AABBs into this one.
            bool contains(const AABB&) const; // Test whether an AABB is contained within this one.
            bool overlaps(const AABB&, bool touchIsOverlap) const; // Test whether an AABB overlaps with this one

            inline static bool shouldRenderAABBs = false;
            static void RenderAllAABBs();

            private:
            Vecf computeCentre();
            float computeSurfaceArea() const;
        };

        /*! \brief A node of the AABB tree.

            Each node of the tree contains an AABB object which corresponds to a
            particle, or a group of particles, in the simulation box. The AABB
            objects of individual particles are "fattened" before they are stored
            to avoid having to continually update and rebalance the tree when
            displacements are small.

            Nodes are aware of their position within in the tree. The isLeaf member
            function allows the tree to query whether the node is a leaf, i.e. to
            determine whether it holds a single particle.
        */
        struct Node
        {
            /// Constructor.
            Node();

            /// The fattened axis-aligned bounding box.
            AABB aabb;

            /// Index of the parent node.
            unsigned int parent = NULL_NODE;

            /// Index of the next node.
            unsigned int next = NULL_NODE;

            /// Index of the left-hand child.
            unsigned int left = NULL_NODE;

            /// Index of the right-hand child.
            unsigned int right = NULL_NODE;

            /// Height of the node. This is 0 for a leaf and -1 for a free node.
            int height = -1;

            /// The index of the particle that the node contains (leaf nodes only).
            ParticleIndex_t particle;

            //! Test whether the node is a leaf.
            /*! \return
                    Whether the node is a leaf node.
            */
            bool isLeaf() const;
        };

        /*! \brief The dynamic AABB tree.

            The dynamic AABB tree is a hierarchical data structure that can be used
            to efficiently query overlaps between objects of arbitrary shape and
            size that lie inside of a simulation box. Support is provided for
            periodic and non-periodic boxes, as well as boxes with partial
            periodicity, e.g. periodic along specific axes.
        */
        class Tree
        {
            public:
            Tree(double skinThickness = 0.05,
                unsigned int nParticles = 16, bool touchIsOverlap=true);

            void insertParticle(ParticleIndex_t index, Vecf lowerBound, Vecf upperBound);

            /// Return the number of particles in the tree.
            unsigned int nParticles();

            void removeParticle(ParticleIndex_t index);

            /// Remove all particles from the tree.
            void removeAll();

            //! Update the tree if a particle moves outside its fattened AABB.
            /*! \param index
                    The particle index (particleMap will be used to map the node).

                \param lowerBound
                    The lower bound in each dimension.

                \param upperBound
                    The upper bound in each dimension.

                \param alwaysReinsert
                    Always reinsert the particle, even if it's within its old AABB (default: false)
            */
            bool updateParticle(ParticleIndex_t particle, Vecf lowerBound, Vecf upperBound, bool alwaysReinsert=false);

            std::vector<ParticleIndex_t> query(ParticleIndex_t particle);
            std::vector<ParticleIndex_t> query(ParticleIndex_t particle, const AABB& aabb);

            // //! Query the tree to find candidate interactions for an AABB.
            // /*! \param aabb
            //         The AABB.

            //     \return particles
            //         A vector of particle indices.
            // */
            // std::vector<unsigned int> query(const AABB&);

            //! Get a particle AABB.
            /*! \param particle
                    The particle index.
            */
            const AABB& getAABB(ParticleIndex_t particle);

            //! Get the height of the tree.
            /*! \return
                    The height of the binary tree.
            */
            unsigned int getHeight() const;

            //! Get the number of nodes in the tree.
            /*! \return
                    The number of nodes in the tree.
            */
            unsigned int getNodeCount() const;

            //! Compute the maximum balancance of the tree.
            /*! \return
                    The maximum difference between the height of two
                    children of a node.
            */
            unsigned int computeMaximumBalance() const;

            //! Compute the surface area ratio of the tree.
            /*! \return
                    The ratio of the sum of the node surface area to the surface
                    area of the root node.
            */
            double computeSurfaceAreaRatio() const;

            /// Validate the tree.
            void validate() const;

            /// Rebuild an optimal tree.
            void rebuild();

        private:
            /// The index of the root node.
            unsigned int root;

            /// The dynamic tree.
            std::vector<Node> nodes;

            /// The current number of nodes in the tree.
            unsigned int nodeCount;

            /// The current node capacity.
            unsigned int nodeCapacity;

            /// The position of node at the top of the free list.
            unsigned int freeList;


            /// The skin thickness of the fattened AABBs, as a fraction of the AABB base length.
            double skinThickness;

            /// The size of the system in each dimension.
            Vecf boxSize;

            /// The position of the negative minimum image.
            Vecf negMinImage;

            /// The position of the positive minimum image.
            Vecf posMinImage;

            /// A map between particle and node indices.
            std::unordered_map<ParticleIndex_t, unsigned int> particleMap;

            /// Does touching count as overlapping in tree queries?
            bool touchIsOverlap;

            //! Allocate a new node.
            /*! \return
                    The index of the allocated node.
            */
            unsigned int allocateNode();

            //! Free an existing node.
            /*! \param node
                    The index of the node to be freed.
            */
            void freeNode(unsigned int);

            //! Insert a leaf into the tree.
            /*! \param leaf
                    The index of the leaf node.
            */
            void insertLeaf(unsigned int);

            //! Remove a leaf from the tree.
            /*! \param leaf
                    The index of the leaf node.
            */
            void removeLeaf(unsigned int);

            //! Balance the tree.
            /*! \param node
                    The index of the node.
            */
            unsigned int balance(unsigned int);

            //! Compute the height of the tree.
            /*! \return
                    The height of the entire tree.
            */
            unsigned int computeHeight() const;

            //! Compute the height of a sub-tree.
            /*! \param node
                    The index of the root node.

                \return
                    The height of the sub-tree.
            */
            unsigned int computeHeight(unsigned int) const;

            //! Assert that the sub-tree has a valid structure.
            /*! \param node
                    The index of the root node.
            */
            void validateStructure(unsigned int) const;

            //! Assert that the sub-tree has valid metrics.
            /*! \param node
                    The index of the root node.
            */
            void validateMetrics(unsigned int) const;

            // //! Apply periodic boundary conditions.
            // /* \param position
            //         The position vector.
            //  */
            // void periodicBoundaries(std::vector<double>&);

            //! Compute minimum image separation.
            /*! \param separation
                    The separation vector.

                \param shift
                    The shift vector.

                \return
                    Whether a periodic shift has been applied.
            */
            bool minimumImage(std::vector<double>&, std::vector<double>&);
        };
    }
}
