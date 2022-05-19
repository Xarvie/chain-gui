//
// Created by caspase on 2021/1/3.
//

#include "AABB.h"

namespace aabb
{
    AABB::AABB()
    {
    }

    AABB::AABB(AABBKey dimension)
    {
        assert(dimension >= 2);

        lowerBound.resize(dimension);
        upperBound.resize(dimension);
    }

    AABB::AABB(const std::vector<double>& lowerBound_, const std::vector<double>& upperBound_) :
            lowerBound(lowerBound_), upperBound(upperBound_)
    {
        // Validate the dimensionality of the bounds vectors.
        if (lowerBound.size() != upperBound.size())
        {
            throw std::invalid_argument("[ERROR]: Dimensionality mismatch!");
        }

        // Validate that the upper bounds exceed the lower bounds.
        for (AABBKey i=0;i<lowerBound.size();i++)
        {
            // Validate the bound.
            if (lowerBound[i] > upperBound[i])
            {
                throw std::invalid_argument("[ERROR]: AABB lower bound is greater than the upper bound!");
            }
        }

        surfaceArea = computeSurfaceArea();
        centre = computeCentre();
    }

    double AABB::computeSurfaceArea() const
    {
        // Sum of "area" of all the sides.
        double sum = 0;

        // General formula for one side: hold one dimension constant
        // and multiply by all the other ones.
        for (AABBKey d1 = 0; d1 < lowerBound.size(); d1++)
        {
            // "Area" of current side.
            double product = 1;

            for (AABBKey d2 = 0; d2 < lowerBound.size(); d2++)
            {
                if (d1 == d2)
                    continue;

                double dx = upperBound[d2] - lowerBound[d2];
                product *= dx;
            }

            // Update the sum.
            sum += product;
        }

        return 2.0 * sum;
    }

    double AABB::getSurfaceArea() const
    {
        return surfaceArea;
    }

    void AABB::merge(const AABB& aabb1, const AABB& aabb2)
    {
        assert(aabb1.lowerBound.size() == aabb2.lowerBound.size());
        assert(aabb1.upperBound.size() == aabb2.upperBound.size());

        lowerBound.resize(aabb1.lowerBound.size());
        upperBound.resize(aabb1.lowerBound.size());

        for (AABBKey i=0;i<lowerBound.size();i++)
        {
            lowerBound[i] = std::min(aabb1.lowerBound[i], aabb2.lowerBound[i]);
            upperBound[i] = std::max(aabb1.upperBound[i], aabb2.upperBound[i]);
        }

        surfaceArea = computeSurfaceArea();
        centre = computeCentre();
    }

    bool AABB::contains(const AABB& aabb) const
    {
        assert(aabb.lowerBound.size() == lowerBound.size());

        for (AABBKey i=0;i<lowerBound.size();i++)
        {
            if (aabb.lowerBound[i] < lowerBound[i]) return false;
            if (aabb.upperBound[i] > upperBound[i]) return false;
        }

        return true;
    }

    bool AABB::overlaps(const AABB& aabb, bool touchIsOverlap) const
    {
        assert(aabb.lowerBound.size() == lowerBound.size());

        bool rv = true;

        if (touchIsOverlap)
        {
            for (AABBKey i = 0; i < lowerBound.size(); ++i)
            {
                if (aabb.upperBound[i] < lowerBound[i] || aabb.lowerBound[i] > upperBound[i])
                {
                    rv = false;
                    break;
                }
            }
        }
        else
        {
            for (AABBKey i = 0; i < lowerBound.size(); ++i)
            {
                if (aabb.upperBound[i] <= lowerBound[i] || aabb.lowerBound[i] >= upperBound[i])
                {
                    rv = false;
                    break;
                }
            }
        }

        return rv;
    }

    bool AABB::overlapsPoint(const AABB& aabb, bool touchIsOverlap) const
    {
        assert(aabb.lowerBound.size() == lowerBound.size());

        bool rv = true;

        if (touchIsOverlap)
        {
            for (AABBKey i = 0; i < lowerBound.size(); ++i)
            {
                if (aabb.upperBound[i] < lowerBound[i] || aabb.lowerBound[i] > upperBound[i])
                {
                    rv = false;
                    break;
                }
            }
        }
        else
        {
            for (AABBKey i = 0; i < lowerBound.size(); ++i)
            {
                if (aabb.upperBound[i] <= lowerBound[i] || aabb.lowerBound[i] >= upperBound[i])
                {
                    rv = false;
                    break;
                }
            }
        }

        return rv;
    }
    std::vector<double> AABB::computeCentre()
    {
        std::vector<double> position(lowerBound.size());

        for (AABBKey i=0;i<position.size();i++)
            position[i] = 0.5 * (lowerBound[i] + upperBound[i]);

        return position;
    }

    void AABB::setDimension(AABBKey dimension)
    {
        assert(dimension >= 2);

        lowerBound.resize(dimension);
        upperBound.resize(dimension);
    }

    Node::Node()
    {
    }

    bool Node::isLeaf() const
    {
        return (left == NULL_NODE);
    }

    Tree::Tree(AABBKey dimension_,
               double skinThickness_,
               AABBKey nParticles,
               bool touchIsOverlap_) :
            dimension(dimension_), isPeriodic(false), skinThickness(skinThickness_),
            touchIsOverlap(touchIsOverlap_)
    {
        // Validate the dimensionality.
        if ((dimension < 2))
        {
            throw std::invalid_argument("[ERROR]: Invalid dimensionality!");
        }

        // Initialise the periodicity vector.
        periodicity.resize(dimension);
        std::fill(periodicity.begin(), periodicity.end(), false);

        // Initialise the tree.
        root = NULL_NODE;
        nodeCount = 0;
        nodeCapacity = nParticles;
        nodes.resize(nodeCapacity);

        // Build a linked list for the list of free nodes.
        for (AABBKey i=0;i<nodeCapacity-1;i++)
        {
            nodes[i].next = i + 1;
            nodes[i].height = -1;
        }
        nodes[nodeCapacity-1].next = NULL_NODE;
        nodes[nodeCapacity-1].height = -1;

        // Assign the index of the first free node.
        freeList = 0;
    }

    Tree::Tree(AABBKey dimension_,
               double skinThickness_,
               const std::vector<bool>& periodicity_,
               const std::vector<double>& boxSize_,
               AABBKey nParticles,
               bool touchIsOverlap_) :
            dimension(dimension_), skinThickness(skinThickness_),
            periodicity(periodicity_), boxSize(boxSize_),
            touchIsOverlap(touchIsOverlap_)
    {
        // Validate the dimensionality.
        if (dimension < 2)
        {
            throw std::invalid_argument("[ERROR]: Invalid dimensionality!");
        }

        // Validate the dimensionality of the vectors.
        if ((periodicity.size() != dimension) || (boxSize.size() != dimension))
        {
            throw std::invalid_argument("[ERROR]: Dimensionality mismatch!");
        }

        // Initialise the tree.
        root = NULL_NODE;
        nodeCount = 0;
        nodeCapacity = nParticles;
        nodes.resize(nodeCapacity);

        // Build a linked list for the list of free nodes.
        for (AABBKey i=0;i<nodeCapacity-1;i++)
        {
            nodes[i].next = i + 1;
            nodes[i].height = -1;
        }
        nodes[nodeCapacity-1].next = NULL_NODE;
        nodes[nodeCapacity-1].height = -1;

        // Assign the index of the first free node.
        freeList = 0;

        // Check periodicity.
        isPeriodic = false;
        posMinImage.resize(dimension);
        negMinImage.resize(dimension);
        for (AABBKey i=0;i<dimension;i++)
        {
            posMinImage[i] =  0.5*boxSize[i];
            negMinImage[i] = -0.5*boxSize[i];

            if (periodicity[i])
                isPeriodic = true;
        }
    }

    void Tree::setPeriodicity(const std::vector<bool>& periodicity_)
    {
        periodicity = periodicity_;
    }

    void Tree::setBoxSize(const std::vector<double>& boxSize_)
    {
        boxSize = boxSize_;
    }

    AABBKey Tree::allocateNode()
    {
        // Exand the node pool as needed.
        if (freeList == NULL_NODE)
        {
            assert(nodeCount == nodeCapacity);

            // The free list is empty. Rebuild a bigger pool.
            nodeCapacity *= 2;
            nodes.resize(nodeCapacity);

            // Build a linked list for the list of free nodes.
            for (AABBKey i=nodeCount;i<nodeCapacity-1;i++)
            {
                nodes[i].next = i + 1;
                nodes[i].height = -1;
            }
            nodes[nodeCapacity-1].next = NULL_NODE;
            nodes[nodeCapacity-1].height = -1;

            // Assign the index of the first free node.
            freeList = nodeCount;
        }

        // Peel a node off the free list.
        AABBKey node = freeList;
        freeList = nodes[node].next;
        nodes[node].parent = NULL_NODE;
        nodes[node].left = NULL_NODE;
        nodes[node].right = NULL_NODE;
        nodes[node].height = 0;
        nodes[node].aabb.setDimension(dimension);
        nodeCount++;

        return node;
    }

    void Tree::freeNode(AABBKey node)
    {
        assert(node < nodeCapacity);
        assert(0 < nodeCount);

        nodes[node].next = freeList;
        nodes[node].height = -1;
        freeList = node;
        nodeCount--;
    }

    void Tree::insertParticle(AABBKey particle, std::vector<double>& position, double radius)
    {
        // Make sure the particle doesn't already exist.
        if (particleMap.count(particle) != 0)
        {
            throw std::invalid_argument("[ERROR]: Particle already exists in tree!");
        }

        // Validate the dimensionality of the position vector.
        if (position.size() != dimension)
        {
            throw std::invalid_argument("[ERROR]: Dimensionality mismatch!");
        }

        // Allocate a new node for the particle.
        AABBKey node = allocateNode();

        // AABB size in each dimension.
        std::vector<double> size(dimension);

        // Compute the AABB limits.
        for (AABBKey i=0;i<dimension;i++)
        {
            nodes[node].aabb.lowerBound[i] = position[i] - radius;
            nodes[node].aabb.upperBound[i] = position[i] + radius;
            size[i] = nodes[node].aabb.upperBound[i] - nodes[node].aabb.lowerBound[i];
        }

        // Fatten the AABB.
        for (AABBKey i=0;i<dimension;i++)
        {
            nodes[node].aabb.lowerBound[i] -= skinThickness * size[i];
            nodes[node].aabb.upperBound[i] += skinThickness * size[i];
        }
        nodes[node].aabb.surfaceArea = nodes[node].aabb.computeSurfaceArea();
        nodes[node].aabb.centre = nodes[node].aabb.computeCentre();

        // Zero the height.
        nodes[node].height = 0;

        // Insert a new leaf into the tree.
        insertLeaf(node);

        // Add the new particle to the map.
        particleMap.insert(std::unordered_map<AABBKey, AABBKey>::value_type(particle, node));

        // Store the particle index.
        nodes[node].particle = particle;
    }

    void Tree::insertParticle(AABBKey particle, std::vector<double>& lowerBound, std::vector<double>& upperBound)
    {
        // Make sure the particle doesn't already exist.
        if (particleMap.count(particle) != 0)
        {
            throw std::invalid_argument("[ERROR]: Particle already exists in tree!");
        }

        // Validate the dimensionality of the bounds vectors.
        if ((lowerBound.size() != dimension) || (upperBound.size() != dimension))
        {
            throw std::invalid_argument("[ERROR]: Dimensionality mismatch!");
        }

        // Allocate a new node for the particle.
        AABBKey node = allocateNode();

        // AABB size in each dimension.
        std::vector<double> size(dimension);

        // Compute the AABB limits.
        for (AABBKey i=0;i<dimension;i++)
        {
            // Validate the bound.
            if (lowerBound[i] > upperBound[i])
            {
                throw std::invalid_argument("[ERROR]: AABB lower bound is greater than the upper bound!");
            }

            nodes[node].aabb.lowerBound[i] = lowerBound[i];
            nodes[node].aabb.upperBound[i] = upperBound[i];
            size[i] = upperBound[i] - lowerBound[i];
        }

        // Fatten the AABB.
        for (AABBKey i=0;i<dimension;i++)
        {
            nodes[node].aabb.lowerBound[i] -= skinThickness * size[i];
            nodes[node].aabb.upperBound[i] += skinThickness * size[i];
        }
        nodes[node].aabb.surfaceArea = nodes[node].aabb.computeSurfaceArea();
        nodes[node].aabb.centre = nodes[node].aabb.computeCentre();

        // Zero the height.
        nodes[node].height = 0;

        // Insert a new leaf into the tree.
        insertLeaf(node);

        // Add the new particle to the map.
        particleMap.insert(std::unordered_map<AABBKey, AABBKey>::value_type(particle, node));

        // Store the particle index.
        nodes[node].particle = particle;
    }

    AABBKey Tree::nParticles()
    {
        return particleMap.size();
    }

    void Tree::removeParticle(AABBKey particle)
    {
        // Map iterator.
        std::unordered_map<AABBKey, AABBKey>::iterator it;

        // Find the particle.
        it = particleMap.find(particle);

        // The particle doesn't exist.
        if (it == particleMap.end())
        {
            throw std::invalid_argument("[ERROR]: Invalid particle index!");
        }

        // Extract the node index.
        AABBKey node = it->second;

        // Erase the particle from the map.
        particleMap.erase(it);

        assert(node < nodeCapacity);
        assert(nodes[node].isLeaf());

        removeLeaf(node);
        freeNode(node);
    }

    void Tree::removeAll()
    {
        // Iterator pointing to the start of the particle map.
        std::unordered_map<AABBKey, AABBKey>::iterator it = particleMap.begin();

        // Iterate over the map.
        while (it != particleMap.end())
        {
            // Extract the node index.
            AABBKey node = it->second;

            assert(node < nodeCapacity);
            assert(nodes[node].isLeaf());

            removeLeaf(node);
            freeNode(node);

            it++;
        }

        // Clear the particle map.
        particleMap.clear();
    }

    bool Tree::updateParticle(AABBKey particle, std::vector<double>& position, double radius,
                              bool alwaysReinsert)
    {
        // Validate the dimensionality of the position vector.
        if (position.size() != dimension)
        {
            throw std::invalid_argument("[ERROR]: Dimensionality mismatch!");
        }

        // AABB bounds vectors.
        std::vector<double> lowerBound(dimension);
        std::vector<double> upperBound(dimension);

        // Compute the AABB limits.
        for (AABBKey i=0;i<dimension;i++)
        {
            lowerBound[i] = position[i] - radius;
            upperBound[i] = position[i] + radius;
        }

        // Update the particle.
        return updateParticle(particle, lowerBound, upperBound, alwaysReinsert);
    }

    bool Tree::updateParticle(AABBKey particle, std::vector<double>& lowerBound,
                              std::vector<double>& upperBound, bool alwaysReinsert)
    {
        // Validate the dimensionality of the bounds vectors.
        if ((lowerBound.size() != dimension) && (upperBound.size() != dimension))
        {
            throw std::invalid_argument("[ERROR]: Dimensionality mismatch!");
        }

        // Map iterator.
        std::unordered_map<AABBKey, AABBKey>::iterator it;

        // Find the particle.
        it = particleMap.find(particle);

        // The particle doesn't exist.
        if (it == particleMap.end())
        {
            throw std::invalid_argument("[ERROR]: Invalid particle index!");
        }

        // Extract the node index.
        AABBKey node = it->second;

        assert(node < nodeCapacity);
        assert(nodes[node].isLeaf());

        // AABB size in each dimension.
        std::vector<double> size(dimension);

        // Compute the AABB limits.
        for (AABBKey i=0;i<dimension;i++)
        {
            // Validate the bound.
            if (lowerBound[i] > upperBound[i])
            {
                throw std::invalid_argument("[ERROR]: AABB lower bound is greater than the upper bound!");
            }

            size[i] = upperBound[i] - lowerBound[i];
        }

        // Create the new AABB.
        AABB aabb(lowerBound, upperBound);

        // No need to update if the particle is still within its fattened AABB.
        if (!alwaysReinsert && nodes[node].aabb.contains(aabb)) return false;

        // Remove the current leaf.
        removeLeaf(node);

        // Fatten the new AABB.
        for (AABBKey i=0;i<dimension;i++)
        {
            aabb.lowerBound[i] -= skinThickness * size[i];
            aabb.upperBound[i] += skinThickness * size[i];
        }

        // Assign the new AABB.
        nodes[node].aabb = aabb;

        // Update the surface area and centroid.
        nodes[node].aabb.surfaceArea = nodes[node].aabb.computeSurfaceArea();
        nodes[node].aabb.centre = nodes[node].aabb.computeCentre();

        // Insert a new leaf node.
        insertLeaf(node);

        return true;
    }

    std::vector<AABBKey> Tree::query(AABBKey particle)
    {
        // Make sure that this is a valid particle.
        if (particleMap.count(particle) == 0)
        {
            throw std::invalid_argument("[ERROR]: Invalid particle index!");
        }

        // Test overlap of particle AABB against all other particles.
        return query(particle, nodes[particleMap.find(particle)->second].aabb);
    }

    std::vector<AABBKey> Tree::query(AABBKey particle, const AABB& aabb)
    {
        std::vector<AABBKey> stack;
        stack.reserve(256);
        stack.push_back(root);

        std::vector<AABBKey> particles;

        while (stack.size() > 0)
        {
            AABBKey node = stack.back();
            stack.pop_back();

            // Copy the AABB.
            AABB nodeAABB = nodes[node].aabb;

            if (node == NULL_NODE) continue;

            if (isPeriodic)
            {
                std::vector<double> separation(dimension);
                std::vector<double> shift(dimension);
                for (AABBKey i=0;i<dimension;i++)
                    separation[i] = nodeAABB.centre[i] - aabb.centre[i];

                bool isShifted = minimumImage(separation, shift);

                // Shift the AABB.
                if (isShifted)
                {
                    for (AABBKey i=0;i<dimension;i++)
                    {
                        nodeAABB.lowerBound[i] += shift[i];
                        nodeAABB.upperBound[i] += shift[i];
                    }
                }
            }

            // Test for overlap between the AABBs.
            if (aabb.overlaps(nodeAABB, touchIsOverlap))
            {
                // Check that we're at a leaf node.
                if (nodes[node].isLeaf())
                {
                    // Can't interact with itself.
                    if (nodes[node].particle != particle)
                    {
                        particles.push_back(nodes[node].particle);
                    }
                }
                else
                {
                    stack.push_back(nodes[node].left);
                    stack.push_back(nodes[node].right);
                }
            }
        }

        return particles;
    }

    std::vector<AABBKey> Tree::queryPoint(AABBKey particle, const AABB& aabb)
    {
        std::vector<AABBKey> stack;
        stack.reserve(256);
        stack.push_back(root);

        std::vector<AABBKey> particles;

        while (stack.size() > 0)
        {
            AABBKey node = stack.back();
            stack.pop_back();

            // Copy the AABB.
            AABB nodeAABB = nodes[node].aabb;

            if (node == NULL_NODE) continue;

            if (isPeriodic)
            {
                std::vector<double> separation(dimension);
                std::vector<double> shift(dimension);
                for (AABBKey i=0;i<dimension;i++)
                    separation[i] = 0;//nodeAABB.centre[i] - aabb.centre[i];

                bool isShifted = minimumImage(separation, shift);

                // Shift the AABB.
                if (isShifted)
                {
                    for (AABBKey i=0;i<dimension;i++)
                    {
                        nodeAABB.lowerBound[i] += shift[i];
                        nodeAABB.upperBound[i] += shift[i];
                    }
                }
            }

            // Test for overlap between the AABBs.
            if (aabb.overlaps(nodeAABB, touchIsOverlap))
            {
                // Check that we're at a leaf node.
                if (nodes[node].isLeaf())
                {
                    // Can't interact with itself.
                    if (nodes[node].particle != particle)
                    {
                        particles.push_back(nodes[node].particle);
                    }
                }
                else
                {
                    stack.push_back(nodes[node].left);
                    stack.push_back(nodes[node].right);
                }
            }
        }

        return particles;
    }


    std::vector<AABBKey> Tree::query(const AABB& aabb)
    {
        // Make sure the tree isn't empty.
        if (particleMap.size() == 0)
        {
            return std::vector<AABBKey>();
        }

        // Test overlap of AABB against all particles.
        return query(std::numeric_limits<AABBKey>::max(), aabb);
    }

    const AABB& Tree::getAABB(AABBKey particle)
    {
        return nodes[particleMap[particle]].aabb;
    }

    void Tree::insertLeaf(AABBKey leaf)
    {
        if (root == NULL_NODE)
        {
            root = leaf;
            nodes[root].parent = NULL_NODE;
            return;
        }

        // Find the best sibling for the node.

        AABB leafAABB = nodes[leaf].aabb;
        AABBKey index = root;

        while (!nodes[index].isLeaf())
        {
            // Extract the children of the node.
            AABBKey left  = nodes[index].left;
            AABBKey right = nodes[index].right;

            double surfaceArea = nodes[index].aabb.getSurfaceArea();

            AABB combinedAABB;
            combinedAABB.merge(nodes[index].aabb, leafAABB);
            double combinedSurfaceArea = combinedAABB.getSurfaceArea();

            // Cost of creating a new parent for this node and the new leaf.
            double cost = 2.0 * combinedSurfaceArea;

            // Minimum cost of pushing the leaf further down the tree.
            double inheritanceCost = 2.0 * (combinedSurfaceArea - surfaceArea);

            // Cost of descending to the left.
            double costLeft;
            if (nodes[left].isLeaf())
            {
                AABB aabb;
                aabb.merge(leafAABB, nodes[left].aabb);
                costLeft = aabb.getSurfaceArea() + inheritanceCost;
            }
            else
            {
                AABB aabb;
                aabb.merge(leafAABB, nodes[left].aabb);
                double oldArea = nodes[left].aabb.getSurfaceArea();
                double newArea = aabb.getSurfaceArea();
                costLeft = (newArea - oldArea) + inheritanceCost;
            }

            // Cost of descending to the right.
            double costRight;
            if (nodes[right].isLeaf())
            {
                AABB aabb;
                aabb.merge(leafAABB, nodes[right].aabb);
                costRight = aabb.getSurfaceArea() + inheritanceCost;
            }
            else
            {
                AABB aabb;
                aabb.merge(leafAABB, nodes[right].aabb);
                double oldArea = nodes[right].aabb.getSurfaceArea();
                double newArea = aabb.getSurfaceArea();
                costRight = (newArea - oldArea) + inheritanceCost;
            }

            // Descend according to the minimum cost.
            if ((cost < costLeft) && (cost < costRight)) break;

            // Descend.
            if (costLeft < costRight) index = left;
            else                      index = right;
        }

        AABBKey sibling = index;

        // Create a new parent.
        AABBKey oldParent = nodes[sibling].parent;
        AABBKey newParent = allocateNode();
        nodes[newParent].parent = oldParent;
        nodes[newParent].aabb.merge(leafAABB, nodes[sibling].aabb);
        nodes[newParent].height = nodes[sibling].height + 1;

        // The sibling was not the root.
        if (oldParent != NULL_NODE)
        {
            if (nodes[oldParent].left == sibling) nodes[oldParent].left = newParent;
            else                                  nodes[oldParent].right = newParent;

            nodes[newParent].left = sibling;
            nodes[newParent].right = leaf;
            nodes[sibling].parent = newParent;
            nodes[leaf].parent = newParent;
        }
            // The sibling was the root.
        else
        {
            nodes[newParent].left = sibling;
            nodes[newParent].right = leaf;
            nodes[sibling].parent = newParent;
            nodes[leaf].parent = newParent;
            root = newParent;
        }

        // Walk back up the tree fixing heights and AABBs.
        index = nodes[leaf].parent;
        while (index != NULL_NODE)
        {
            index = balance(index);

            AABBKey left = nodes[index].left;
            AABBKey right = nodes[index].right;

            assert(left != NULL_NODE);
            assert(right != NULL_NODE);

            nodes[index].height = 1 + std::max(nodes[left].height, nodes[right].height);
            nodes[index].aabb.merge(nodes[left].aabb, nodes[right].aabb);

            index = nodes[index].parent;
        }
    }

    void Tree::removeLeaf(AABBKey leaf)
    {
        if (leaf == root)
        {
            root = NULL_NODE;
            return;
        }

        AABBKey parent = nodes[leaf].parent;
        AABBKey grandParent = nodes[parent].parent;
        AABBKey sibling;

        if (nodes[parent].left == leaf) sibling = nodes[parent].right;
        else                            sibling = nodes[parent].left;

        // Destroy the parent and connect the sibling to the grandparent.
        if (grandParent != NULL_NODE)
        {
            if (nodes[grandParent].left == parent) nodes[grandParent].left = sibling;
            else                                   nodes[grandParent].right = sibling;

            nodes[sibling].parent = grandParent;
            freeNode(parent);

            // Adjust ancestor bounds.
            AABBKey index = grandParent;
            while (index != NULL_NODE)
            {
                index = balance(index);

                AABBKey left = nodes[index].left;
                AABBKey right = nodes[index].right;

                nodes[index].aabb.merge(nodes[left].aabb, nodes[right].aabb);
                nodes[index].height = 1 + std::max(nodes[left].height, nodes[right].height);

                index = nodes[index].parent;
            }
        }
        else
        {
            root = sibling;
            nodes[sibling].parent = NULL_NODE;
            freeNode(parent);
        }
    }

    AABBKey Tree::balance(AABBKey node)
    {
        assert(node != NULL_NODE);

        if (nodes[node].isLeaf() || (nodes[node].height < 2))
            return node;

        AABBKey left = nodes[node].left;
        AABBKey right = nodes[node].right;

        assert(left < nodeCapacity);
        assert(right < nodeCapacity);

        int currentBalance = nodes[right].height - nodes[left].height;

        // Rotate right branch up.
        if (currentBalance > 1)
        {
            AABBKey rightLeft = nodes[right].left;
            AABBKey rightRight = nodes[right].right;

            assert(rightLeft < nodeCapacity);
            assert(rightRight < nodeCapacity);

            // Swap node and its right-hand child.
            nodes[right].left = node;
            nodes[right].parent = nodes[node].parent;
            nodes[node].parent = right;

            // The node's old parent should now point to its right-hand child.
            if (nodes[right].parent != NULL_NODE)
            {
                if (nodes[nodes[right].parent].left == node) nodes[nodes[right].parent].left = right;
                else
                {
                    assert(nodes[nodes[right].parent].right == node);
                    nodes[nodes[right].parent].right = right;
                }
            }
            else root = right;

            // Rotate.
            if (nodes[rightLeft].height > nodes[rightRight].height)
            {
                nodes[right].right = rightLeft;
                nodes[node].right = rightRight;
                nodes[rightRight].parent = node;
                nodes[node].aabb.merge(nodes[left].aabb, nodes[rightRight].aabb);
                nodes[right].aabb.merge(nodes[node].aabb, nodes[rightLeft].aabb);

                nodes[node].height = 1 + std::max(nodes[left].height, nodes[rightRight].height);
                nodes[right].height = 1 + std::max(nodes[node].height, nodes[rightLeft].height);
            }
            else
            {
                nodes[right].right = rightRight;
                nodes[node].right = rightLeft;
                nodes[rightLeft].parent = node;
                nodes[node].aabb.merge(nodes[left].aabb, nodes[rightLeft].aabb);
                nodes[right].aabb.merge(nodes[node].aabb, nodes[rightRight].aabb);

                nodes[node].height = 1 + std::max(nodes[left].height, nodes[rightLeft].height);
                nodes[right].height = 1 + std::max(nodes[node].height, nodes[rightRight].height);
            }

            return right;
        }

        // Rotate left branch up.
        if (currentBalance < -1)
        {
            AABBKey leftLeft = nodes[left].left;
            AABBKey leftRight = nodes[left].right;

            assert(leftLeft < nodeCapacity);
            assert(leftRight < nodeCapacity);

            // Swap node and its left-hand child.
            nodes[left].left = node;
            nodes[left].parent = nodes[node].parent;
            nodes[node].parent = left;

            // The node's old parent should now point to its left-hand child.
            if (nodes[left].parent != NULL_NODE)
            {
                if (nodes[nodes[left].parent].left == node) nodes[nodes[left].parent].left = left;
                else
                {
                    assert(nodes[nodes[left].parent].right == node);
                    nodes[nodes[left].parent].right = left;
                }
            }
            else root = left;

            // Rotate.
            if (nodes[leftLeft].height > nodes[leftRight].height)
            {
                nodes[left].right = leftLeft;
                nodes[node].left = leftRight;
                nodes[leftRight].parent = node;
                nodes[node].aabb.merge(nodes[right].aabb, nodes[leftRight].aabb);
                nodes[left].aabb.merge(nodes[node].aabb, nodes[leftLeft].aabb);

                nodes[node].height = 1 + std::max(nodes[right].height, nodes[leftRight].height);
                nodes[left].height = 1 + std::max(nodes[node].height, nodes[leftLeft].height);
            }
            else
            {
                nodes[left].right = leftRight;
                nodes[node].left = leftLeft;
                nodes[leftLeft].parent = node;
                nodes[node].aabb.merge(nodes[right].aabb, nodes[leftLeft].aabb);
                nodes[left].aabb.merge(nodes[node].aabb, nodes[leftRight].aabb);

                nodes[node].height = 1 + std::max(nodes[right].height, nodes[leftLeft].height);
                nodes[left].height = 1 + std::max(nodes[node].height, nodes[leftRight].height);
            }

            return left;
        }

        return node;
    }

    AABBKey Tree::computeHeight() const
    {
        return computeHeight(root);
    }

    AABBKey Tree::computeHeight(AABBKey node) const
    {
        assert(node < nodeCapacity);

        if (nodes[node].isLeaf()) return 0;

        AABBKey height1 = computeHeight(nodes[node].left);
        AABBKey height2 = computeHeight(nodes[node].right);

        return 1 + std::max(height1, height2);
    }

    AABBKey Tree::getHeight() const
    {
        if (root == NULL_NODE) return 0;
        return nodes[root].height;
    }

    AABBKey Tree::getNodeCount() const
    {
        return nodeCount;
    }

    AABBKey Tree::computeMaximumBalance() const
    {
        AABBKey maxBalance = 0;
        for (AABBKey i=0; i<nodeCapacity; i++)
        {
            if (nodes[i].height <= 1)
                continue;

            assert(nodes[i].isLeaf() == false);

            AABBKey balance = std::abs(nodes[nodes[i].left].height - nodes[nodes[i].right].height);
            maxBalance = std::max(maxBalance, balance);
        }

        return maxBalance;
    }

    double Tree::computeSurfaceAreaRatio() const
    {
        if (root == NULL_NODE) return 0.0;

        double rootArea = nodes[root].aabb.computeSurfaceArea();
        double totalArea = 0.0;

        for (AABBKey i=0; i<nodeCapacity;i++)
        {
            if (nodes[i].height < 0) continue;

            totalArea += nodes[i].aabb.computeSurfaceArea();
        }

        return totalArea / rootArea;
    }

    void Tree::validate() const
    {
#ifndef NDEBUG
        validateStructure(root);
        validateMetrics(root);

        AABBKey freeCount = 0;
        AABBKey freeIndex = freeList;

        while (freeIndex != NULL_NODE)
        {
            assert(freeIndex < nodeCapacity);
            freeIndex = nodes[freeIndex].next;
            freeCount++;
        }

        assert(getHeight() == computeHeight());
        assert((nodeCount + freeCount) == nodeCapacity);
#endif
    }

    void Tree::rebuild()
    {
        std::vector<AABBKey> nodeIndices(nodeCount);
        AABBKey count = 0;

        for (AABBKey i=0;i<nodeCapacity;i++)
        {
            // Free node.
            if (nodes[i].height < 0) continue;

            if (nodes[i].isLeaf())
            {
                nodes[i].parent = NULL_NODE;
                nodeIndices[count] = i;
                count++;
            }
            else freeNode(i);
        }

        while (count > 1)
        {
            double minCost = std::numeric_limits<double>::max();
            int iMin = -1, jMin = -1;

            for (AABBKey i=0;i<count;i++)
            {
                AABB aabbi = nodes[nodeIndices[i]].aabb;

                for (AABBKey j=i+1;j<count;j++)
                {
                    AABB aabbj = nodes[nodeIndices[j]].aabb;
                    AABB aabb;
                    aabb.merge(aabbi, aabbj);
                    double cost = aabb.getSurfaceArea();

                    if (cost < minCost)
                    {
                        iMin = i;
                        jMin = j;
                        minCost = cost;
                    }
                }
            }

            AABBKey index1 = nodeIndices[iMin];
            AABBKey index2 = nodeIndices[jMin];

            AABBKey parent = allocateNode();
            nodes[parent].left = index1;
            nodes[parent].right = index2;
            nodes[parent].height = 1 + std::max(nodes[index1].height, nodes[index2].height);
            nodes[parent].aabb.merge(nodes[index1].aabb, nodes[index2].aabb);
            nodes[parent].parent = NULL_NODE;

            nodes[index1].parent = parent;
            nodes[index2].parent = parent;

            nodeIndices[jMin] = nodeIndices[count-1];
            nodeIndices[iMin] = parent;
            count--;
        }

        root = nodeIndices[0];

        validate();
    }

    void Tree::validateStructure(AABBKey node) const
    {
        if (node == NULL_NODE) return;

        if (node == root) assert(nodes[node].parent == NULL_NODE);

        AABBKey left = nodes[node].left;
        AABBKey right = nodes[node].right;

        if (nodes[node].isLeaf())
        {
            assert(left == NULL_NODE);
            assert(right == NULL_NODE);
            assert(nodes[node].height == 0);
            return;
        }

        assert(left < nodeCapacity);
        assert(right < nodeCapacity);

        assert(nodes[left].parent == node);
        assert(nodes[right].parent == node);

        validateStructure(left);
        validateStructure(right);
    }

    void Tree::validateMetrics(AABBKey node) const
    {
        if (node == NULL_NODE) return;

        AABBKey left = nodes[node].left;
        AABBKey right = nodes[node].right;

        if (nodes[node].isLeaf())
        {
            assert(left == NULL_NODE);
            assert(right == NULL_NODE);
            assert(nodes[node].height == 0);
            return;
        }

        assert(left < nodeCapacity);
        assert(right < nodeCapacity);

        int height1 = nodes[left].height;
        int height2 = nodes[right].height;
        int height = 1 + std::max(height1, height2);
        (void)height; // Unused variable in Release build
        assert(nodes[node].height == height);

        AABB aabb;
        aabb.merge(nodes[left].aabb, nodes[right].aabb);

        for (AABBKey i=0;i<dimension;i++)
        {
            assert(aabb.lowerBound[i] == nodes[node].aabb.lowerBound[i]);
            assert(aabb.upperBound[i] == nodes[node].aabb.upperBound[i]);
        }

        validateMetrics(left);
        validateMetrics(right);
    }

    void Tree::periodicBoundaries(std::vector<double>& position)
    {
        for (AABBKey i=0;i<dimension;i++)
        {
            if (position[i] < 0)
            {
                position[i] += boxSize[i];
            }
            else
            {
                if (position[i] >= boxSize[i])
                {
                    position[i] -= boxSize[i];
                }
            }
        }
    }

    bool Tree::minimumImage(std::vector<double>& separation, std::vector<double>& shift)
    {
        bool isShifted = false;

        for (AABBKey i=0;i<dimension;i++)
        {
            if (separation[i] < negMinImage[i])
            {
                separation[i] += periodicity[i]*boxSize[i];
                shift[i] = periodicity[i]*boxSize[i];
                isShifted = true;
            }
            else
            {
                if (separation[i] >= posMinImage[i])
                {
                    separation[i] -= periodicity[i]*boxSize[i];
                    shift[i] = -periodicity[i]*boxSize[i];
                    isShifted = true;
                }
            }
        }

        return isShifted;
    }
}