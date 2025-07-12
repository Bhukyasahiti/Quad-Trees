#include <iostream>
#include <cmath>
#include <memory>

using namespace std;

// Points
struct Point
{
    float x;
    float y;

    // Constructor
    Point(float x, float y) : x(x), y(y) {}
};

Point *newPoint(float x, float y)
{
    return new Point(x, y);
}

// Check if two points are equal
bool equal(Point *p1, Point *p2)
{
    return (p1->x == p2->x && p1->y == p2->y);
}

// Region
struct Region
{
    Point *p1; // top left
    Point *p2; // top right
    Point *p3; // bottom right
    Point *p4; // bottom left

    // Constructor
    Region(Point *p1, Point *p2, Point *p3, Point *p4)
        : p1(p1), p2(p2), p3(p3), p4(p4) {}
};

Region *newRegion(Point *p1, Point *p2, Point *p3, Point *p4)
{
    return new Region(p1, p2, p3, p4);
}

// Find if point is in region or not
bool isPointInRegion(Point *p, Region *r)
{
    return (p->x >= r->p1->x && p->x <= r->p2->x && p->y <= r->p1->y && p->y >= r->p4->y);
}

// Find which region point is in
int findRegion(Point *p, Region *r)
{
    float xmid = (r->p1->x + r->p2->x) / 2;
    float ymid = (r->p1->y + r->p4->y) / 2;
    if (p->x <= xmid && p->y >= ymid)
        return 1;
    else if (p->x >= xmid && p->y >= ymid)
        return 2;
    else if (p->x >= xmid && p->y <= ymid)
        return 3;
    else if (p->x <= xmid && p->y <= ymid)
        return 4;
    return 0;
}

int boxIntersectCheck(Region *b1, Region *b2)
{
    if (b2->p1->x > b1->p1->x && b2->p2->x < b1->p2->x && b2->p1->y < b1->p1->y && b2->p3->y > b1->p3->y)
        return 1;
    else if (b2->p1->x < b1->p1->x && b2->p2->x > b1->p2->x && b2->p1->y > b1->p1->y && b2->p3->y < b1->p3->y)
        return 2;
    else if (b1->p2->x >= b2->p1->x && b1->p2->x <= b2->p2->x)
    {
        if (b1->p3->y <= b2->p1->y && b1->p3->y >= b2->p3->y)
            return 1;
        else if (b1->p1->y <= b2->p1->y && b1->p1->y >= b2->p3->y)
            return 1;
    }
    else if (b1->p1->x <= b2->p2->x && b1->p1->x >= b2->p1->x)
    {
        if (b1->p1->y <= b2->p1->y && b1->p1->y >= b2->p3->y)
            return 1;
        else if (b1->p3->y <= b2->p1->y && b1->p3->y >= b2->p3->y)
            return 1;
    }
    return 0;
}

// Quad Tree
struct QuadTree
{
    Region *r;
    Point *p;
    unique_ptr<QuadTree> nw;
    unique_ptr<QuadTree> ne;
    unique_ptr<QuadTree> se;
    unique_ptr<QuadTree> sw;

    // Constructor
    QuadTree(Region *r, Point *p, unique_ptr<QuadTree> nw, unique_ptr<QuadTree> ne,
             unique_ptr<QuadTree> se, unique_ptr<QuadTree> sw)
        : r(r), p(p), nw(move(nw)), ne(move(ne)), se(move(se)), sw(move(sw)) {}
};

QuadTree *newQuadTree(Point *p1, Point *p2, Point *p3, Point *p4)
{
    return new QuadTree(newRegion(p1, p2, p3, p4), nullptr, nullptr, nullptr, nullptr, nullptr);
}

// Insert in quad tree
void insertQuadTree(QuadTree *qt, Point *p)
{
    if (!qt)
        return;

    if (isPointInRegion(p, qt->r))
    {
        if (!qt->p)
        {
            qt->p = p;
            Point *tophalf = newPoint((qt->r->p1->x + qt->r->p2->x) / 2, qt->r->p1->y);
            Point *bottomhalf = newPoint((qt->r->p1->x + qt->r->p2->x) / 2, qt->r->p4->y);
            Point *lefthalf = newPoint(qt->r->p1->x, (qt->r->p1->y + qt->r->p4->y) / 2);
            Point *righthalf = newPoint(qt->r->p2->x, (qt->r->p1->y + qt->r->p4->y) / 2);
            Point *center = newPoint((qt->r->p1->x + qt->r->p2->x) / 2, (qt->r->p1->y + qt->r->p4->y) / 2);
            qt->nw = unique_ptr<QuadTree>(newQuadTree(qt->r->p1, tophalf, center, lefthalf));
            qt->ne = unique_ptr<QuadTree>(newQuadTree(tophalf, qt->r->p2, righthalf, center));
            qt->se = unique_ptr<QuadTree>(newQuadTree(center, righthalf, qt->r->p3, bottomhalf));
            qt->sw = unique_ptr<QuadTree>(newQuadTree(lefthalf, center, bottomhalf, qt->r->p4));
            cout << "Point inserted\n";
        }
        else
        {
            if (isPointInRegion(p, qt->nw->r))
                insertQuadTree(qt->nw.get(), p);
            else if (isPointInRegion(p, qt->ne->r))
                insertQuadTree(qt->ne.get(), p);
            else if (isPointInRegion(p, qt->se->r))
                insertQuadTree(qt->se.get(), p);
            else if (isPointInRegion(p, qt->sw->r))
                insertQuadTree(qt->sw.get(), p);
        }
    }
    else
    {
        cout << "Point not in region\n";
    }
}

// Searching in quad tree
bool searchQuadTree(QuadTree *qt, Point *p)
{
    if (!qt->p)
        return false;

    if (equal(qt->p, p))
        return true;

    int region = findRegion(p, qt->r);
    if (region == 1)
        return searchQuadTree(qt->nw.get(), p);
    else if (region == 2)
        return searchQuadTree(qt->ne.get(), p);
    else if (region == 3)
        return searchQuadTree(qt->se.get(), p);
    else if (region == 4)
        return searchQuadTree(qt->sw.get(), p);

    return false;
}

void rangeQuery(QuadTree *qt, Region *re)
{
    if (!qt || !qt->p)
        return;

    if (boxIntersectCheck(qt->r, re) == 0)
        return;
    if (boxIntersectCheck(qt->r, re) == 2)
        re = qt->r;

    if (isPointInRegion(qt->p, re))
        cout << qt->p->x << " " << qt->p->y << endl;

    rangeQuery(qt->nw.get(), re);
    rangeQuery(qt->ne.get(), re);
    rangeQuery(qt->se.get(), re);
    rangeQuery(qt->sw.get(), re);
}

int main()
{
    int choice = 1;
    QuadTree *qt = nullptr;

    while (true)
    {
        cout << "1. Create QuadTree\n";
        cout << "2. Insert Point\n";
        cout << "3. Search Point\n";
        cout << "4. Range Query\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            float x1, y1, x2, y2, x3, y3, x4, y4;
            cout << "Enter coordinates of four points defining the region:\n";
            cout << "First point: ";
            cin >> x1 >> y1;
            cout << "Second point: ";
            cin >> x2 >> y2;
            cout << "Third point: ";
            cin >> x3 >> y3;
            cout << "Fourth point: ";
            cin >> x4 >> y4;
            qt = newQuadTree(newPoint(x1, y1), newPoint(x2, y2), newPoint(x3, y3), newPoint(x4, y4));
        }
        else if (choice == 2)
        {
            float x, y;
            cout << "Enter the point to be inserted:\n";
            cin >> x >> y;
            insertQuadTree(qt, newPoint(x, y));
        }
        else if (choice == 3)
        {
            float x, y;
            cout << "Enter the point to be searched:\n";
            cin >> x >> y;
            if (searchQuadTree(qt, newPoint(x, y)))
                cout << "Point found\n";
            else
                cout << "Point not found\n";
        }
        else if (choice == 4)
        {
            cout << "Enter region coordinates:\n";
            float x1, y1, x2, y2, x3, y3, x4, y4;
            cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;
            cout << "The points in the region are:\n";
            rangeQuery(qt, newRegion(newPoint(x1, y1), newPoint(x2, y2), newPoint(x3, y3), newPoint(x4, y4)));
        }
        else if (choice == 5)
        {
            break; // Exit the loop
        }
        else
        {
            cout << "Invalid input.\n";
        }
    }

    return 0;
}