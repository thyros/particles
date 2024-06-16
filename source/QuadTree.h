#pragma once
#include "State.h"
#include <memory>
#include <vector>

struct Boundry
{
    float x;
    float y;
    float width;
    float height;
};
bool Contains(const Boundry &b, const Position &p)
{
    return p.x >= b.x && p.y >= b.y &&
           p.x < b.x + b.width && p.y < b.y + b.height;
}
bool Overlaps(const Boundry &lhs, const Boundry &rhs)
{
    return false;
}

class QuadTree
{
public:
    QuadTree(Boundry boundry, size_t capacity) : mBoundry(std::move(boundry)), mCapacity(capacity)
    {
    }

    void insert(Position point)
    {
        if (!Contains(mBoundry, point))
        {
            return;
        }

        if (mPoints.size() < mCapacity)
        {
            mPoints.emplace_back(std::move(point));
        }
        else
        {
            if (!mDivided)
            {
                subdivide();
            }

            mNorthEast.insert(point);
            mNorthWest.insert(point);
            mSouthEast.insert(point);
            mSouthWest.insert(point);
        }
    }

    void query(const Boundry &boundry, std::vector<Position> &found)
    {
        if (!Overlaps(mBoundry, boundry))
        {
            return;
        }

        for (const Position &p, mPoints)
        {
            if (Contains(boundry, p))
            {
                found.push_back(p);
            }
        }

        if (mDivided)
        {
            mNorthEast.query(boundry, found);
            mNorthWest.query(boundry, found);
            mSouthEast.query(boundry, found);
            mSouthWest.query(boundry, found);
        }
    }

private:
    void subdivide()
    {
        const float x = mBoundry.x;
        const float y = mBoundry.y;
        const float hw = mBoundry.width / 2;
        const float hh = mBoundry.height / 2;

        mNorthEast = std::make_unique<QuadTree>(Boundry{x + hw, y - hh, hw, hh}, mCapacity);
        mNorthWest = std::make_unique<QuadTree>(Boundry{x - hw, y - hh, hw, hh}, mCapacity);
        mSouthEast = std::make_unique<QuadTree>(Boundry{x + hw, y + hh, hw, hh}, mCapacity);
        mSouthWest = std::make_unique<QuadTree>(Boundry{x - hw, y + hh, hw, hh}, mCapacity);

        mDivided = true;
    }

    Boundry mBoundry;
    std::vector<Position> mPoints;
    size_t mCapacity;
    bool mDivided = false;

    std::unique_ptr<QuadTree> mNorthWest;
    std::unique_ptr<QuadTree> mNorthEast;
    std::unique_ptr<QuadTree> mSouthWest;
    std::unique_ptr<QuadTree> mSouthEast;
};